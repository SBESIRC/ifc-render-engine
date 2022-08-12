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

//位图文件头定义;
typedef struct  tagBITMAPFILEHEADER {
    //      WORD bfType;//单独读取，结构体中就不定义了
    DWORD bfSize;//文件大小
    WORD bfReserved1;//保留字
    WORD bfReserved2;//保留字
    DWORD bfOffBits;//从文件头到实际位图数据的偏移字节数
}BITMAPFILEHEADER;


typedef struct tagBITMAPINFOHEADER {
    DWORD biSize;//信息头大小
    DWORD biWidth;//图像宽度
    DWORD biHeight;//图像高度
    WORD biPlanes;//位平面数，必须为1
    WORD biBitCount;//每像素位数
    DWORD  biCompression; //压缩类型
    DWORD  biSizeImage; //压缩图像大小字节数
    DWORD  biXPelsPerMeter; //水平分辨率
    DWORD  biYPelsPerMeter; //垂直分辨率
    DWORD  biClrUsed; //位图实际用到的色彩数
    DWORD  biClrImportant; //本位图中重要的色彩数
}BITMAPINFOHEADER; //位图信息头定义

//像素信息
typedef struct tagIMAGEDATA
{
    BYTE blue;
    BYTE green;
    BYTE red;
}DATA;

//BMP整体信息
typedef struct tagBMP_BUFFER
{
    WORD                mbfType;
    BITMAPFILEHEADER    hand;
    BITMAPINFOHEADER    info;
    DATA* BUFFER;
}BMP_BUFFER;