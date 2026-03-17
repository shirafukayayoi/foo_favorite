# foo_favorite

foobar2000 用のお気に入り管理コンポーネントです。  
右クリックメニューから曲をお気に入り登録/解除し、指定プレイリストへ同期します。

## 機能

- 右クリックメニュー
  - `Add to Favorites`
  - `Remove from Favorites`
- お気に入りプレイリスト自動同期
- （任意）お気に入り時にファイルをコピー
- （任意）解除時にコピー先ファイルを削除

## 仕様（重要）

- お気に入り判定は **`path + subsong` の完全一致** で管理します。  
  そのため、過去に発生していたハッシュ衝突による誤登録（例: `foo_youtube` 系で複数曲が同時登録される問題）を回避します。
- コンテキストメニュー操作は **先頭1曲のみ** を対象に処理します。

## ビルド

前提:

- Windows
- Visual Studio 2022 (MSBuild 利用可能)

実行:

```powershell
./build.ps1 -Configuration Release -Platform x64
```

生成物:

- `foobar2000/foo_favorite/_result/foo_favorite.fb2k-component`

## インストール

1. 生成された `foo_favorite.fb2k-component` を foobar2000 にインストール
2. foobar2000 を再起動
3. 曲を右クリックして `Add to Favorites` を実行

## 設定

Advanced Preferences の `Tools > Favorites` 配下:

- Playlist name
- Copy files on favorite
- Destination folder
- Skip existing destination files
- Delete copied file on unfavorite

## トラブルシュート

### プラグインが読み込まれない

- x64 版 foobar2000 を使用しているか確認
- コンポーネントを再インストール後、再起動

### 期待しない曲が Favorites に入る

- 最新版コンポーネントを再インストール
- Favorites プレイリストを一度空にしてから再登録

## 開発メモ

- エントリーファイル: `foobar2000/foo_favorite/foo_favorite.cpp`
- パッケージング: `build.ps1`
- `shared.dll` は実行時依存のためコンポーネントに同梱
