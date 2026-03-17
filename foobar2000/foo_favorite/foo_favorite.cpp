#include <foobar2000.h>
#include <advconfig_impl.h>

#include <sstream>
#include <string>
#include <unordered_set>

namespace
{

    static const GUID guid_component = {0x68d639f3, 0x9f5f, 0x4e63, {0x83, 0x22, 0x84, 0xad, 0x64, 0xcb, 0x98, 0x7f}};
    static const GUID guid_cfg_playlist_name = {0x92d8e74f, 0x2a10, 0x417b, {0x99, 0x84, 0x58, 0x10, 0x2a, 0x8f, 0x87, 0x3f}};
    static const GUID guid_cfg_copy_enabled = {0x9c6b5d45, 0xa451, 0x4c4f, {0xae, 0x6b, 0x89, 0x4e, 0x73, 0x43, 0x17, 0x65}};
    static const GUID guid_cfg_destination = {0x56a9930f, 0x5c5b, 0x4618, {0xa0, 0x81, 0x82, 0x19, 0xf5, 0x8f, 0xd9, 0x77}};
    static const GUID guid_cfg_skip_existing = {0xb46529c3, 0x95f6, 0x45e4, {0x9e, 0x1d, 0x9a, 0x17, 0xa0, 0xcd, 0xde, 0x89}};
    static const GUID guid_cfg_delete_on_unfavorite = {0x8c111b08, 0x4f8f, 0x45f5, {0x9f, 0xaf, 0x5c, 0xbf, 0x12, 0x81, 0xa5, 0xe8}};
    static const GUID guid_cfg_favorite_keys = {0x227a925f, 0xb7b9, 0x4dc4, {0x8a, 0x28, 0xf9, 0x4e, 0x96, 0xe3, 0x4e, 0xd9}};
    static const GUID guid_cfg_data_version = {0x1bf89616, 0x3af8, 0x4f34, {0xa2, 0x16, 0xc7, 0x75, 0xde, 0x93, 0x4b, 0x2a}};

    static constexpr t_uint32 favorite_data_version_current = 3;

    static const GUID guid_adv_branch = {0x7e53e4cb, 0x96df, 0x430a, {0x9e, 0x9e, 0x2c, 0x0f, 0x26, 0x8a, 0x04, 0x4a}};
    static const GUID guid_adv_playlist_name = {0x6db6f130, 0x8504, 0x4bc4, {0x9e, 0x81, 0x0a, 0xa1, 0xda, 0x2e, 0x60, 0x74}};
    static const GUID guid_adv_copy_enabled = {0xc715caac, 0xefff, 0x4534, {0x8f, 0x77, 0x67, 0x3d, 0xf0, 0xef, 0x76, 0xd6}};
    static const GUID guid_adv_destination = {0xc1b50186, 0xdc55, 0x42f6, {0xa8, 0x1f, 0x95, 0x7d, 0x8b, 0x77, 0x7e, 0x55}};
    static const GUID guid_adv_skip_existing = {0xd7bb04de, 0xbc2b, 0x4dce, {0x95, 0x10, 0xa8, 0x98, 0x7d, 0xbf, 0x95, 0xe7}};
    static const GUID guid_adv_delete_on_unfavorite = {0xf4e6351f, 0x4d44, 0x4e0a, {0x96, 0x9d, 0x44, 0x3c, 0x9b, 0x42, 0x2d, 0xdc}};

    static const GUID guid_context_add_favorite = {0xef6c0220, 0x8fd0, 0x4077, {0x95, 0x0f, 0xc0, 0x2d, 0x87, 0x2c, 0x46, 0x97}};
    static const GUID guid_context_remove_favorite = {0x24df2fd7, 0x4c49, 0x4adf, {0xa6, 0xd8, 0x97, 0xee, 0xc8, 0xb8, 0x3d, 0x48}};

