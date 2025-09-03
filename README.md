# hmPy

![hmPy v3.0.2](https://img.shields.io/badge/hmPy-v3.0.2-6479ff.svg)
[![Apache 2.0](https://img.shields.io/badge/license-Apache_2.0-blue.svg?style=flat)](LICENSE)
![Hidemaru 8.73](https://img.shields.io/badge/Hidemaru-v8.73-6479ff.svg)
![IronPython 3.4.11](https://img.shields.io/badge/IronPython-v3.4.11-6479ff.svg?logo=python&logoColor=white)

`hmPy`は、高機能テキストエディタ「秀丸エディタ」のマクロ機能からIronPythonをシームレスに利用するための連携コンポーネントです。秀丸マクロの強力なエディタ操作機能と、Pythonの柔軟で強力なスクリプティング能力を組み合わせることができます。

より詳しい情報は、ウェブサイトを参照してください。
[https://秀丸マクロ.net/?page=nobu_tool_hm_ironpython](https://秀丸マクロ.net/?page=nobu_tool_hm_ironpython)

## 主な機能

*   **秀丸マクロからのPython実行:** 秀丸マクロから直接Pythonスクリプト（`.py`ファイル）の実行や、Pythonコードの文字列を評価させることが可能です。
*   **秀丸の機能へのアクセス:** Pythonスクリプト内から、`hm`というオブジェクトを通じて、秀丸エディタの膨大なマクロコマンドや関数、変数にアクセスできます。
*   **Pythonラッパーライブラリ:** `hmPy.py`を利用することで、秀丸の機能をよりPythonらしい自然な記述で利用できます。

## アーキテクチャ概要

`hmPy`は、以下のコンポーネントから構成されています。

1.  **`hmPy.dll` (C++/CLI):** 秀丸エディタから直接ロードされるDLL。ネイティブコードである秀丸マクロと、マネージドコード(.NET)であるC#ライブラリとの橋渡しを行います。
2.  **`hmPyStaticLib` (C#):** IronPythonの実行エンジンをホストし、Pythonスクリプトの評価・実行を担う中心的なライブラリです。`hm`オブジェクトをPythonスクリプトに公開します。
3.  **`hmPy.py` (Python):** `hm`オブジェクトの機能をラップし、Python開発者にとってより直感的で使いやすいインターフェースを提供するヘルパーライブラリです。

これらのコンポーネントが連携することで、秀丸マクロとIronPythonのシームレスな統合を実現しています。
