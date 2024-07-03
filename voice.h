#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "sound.h"
#include "input.h"

typedef short sample_t;

int process_key(char key, int* sound_n, int* freq_level){

    int repeat = 10; // 音を流す回数。10の場合は1秒間流れる。

    // sound_nはそれぞれの音を繰り返す残りの回数を保持
    if(key == 'a'){
        printf("pressed a\n");
        sound_n[0] = repeat;
        sound_n[1] = 0;
        sound_n[2] = 0;
        sound_n[3] = 0;
        sound_n[4] = 0;
        sound_n[5] = 0;
    }
    else if(key == 'i'){
        printf("pressed i\n");
        sound_n[0] = 0;
        sound_n[1] = repeat;
        sound_n[2] = 0;
        sound_n[3] = 0;
        sound_n[4] = 0;
        sound_n[5] = 0;
    }
    else if(key == 'u'){
        printf("pressed u\n");
        sound_n[0] = 0;
        sound_n[1] = 0;
        sound_n[2] = repeat;
        sound_n[3] = 0;
        sound_n[4] = 0;
        sound_n[5] = 0;
    }
    else if(key == 'e'){
        printf("pressed e\n");
        sound_n[0] = 0;
        sound_n[1] = 0;
        sound_n[2] = 0;
        sound_n[3] = repeat;
        sound_n[4] = 0;
        sound_n[5] = 0;
    }
    else if(key == 'o'){
        printf("pressed o\n");
        sound_n[0] = 0;
        sound_n[1] = 0;
        sound_n[2] = 0;
        sound_n[3] = 0;
        sound_n[4] = repeat;
        sound_n[5] = 0;
    }
    else if(key == 's'){
        printf("pressed s\n");
        sound_n[0] = 0;
        sound_n[1] = 0;
        sound_n[2] = 0;
        sound_n[3] = 0;
        sound_n[4] = 0;
        sound_n[5] = repeat;
    }
    else{
        for(int i = 0; i < 10; i++){
            if(key - '0' == i){
                *freq_level = i;
                break;
            }
        }
    }

    int mode = 6; // デフォルトではマイクの音を送る
    for(int i = 0; i < 6; i++){
        if(sound_n[i] > 0){
            mode = i; // 0は「あ」、1は「い」、・・・、5は「お」、6は正弦波に対応
        }
    }
    return mode;
}

void handle_voice_call(int s) {
    // popenを使って録音と再生をコード内で実行
    FILE* fp_rec = popen("rec -t raw -b 16 -c 1 -e s -r 44100 - ", "r");
    FILE* fp_play = popen("play -t raw -b 16 -c 1 -e s -r 44100 - ", "w");
    if (fp_rec == NULL || fp_play == NULL) {
        perror("popen");
        exit(1);
    }

    int N = 4410; // 一度に送信/受信するデータ数（0.1秒分）

    sample_t buffer_rec[N];
    char buffer_rec_char[N*2];
    sample_t buffer_play[N];
    char buffer_play_char[N*2];
    
    const int NUM_VOWELS = 5; // 5つの母音に対応する定数
    sample_t buffers[NUM_VOWELS + 1][10][N]; // 三次元配列での宣言。長さNの波形を10段階の高さ、6種類の音で作成。

    // ループを使って初期化
    for (int vowel = 0; vowel < NUM_VOWELS; ++vowel) {
        for (int index = 0; index < 10; ++index) { // indexは10段階の音の高さに対応
            sound(vowel, buffers[vowel][index], N, index); // 5つの母音の波形を予めbufferに追加。参照：sound.h
        }
    }
    for (int index = 0; index < 10; ++index) {
        sin_sound(buffers[5][index], N, index); // 正弦波をbufferに追加。参照：sound.h
    }

    int* sound_n = (int*)calloc(6,sizeof(int));
    char key = 0;
    int n_send = 0;
    int mode = 0;
    int freq_level = 0;

    while(1) {
        key = getkey();
        mode = process_key(key,sound_n,&freq_level);
        n_send = N;
        if(mode == 6){ //modeが6のときはマイクの音声を送る
            n_send = fread(buffer_rec, sizeof(sample_t), N, fp_rec);
            if (n_send == -1) {
                perror("read");
                exit(1);
            } else if (n_send == 0) {
                break;
            }
        }
        else{
            memcpy(buffer_rec,buffers[mode][freq_level],sizeof(sample_t)*N);
            sound_n[mode]--;
        }
       
        // shortからcharに変換
        for(int i=0; i<N; i++){
            buffer_rec_char[i*2] = buffer_rec[i] & 0xff;
            buffer_rec_char[i*2+1] = (buffer_rec[i] >> 8) & 0xff;
        }

        // char型で送信
        send(s, buffer_rec_char, n_send*2, 0);

        // char型で受信
        int n_read_char = recv(s, buffer_play_char, N*2, 0);
        if (n_read_char == -1) {
            perror("read");
            exit(1);
        } else if (n_read_char == 0) {
            break;
        }

        // charからshortに変換
        for (int i = 0; i < n_read_char/2; i++) {
            buffer_play[i] = (buffer_play_char[i*2] & 0xff) | ((buffer_play_char[i*2+1] & 0xff) << 8);
        }
        int n_read = n_read_char/2;

        // 送られてきた音声を再生
        fwrite(buffer_play, sizeof(sample_t), n_read, fp_play);

        // buffer_recとbuffer_playをdata.csvに書き込む
        FILE* fp = fopen("data.csv", "a");
        if (fp == NULL) {
            perror("fopen");
            exit(1);
        }
        for (int i = 0; i < n_read; i++) {
            fprintf(fp, "%d,%d\n", buffer_rec[i], buffer_play[i]);
        }
    }
    pclose(fp_rec);
    pclose(fp_play);
    free(sound_n);
    close(s);
}