    cfg_string cfg_playlist_name(guid_cfg_playlist_name, "Favorites");
    cfg_bool cfg_copy_enabled(guid_cfg_copy_enabled, false);
    cfg_string cfg_destination(guid_cfg_destination, "");
    cfg_bool cfg_skip_existing(guid_cfg_skip_existing, true);
    cfg_bool cfg_delete_on_unfavorite(guid_cfg_delete_on_unfavorite, false);
    cfg_string cfg_favorite_keys(guid_cfg_favorite_keys, "");
    cfg_uint cfg_favorite_data_version(guid_cfg_data_version, 0);

    advconfig_branch_factory g_adv_branch("Favorites", guid_adv_branch, advconfig_entry::guid_branch_tools, 0);
    advconfig_string_factory g_adv_playlist_name(
        "Playlist name",
        "favorites.playlist_name",
        guid_adv_playlist_name,
        guid_adv_branch,
        0,
        "Favorites");
    advconfig_checkbox_factory g_adv_copy_enabled(
        "Copy files on favorite",
        "favorites.copy_enabled",
        guid_adv_copy_enabled,
        guid_adv_branch,
        1,
        false);
    advconfig_string_factory g_adv_destination(
        "Destination folder",
        "favorites.destination",
        guid_adv_destination,
        guid_adv_branch,
        2,
        "");
    advconfig_checkbox_factory g_adv_skip_existing(
        "Skip existing destination files",
        "favorites.skip_existing",
        guid_adv_skip_existing,
        guid_adv_branch,
        3,
        true);
    advconfig_checkbox_factory g_adv_delete_on_unfavorite(
        "Delete copied file on unfavorite",
        "favorites.delete_on_unfavorite",
        guid_adv_delete_on_unfavorite,
        guid_adv_branch,
        4,
        false);

    std::unordered_set<std::string> g_favorite_keys;

    std::string make_favorite_key(const playable_location &location)
    {
        std::string key = location.get_path();
        key.push_back('\t');
        key += std::to_string((unsigned)location.get_subsong_index());
        return key;
    }

    bool get_favorite_key(const metadb_handle_ptr &handle, std::string &out)
    {
        try
        {
            out = make_favorite_key(handle->get_location());
            return !out.empty();
        }
        catch (...)
        {
            return false;
        }
    }

    void save_favorite_keys_cfg()
    {
        pfc::string_formatter blob;
        for (const auto &key : g_favorite_keys)
        {
            blob << key.c_str() << "\n";
        }
        cfg_favorite_keys = blob;
    }

    void load_favorite_keys_cfg()
    {
        g_favorite_keys.clear();

        std::istringstream stream((const char *)cfg_favorite_keys);
        std::string line;
        while (std::getline(stream, line))
        {
            if (line.empty())
                continue;
            g_favorite_keys.insert(line);
        }
    }

    void migrate_favorite_data_if_needed()
    {
        if ((t_uint32)cfg_favorite_data_version >= favorite_data_version_current)
            return;

        g_favorite_keys.clear();
        save_favorite_keys_cfg();
        cfg_favorite_data_version = favorite_data_version_current;
    }

    bool is_favorite_handle(const metadb_handle_ptr &handle)
    {
        std::string key;
        return get_favorite_key(handle, key) && g_favorite_keys.find(key) != g_favorite_keys.end();
    }

    pfc::string8 get_playlist_name()
    {
        pfc::string8 name = cfg_playlist_name.get();
        if (name.is_empty())
            name = "Favorites";
        return name;
    }

    pfc::string8 get_destination_root()
    {
        pfc::string8 root = cfg_destination.get();
        return root;
    }

    class favorite_file_worker : public threaded_process_callback
    {
    public:
        enum class mode_t
        {
            copy,
            remove
        };

        favorite_file_worker(metadb_handle_list_cref handles, mode_t mode) : m_mode(mode)
        {
            m_handles.add_items(handles);
        }

