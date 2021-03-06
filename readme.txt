【ソフト名】    Stella Vista
【バージョン】  1.0
【著作権者】    ライ
【制作日】      2016/7/31
【種　別】      フリーソフトウェア
【配布元】　　　github.com/ryechat/ImageViewer
【転載の可否】　可
【登 録 名】　　Stella Vista v1.0.exe
【動作環境】　　Windows XP, Vista以降
【開発環境】　　Visual Studio 2015, Windows10

―――――――――――――――――――――――――――――――――――――
≪著作権および免責事項≫

　本ソフトはフリーソフトです。個人／団体／社内利用を問わず、ご自由にお使い下さい。
　ソフトウェアおよびソースコードの著作権は作者が保有しています。

　このソフトウェアを使用したことによって生じたすべての障害・損害・不具
合等に関しては、作者は一切の責任を負いません。各自の責任においてご使用ください。


●はじめに
  Stella Vistaは、画像ファイルを閲覧できるWindows専用ソフトウェアです。
  exeファイル起動後のウィンドウに、画像ファイルをドロップすると表示します。
  マウスやキーボードを使って、同一フォルダにあるすべてのファイルを閲覧することができます。

●対応画像形式
  BMP, ICO, GIF, JPEG, Exif, PNG, TIFF, WMF, EMF.

●主な特長
  1)シンプルで分かり易いインターフェース
  2)マルチスレッドに対応した高速な画像展開
  3)マルチモニタ環境に対応

●インストール
　配布元サイトより圧縮ファイルをダウンロード、
　パスの通った任意の位置で展開します。
　exeファイルをダブルクリックするとアプリケーションが起動します。

　この際、アプリケーションのダウンロード数が少ないため、
　危険なファイルとしてWindows SmartScreenが実行をブロックする場合があります。
　詳細情報をクリックし、表示された実行ボタンをクリックすることで回避できます。

●アンインストール
  展開したファイルをすべて削除します。
  レジストリは使用していません。

●ソースコード
　Stella Vistaはオープンソースソフトウェアです。
　以下のサイトからソースコードをダウンロードできます。
　https://github.com/ryechat/ImageViewer

●レポジトリ構成
　setting.ini     (設定ファイル)
　language.ini    (言語ファイル)
　readme.txt　　　(この説明ファイル)
　basis           (ライブラリ)
　app             (ソースコード)
　stdafx.h/.cc    (プリコンパイル済みヘッダ用ソースファイル)

●プロジェクトのビルド
  ソースコードは、app および basis フォルダに含まれています。
 
  ビルドするには、これらすべての.ccファイルをコンパイル、リンクします。
　その際、basisフォルダを追加のインクルードディレクトリに指定する必要があります。
 
  必ずしもプリコンパイル済みヘッダは必要ではありません。

--以上--
