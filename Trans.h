/********************************************************
���ܣ���ͼ��������Ե����㣬ʵ��ͼ����ǿ
���룺
		IplImage* srcImg: Դ�Ҷ�ͼ��
		float a����ϵ��a
�����
		IplImage* dstImg������������Ա任���ͼ��
********************************************************/
#pragma once
#ifndef NONLINETRANS_H
#define NONLINETRANS_H
#include<opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp> 
#include<opencv2/highgui/highgui_c.h>
#include <opencv2/core/core.hpp>

class Trans {
private:

public:
	Trans(IplImage* srcImg, IplImage* dstImg, float a);
};
#endif // !NONLINETRANS_H