        void run(threaded_process_status &status, abort_callback &abort) override
        {
            const pfc::string8 rootNative = get_destination_root();
            if (rootNative.is_empty())
                return;

            pfc::string8 rootCanonical;
            filesystem::g_get_canonical_path(rootNative, rootCanonical);
            try
            {
                filesystem::g_create_directory(rootCanonical, abort);
            }
            catch (exception_io_already_exists const &)
            {
            }

            for (t_size i = 0; i < m_handles.get_count(); ++i)
            {
                abort.check();
                status.set_progress(i, m_handles.get_count());

                const metadb_handle_ptr &handle = m_handles.get_item(i);
                pfc::string8 srcCanonical;
                filesystem::g_get_canonical_path(handle->get_path(), srcCanonical);
                status.set_item_path(srcCanonical);

                pfc::string8 srcNative = filesystem::g_get_native_path(srcCanonical, abort);
                pfc::string8 fileName = pfc::io::path::getFileName(srcNative);
                if (fileName.is_empty())
                    continue;

                const pfc::string8 dstNative = pfc::io::path::combine(rootNative, fileName);
                if (!pfc::io::path::isInsideDirectory(rootNative, dstNative))
                    continue;

                pfc::string8 dstCanonical;
                filesystem::g_get_canonical_path(dstNative, dstCanonical);

                if (m_mode == mode_t::copy)
                {
                    if (cfg_skip_existing && filesystem::g_exists(dstCanonical, fb2k::noAbort))
                        continue;
                    filesystem::g_copy(srcCanonical, dstCanonical, abort);
                }
                else
                {
                    if (filesystem::g_exists(dstCanonical, fb2k::noAbort))
                    {
                        filesystem::g_remove(dstCanonical, abort);
                    }
                }
            }
        }

    private:
        metadb_handle_list m_handles;
        mode_t m_mode;
    };

    void run_copy_worker(metadb_handle_list_cref handles)
    {
        if (!cfg_copy_enabled)
            return;
        if (get_destination_root().is_empty())
            return;

        threaded_process::g_run_modeless(
            fb2k::service_new<favorite_file_worker>(handles, favorite_file_worker::mode_t::copy),
            threaded_process::flag_show_abort | threaded_process::flag_show_progress | threaded_process::flag_show_item,
            core_api::get_main_window(),
            "Copying favorites");
    }

    void run_remove_worker(metadb_handle_list_cref handles)
    {
        if (!cfg_delete_on_unfavorite)
            return;
        if (get_destination_root().is_empty())
            return;

        threaded_process::g_run_modeless(
            fb2k::service_new<favorite_file_worker>(handles, favorite_file_worker::mode_t::remove),
            threaded_process::flag_show_abort | threaded_process::flag_show_progress | threaded_process::flag_show_item,
            core_api::get_main_window(),
            "Removing favorites");
    }

    void collect_handles_fallback(std::unordered_set<std::string> const &favoriteLookup, metadb_handle_list_ref out)
    {
        auto pm = playlist_manager::get();
        const t_size playlistCount = pm->get_playlist_count();

        for (t_size playlist = 0; playlist < playlistCount; ++playlist)
        {
            metadb_handle_list items;
            pm->playlist_get_all_items(playlist, items);
            for (t_size i = 0; i < items.get_count(); ++i)
            {
                const auto &handle = items.get_item(i);
                std::string key;
                if (!get_favorite_key(handle, key))
                    continue;
                if (favoriteLookup.find(key) == favoriteLookup.end())
                    continue;
                out.add_item(handle);
            }
        }
    }

    void sync_playlist_from_index()
    {
        std::unordered_set<std::string> favoriteLookup = g_favorite_keys;

        metadb_handle_list handles;
        if (favoriteLookup.size() > 0)
        {
            collect_handles_fallback(favoriteLookup, handles);
        }

        metadb_handle_list_helper::sort_by_pointer_remove_duplicates(handles);

        auto pm = playlist_manager::get();
        const pfc::string8 playlistName = get_playlist_name();
        const t_size playlist = pm->find_or_create_playlist(playlistName);
        pm->playlist_update_content(playlist, handles, true);
    }

