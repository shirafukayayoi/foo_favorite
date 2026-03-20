# foo_favorite

A Favorites management component for foobar2000.  
You can add/remove tracks from Favorites via the context menu and keep them synced to a specified playlist.

## Features

- Context menu actions
  - `Add to Favorites`
  - `Remove from Favorites`
- Automatic synchronization with the Favorites playlist
- (Optional) Copy files when favoriting
- (Optional) Delete copied destination files when unfavoriting

## Important Behavior

- Favorite identity is managed by an exact match of **`path + subsong`**.  
  This avoids false registrations caused by hash collisions seen in older behavior (for example, multiple `foo_youtube` tracks being added at once).
- Context menu operations currently process **only the first selected track**.

## Build

Prerequisites:

- Windows
- Visual Studio 2022 (MSBuild available)

Run:

```powershell
./build.ps1 -Configuration Release -Platform x64
```

Output:

- `foobar2000/foo_favorite/_result/foo_favorite.fb2k-component`

## Installation

1. Install the generated `foo_favorite.fb2k-component` into foobar2000.
2. Restart foobar2000.
3. Right-click a track and run `Add to Favorites`.

## Configuration

Under Advanced Preferences: `Tools > Favorites`

- Playlist name
- Copy files on favorite
- Destination folder
- Skip existing destination files
- Delete copied file on unfavorite

## Troubleshooting

### Plugin does not load

- Confirm you are using x64 foobar2000.
- Reinstall the component, then restart foobar2000.

### Unexpected tracks are added to Favorites

- Reinstall the latest component build.
- Clear the Favorites playlist once, then register tracks again.

## Development Notes

- Entry file: `foobar2000/foo_favorite/foo_favorite.cpp`
- Packaging script: `build.ps1`
- `shared.dll` is bundled as a runtime dependency.

## License

- foobar2000 SDK: `sdk-license.txt`
- pfc: `pfc/pfc-license.txt`
- Third-party summary: `THIRD_PARTY_NOTICES.md`

---

## 日本語

foobar2000 用のお気に入り管理コンポーネントです。  
右クリックメニューから曲をお気に入り登録/解除し、指定プレイリストへ同期します。

### 機能

- 右クリックメニュー
  - `Add to Favorites`
  - `Remove from Favorites`
- お気に入りプレイリスト自動同期
- （任意）お気に入り時にファイルをコピー
- （任意）解除時にコピー先ファイルを削除

### 仕様（重要）

- お気に入り判定は **`path + subsong` の完全一致** で管理します。  
  そのため、過去に発生していたハッシュ衝突による誤登録（例: `foo_youtube` 系で複数曲が同時登録される問題）を回避します。
- コンテキストメニュー操作は **先頭1曲のみ** を対象に処理します。

### ビルド

前提:

- Windows
- Visual Studio 2022 (MSBuild 利用可能)

実行:

```powershell
./build.ps1 -Configuration Release -Platform x64
```

生成物:

- `foobar2000/foo_favorite/_result/foo_favorite.fb2k-component`

### インストール

1. 生成された `foo_favorite.fb2k-component` を foobar2000 にインストール
2. foobar2000 を再起動
3. 曲を右クリックして `Add to Favorites` を実行

### 設定

Advanced Preferences の `Tools > Favorites` 配下:

- Playlist name
- Copy files on favorite
- Destination folder
- Skip existing destination files
- Delete copied file on unfavorite

### トラブルシュート

#### プラグインが読み込まれない

- x64 版 foobar2000 を使用しているか確認
- コンポーネントを再インストール後、再起動

#### 期待しない曲が Favorites に入る

- 最新版コンポーネントを再インストール
- Favorites プレイリストを一度空にしてから再登録

### 開発メモ

- エントリーファイル: `foobar2000/foo_favorite/foo_favorite.cpp`
- パッケージング: `build.ps1`
- `shared.dll` は実行時依存のためコンポーネントに同梱
