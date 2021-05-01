<!--
# @title BCDice IRC
-->
# BCDice IRC Qt

[![Build Status](https://travis-ci.com/bcdice/bcdice-irc.svg?branch=master)](https://travis-ci.com/bcdice/bcdice-irc)

BCDice IRC Qtは、TRPG向けダイスエンジン[BCDice](https://bcdice.org/)を搭載したIRCボットです。このボットを使用すると、IRCでのチャットにおいて、ダイスロールをはじめとするBCDiceのオンラインセッション支援機能を利用できます。

## 1. 主な機能

* ダイスロール：2D6などの基本的なダイスロールに加えて、[100種類以上のゲームシステムの固有処理にも対応](https://bcdice.org/systems/)。
* 四則計算：経験値の計算などに。
* プロットの登録・開示
* ポイントカウンタ：イニシアティヴやHP、MP、衝動蓄積など、セッション内で共有したい数値を管理します。
* カード：ゴーストハンターなどの、プレイヤーにカードを配るシステム用の機能。

詳細については、[BCDiceの旧README](https://github.com/bcdice/BCDice/blob/master/docs/README.txt)も参照してください。

## 2. 動作環境

BCDice IRCは、チャットの参加メンバーのうち1人のPCで動作させて使用します。このPCには、以下のソフトウェアが必要です。

* OS：Windows 10（64ビット版）、GNU/Linux、macOSのいずれか（ただし、現在はmacOSでの動作は不安定）
* [Ruby](https://www.ruby-lang.org/) 2.6以降（BCDiceがRubyで書かれているため）

## 3. ビルド手順

GUIはQtで書かれています。開発環境ではQt 6.0およびQt Creatorが使われています。ビルドする場合はまずこれらをインストールしてください。

### 3.1. gRPCのインストール

Rubyプログラムとの通信にgRPCを使用しています。gRPC公式サイトの「[C++ Quick Start#Build and locally install gRPC and Protocol Buffers](https://grpc.io/docs/languages/cpp/quickstart/#install-grpc)」を参考にして、gRPCをローカルインストールしてください（インストール先は `$HOME/grpc-1.37.1` など）。以下では、gRPCのインストール先を `$GRPC_INSTALL_DIR` と表記します。

### 3.2. `CMAKE_PREFIX_PATH` の設定

Qt Creatorで本ディレクトリをプロジェクトとして読み込みます。読み込み後、ビルド時にgRPCを見つけられるように、ビルド設定において `CMAKE_PREFIX_PATH` を設定します。

Qt Creator左側の「Projects」からビルドに使用するキットを選択して、「CMake」欄の変数を変更します。`CMAKE_PREFIX_PATH`（複数のディレクトリを `;` 区切りで指定可能）を以下のように設定します。実際に値を入力する際は、変数を展開してください。

```
${GRPC_INSTALL_DIR};${QT_INSTALL_PREFIX}
```