    void set_favorite_for_handles(metadb_handle_list_cref handles, bool favorite)
    {
        bool changed = false;

        for (t_size i = 0; i < handles.get_count(); ++i)
        {
            std::string key;
            if (!get_favorite_key(handles.get_item(i), key))
                continue;

            const bool wasFavorite = g_favorite_keys.find(key) != g_favorite_keys.end();
            if (wasFavorite == favorite)
                continue;

            if (favorite)
                g_favorite_keys.insert(key);
            else
                g_favorite_keys.erase(key);

            changed = true;
        }

        if (changed)
        {
            save_favorite_keys_cfg();
            sync_playlist_from_index();
        }

        if (favorite)
        {
            run_copy_worker(handles);
        }
        else
        {
            run_remove_worker(handles);
        }
    }

    void set_favorite_for_first_handle(metadb_handle_list_cref handles, bool favorite)
    {
        if (handles.get_count() == 0)
            return;
        metadb_handle_list one;
        one.add_item(handles.get_item(0));
        set_favorite_for_handles(one, favorite);
    }

    void init_before_config_read()
    {
        load_favorite_keys_cfg();
        migrate_favorite_data_if_needed();
    }

    void init_after_ui()
    {
        sync_playlist_from_index();
    }

    FB2K_ON_INIT_STAGE(init_before_config_read, init_stages::before_config_read);
    FB2K_ON_INIT_STAGE(init_after_ui, init_stages::after_ui_init);

    class favorite_contextmenu_items : public contextmenu_item_simple
    {
    public:
        enum
        {
            cmd_add = 0,
            cmd_remove,
            cmd_count
        };

        unsigned get_num_items() override
        {
            return cmd_count;
        }

        void get_item_name(unsigned index, pfc::string_base &out) override
        {
            switch (index)
            {
            case cmd_add:
                out = "Add to Favorites";
                return;
            case cmd_remove:
                out = "Remove from Favorites";
                return;
            default:
                uBugCheck();
            }
        }

        bool get_item_description(unsigned index, pfc::string_base &out) override
        {
            switch (index)
            {
            case cmd_add:
                out = "Mark selected tracks as favorites.";
                return true;
            case cmd_remove:
                out = "Remove selected tracks from favorites.";
                return true;
            default:
                return false;
            }
        }

        GUID get_item_guid(unsigned index) override
        {
            switch (index)
            {
            case cmd_add:
                return guid_context_add_favorite;
            case cmd_remove:
                return guid_context_remove_favorite;
            default:
                uBugCheck();
            }
        }

        GUID get_parent() override
        {
            return contextmenu_groups::root;
        }

        void context_command(unsigned index, metadb_handle_list_cref data, const GUID &) override
        {
            switch (index)
            {
            case cmd_add:
                set_favorite_for_first_handle(data, true);
                return;
            case cmd_remove:
                set_favorite_for_first_handle(data, false);
                return;
            default:
                uBugCheck();
            }
        }

        bool context_get_display(unsigned index, metadb_handle_list_cref data, pfc::string_base &out, unsigned &displayFlags, const GUID &) override
        {
            displayFlags = 0;
            get_item_name(index, out);

            bool anyFavorite = false;
            bool anyNonFavorite = false;
            bool anyHashable = false;

            for (t_size i = 0; i < data.get_count(); ++i)
            {
                std::string key;
                if (!get_favorite_key(data.get_item(i), key))
                    continue;
                anyHashable = true;
                if (g_favorite_keys.find(key) != g_favorite_keys.end())
                    anyFavorite = true;
                else
                    anyNonFavorite = true;
            }

            if (!anyHashable)
                return false;
            if (index == cmd_add)
                return anyNonFavorite;
            if (index == cmd_remove)
                return anyFavorite;
            return false;
        }
    };

    contextmenu_item_factory_t<favorite_contextmenu_items> g_favorite_contextmenu_items;

    DECLARE_COMPONENT_VERSION(
        "Favorites",
        "1.0",
        "Stores favorite state in metadb_index and syncs a Favorites playlist.");
    VALIDATE_COMPONENT_FILENAME("foo_favorite.dll");

}