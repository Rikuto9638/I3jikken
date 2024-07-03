import streamlit as st
import subprocess
import os
import signal
import time
from collections import deque
from ip_address import get_ip_address
import pandas as pd
import altair as alt
import csv

st.title('Internet App')


def load_data():
    if os.stat("data.csv").st_size == 0:
        return None
    data = pd.read_csv('data.csv', on_bad_lines='skip', usecols=[0, 1])
    # dataの型を数値に変換
    for col in data.columns:
        data[col] = pd.to_numeric(data[col], errors='coerce')
    # 読み取ったデータは消去
    open('data.csv', 'w').close()
    return data


def load_data():
    with open('data.csv', 'r') as f:
        reader = csv.reader(f)
        data = []
        for row in reader:
            try:
                # Convert each field to numeric
                data.append([float(field) for field in row])
            except ValueError:
                # Skip the row if an error occurred
                continue
    data = pd.DataFrame(data)
    open('data.csv', 'w').close()
    return data


def run():
    # 初期化
    if 'address' not in st.session_state:
        st.session_state.address = '10.100.10.10'

    if 'port' not in st.session_state:
        st.session_state.port = '50000'

    if 'server_or_client' not in st.session_state:
        st.session_state.server_or_client = 'Server'

    if "graph_start" not in st.session_state:
        st.session_state["graph_start"] = True

    if "user_input" not in st.session_state:
        st.session_state["user_input"] = ""

    if "user_ip_address" not in st.session_state:
        st.session_state["user_ip_address"] = get_ip_address()
    # 初期化終了

    # IPアドレスを表示
    st.markdown(
        f"My machine's IP address is: <span style='color:red'>**{st.session_state.user_ip_address}**</span>", unsafe_allow_html=True)
    # ServerかClientか選択
    st.session_state.server_or_client = st.radio(
        'Are you a server or a client?',
        options=['Server', 'Client']
    )

    # クライアントの場合、サーバのアドレスを入力
    if st.session_state.server_or_client == 'Client':
        st.session_state.address = st.text_input(
            'Input the server address', st.session_state.address)

    # ポートを選択
    st.session_state.port = st.selectbox(
        'Select port', ['50000', '50100', '50200', '50300', '50400', '50500'])

    # Connect/Disconnectボタン
    if st.button('Connect/Disconnect'):
        if 'proc' in st.session_state:  # コマンドが実行中の場合
            os.kill(st.session_state.proc.pid, signal.SIGTERM)
            st.write('Command stopped.')
            del st.session_state['proc']
        else:
            if st.session_state.server_or_client == 'Server':
                # サーバーを起動
                st.session_state.proc = subprocess.Popen(
                    ['./iphone11', st.session_state.port], stdin=subprocess.PIPE, stdout=subprocess.PIPE, bufsize=0)
            else:
                # クライアントとして接続
                st.session_state.proc = subprocess.Popen(
                    ['./iphone11', st.session_state.address, st.session_state.port], stdin=subprocess.PIPE, stdout=subprocess.PIPE, bufsize=0)

    if 'proc' in st.session_state:
        st.write('Connected!')
        st.session_state.start = True

    # テキスト入力
    st.session_state.user_input = st.text_input(
        "Enter some text:", st.session_state.user_input)
    if st.button("Send") and 'proc' in st.session_state:
        st.session_state.proc.stdin.write(
            st.session_state.user_input.encode('utf-8') + b'\n')
        st.session_state.proc.stdin.flush()

    # 人工音声の再生
    # "あいうえお"ボタン
    aiueo_hiragana = ["あ", "い", "う", "え", "お"]
    aiueo_romaji = ["a", "i", "u", "e", "o"]
    cols = st.columns(5)
    for i in range(5):
        with cols[i]:
            if st.button(aiueo_hiragana[i]) and 'proc' in st.session_state:
                st.session_state.proc.stdin.write(
                    aiueo_romaji[i].encode('utf-8') + b'\n')
    # sin_waveボタン
    if st.button("sin_wave") and 'proc' in st.session_state:
        st.session_state.proc.stdin.write(b"s\n")

    # データの可視化
    data = deque(maxlen=10000) # 10000個のデータを保持
    chart = st.empty()
    
    while True:
        new_data = load_data()

        # データがない場合はスキップ
        if new_data is None:
            continue

        # dequeに新しいデータを追加
        data.extend(new_data.values.tolist())

        # データをDataFrameに変換
        df = pd.DataFrame(data)
        if not df.empty:
            df.columns = ['voice of me', 'voice of the person'] + ['extra_column_{}'.format(i) for i in range(1, df.shape[1] - 1)]
            for col in df.columns:
                df[col] = pd.to_numeric(df[col], errors='coerce')

        # グラフの作成
        alt_chart = alt.Chart(df.reset_index()).transform_fold(
            ['voice of me', 'voice of the person'],
            as_=['variable', 'value']
        ).mark_line().encode(
            x='index:Q',
            y=alt.Y('value:Q', scale=alt.Scale(domain=(-32767, 32767))),
            color='variable:N'
        ).properties(
            width=800,
            height=400
        )

        # グラフの更新
        chart.altair_chart(alt_chart)

        # 0.1秒待機
        time.sleep(0.1)


if __name__ == "__main__":
    run()
