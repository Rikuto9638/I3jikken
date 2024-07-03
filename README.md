# I3jikken
EEIC3年の前期実験I3の課題です。

実行中の画面は"sample.png"にあります。また、作成した合成音声のサンプルは"sample.wav"から聴くことができます。

# 実行方法
pythonおよびgccが利用できる環境が必要です。

次のコマンドでプログラムを立ち上げることができます。
```
#ubuntuの場合
python3 -m streamlit run app.py

#それ以外の場合
streamlit run app.py
```
もしstreamlitがインストールされていない場合は、コマンドラインで以下のコマンドを実行してください。
```
pip install streamlit
```
これを実行するとセキュリティーエラーが出ることがあります。その場合は、同じディレクトリで以下を実行してください。
```
gcc -o iphone11 iphone11.c 
```

# 参考文献
* 東京大学工学部電子情報工学科電子電子工学科編　電気電子情報第一（前期）実験テキスト
* [C言語で音声合成もどき ～母音の生成～](https://www.citedrive.com/ja/blog/how-to-cite-a-website-in-bibtex/)
* [ドレミの振動数！ピアノ88鍵全音域の周波数一覧表](https://inalesson.com/frequency_list/2417/)
