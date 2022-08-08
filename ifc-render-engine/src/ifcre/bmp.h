#pragma once
#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstdlib>

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef long LONG;

//λͼ�ļ�ͷ����;
typedef struct  tagBITMAPFILEHEADER {
    //      WORD bfType;//������ȡ���ṹ���оͲ�������
    DWORD bfSize;//�ļ���С
    WORD bfReserved1;//������
    WORD bfReserved2;//������
    DWORD bfOffBits;//���ļ�ͷ��ʵ��λͼ���ݵ�ƫ���ֽ���
}BITMAPFILEHEADER;


typedef struct tagBITMAPINFOHEADER {
    DWORD biSize;//��Ϣͷ��С
    DWORD biWidth;//ͼ����
    DWORD biHeight;//ͼ��߶�
    WORD biPlanes;//λƽ����������Ϊ1
    WORD biBitCount;//ÿ����λ��
    DWORD  biCompression; //ѹ������
    DWORD  biSizeImage; //ѹ��ͼ���С�ֽ���
    DWORD  biXPelsPerMeter; //ˮƽ�ֱ���
    DWORD  biYPelsPerMeter; //��ֱ�ֱ���
    DWORD  biClrUsed; //λͼʵ���õ���ɫ����
    DWORD  biClrImportant; //��λͼ����Ҫ��ɫ����
}BITMAPINFOHEADER; //λͼ��Ϣͷ����

//������Ϣ
typedef struct tagIMAGEDATA
{
    BYTE blue;
    BYTE green;
    BYTE red;
}DATA;

//BMP������Ϣ
typedef struct tagBMP_BUFFER
{
    WORD                mbfType;
    BITMAPFILEHEADER    hand;
    BITMAPINFOHEADER    info;
    DATA* BUFFER;
}BMP_BUFFER;