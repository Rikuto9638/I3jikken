#pragma once

#define _USE_MATH_DEFINES
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* サンプリング周波数 */
#define SMPL    44100

/* 母音の種類 */
typedef enum {
    VOWEL_A,
    VOWEL_I,
    VOWEL_U,
    VOWEL_E,
    VOWEL_O
} VowelType;

#define MAX(a,b)    (((a)<(b))?(b):(a))
#define MIN(a,b)    (((a)<(b))?(a):(b))

double GenRosenberg(double freq)
{
    /* Rosenberg波を生成 */
    static double t = 0;
    double tau  = 0.90;  /* 声門開大期 */
    double tau2 = 0.05;  /* 声門閉小期 */
    double sample = 0.0;

    t += freq / (double)SMPL;
    t -= floor(t);
    if (t <= tau) {
        sample = 3.0*pow(t/tau,2.0)-2.0*pow(t/tau,3.0);
    } else if (t < tau+tau2) {
        sample = 1.0-pow((t-tau)/tau2,2.0);
    }
    return sample;
}

void IIR_DesignBPF(double f, double Q, double param[5])
{
    /* BPFのフィルタ係数を求める */
    double omega = 2.0 * M_PI * f / (double)SMPL;
    double alpha = sin(omega) / (2.0 * Q);
    double a0 = 1.0 + alpha;
    param[0] = alpha             / a0;
    param[1] = 0.0               / a0;
    param[2] = -alpha            / a0;
    param[3] = -2.0 * cos(omega) / a0;
    param[4] = (1.0 - alpha)     / a0;
}

double IIR_ApplyFilter(double base, const double param[5], double delay[4])
{
    /* IIRフィルタを適用 */
    double sample = 0.0;
    sample += param[0] * base;
    sample += param[1] * delay[0];
    sample += param[2] * delay[1];
    sample -= param[3] * delay[2];
    sample -= param[4] * delay[3];
    delay[1] = delay[0]; delay[0] = base;
    delay[3] = delay[2]; delay[2] = sample;
    return sample;
}

double ApplyFormant(double input, const double formant[5], double param[5][5], double delay[5][4])
{
    /* BPFでフォルマントを付加 */
    int i;
    double output = 0.0;
    for (i = 0; i < 5; i++) {
        IIR_DesignBPF(formant[i], 20.0, param[i]);
        output += IIR_ApplyFilter(input, param[i], delay[i]);
    }
    return output;
}

double get_freq(int freq_level){
    double freq;
    /* freq_levelはラからドまで10段階の周波数に対応 */
    switch(freq_level){/* 基本周波数 */
        case 0:
            freq = 220.0; /*ラ*/
            break;
        case 1:
            freq = 246.942; /*シ*/
            break;
        case 2:
            freq = 261.626; /*ド*/
            break;
        case 3:
            freq = 293.665; /*レ*/
            break;
        case 4:
            freq = 329.628; /*ミ*/
            break;
        case 5:
            freq = 349.228; /*ファ*/
            break;
        case 6:
            freq = 391.995; /*ソ*/
            break;
        case 7:
            freq = 440.0; /*ラ*/
            break;
        case 8:
            freq = 493.883; /*シ*/
            break;
        case 9:
            freq = 523.251; /*ド*/
            break;
    }
    return freq;
}

void sound(VowelType type, short* buf, int N, int freq_level){
    int i, size;
    double freq, vtlen, in, out;
    double formant[5], param[5][5], delay[5][4] = {0};
    double male[5][5] = {
        /* フォルマント周波数の変動値 */
        /*  F1,   F2,   F3,   F4,   F5 */
        { 1.60, 0.70, 1.10, 1.00, 1.00 },
        { 0.70, 1.40, 1.20, 1.00, 1.00 },
        { 0.80, 0.90, 0.90, 1.00, 1.00 },
        { 1.20, 1.30, 1.10, 1.00, 1.00 },
        { 1.15, 0.50, 1.20, 1.00, 1.00 }
    };

    /* 設定 */
    size  = N; /* 指定された数 */
    freq = get_freq(freq_level);  /* 基本周波数 */
    vtlen = 19.0;     /* 声道の長さ */

    /* フォルマント周波数を計算 */
    for (i = 0; i < 5; i++) {
        formant[i] = (34000.0*(2*i+1))/(4.0*vtlen);
        formant[i] *= male[type][i]; /* 母音を指定 */
    }

    /* 波形を生成 */
    for (i = 0; i < size; i++) {
        in = GenRosenberg(freq);
        out = ApplyFormant(in, formant, param, delay);
        buf[i] = (short)(32767 * MIN(MAX(out, -1.0), 1.0));
    }
}

void sin_sound(short* buf, int N, int freq_level){
    /* 正弦波を生成 */
    double freq = get_freq(freq_level);
    for(int i = 0; i < N; i ++){
        buf[i] = 32767 * sin(2 * M_PI * freq * i / SMPL);
    }
}