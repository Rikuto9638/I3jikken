# I3jikken
EEIC3年の前期実験I3課題で作成したインターネット電話です。

実行中の画面は"sample.png"にあります。また、作成した合成音声のサンプルは"sample.wav"から聴くことができます。

# 実行方法
pythonおよびgccが利用できる環境が必要です。

もしstreamlitがインストールされていない場合は、コマンドラインで以下のコマンドを実行してください。
```
pip install streamlit
```

次のコマンドでプログラムを立ち上げることができます。
```
#ubuntuの場合
python3 -m streamlit run app.py

#それ以外の場合
streamlit run app.py
```

これを実行するとセキュリティーエラーが出ることがあります。その場合は、同じディレクトリで以下を実行してから上記のコマンドをもう一度実行してください。
```
gcc -o iphone11 iphone11.c 
```

以上が成功すると、"sample.png"にあるような画面がブラウザで立ち上がります。

サーバ側はポート番号を指定して`Connect/Disconnect`をクリックし、待機状態に入ります。クライアント側は通話したい相手のIPアドレスとポート番号を入力し、`Connect/Disconnect`をクリックすると、通話が開始します。

通話中に`あ` `い` `う` `え` `お` `sin_wave`のいずれかをクリックすると、対応した音声を相手に送信することができます。また、"enter some text"の部分に0から9までの数字を入れて`send`をクリックすると、送信する音の高さを変えることができます。

# 参考文献
* 東京大学工学部電子情報工学科電子電子工学科編　電気電子情報第一（前期）実験テキスト
* [C言語で音声合成もどき ～母音の生成～](https://moge32.blogspot.com/2012/08/3c.html)
* [ドレミの振動数！ピアノ88鍵全音域の周波数一覧表](https://inalesson.com/frequency_list/2417/)
* [streamlitを使ったお手軽Webアプリ開発](https://qiita.com/sypn/items/80962d84126be4092d3c)
