/***************************************
���ܣ�����ͼ���ֱ��ͼ
���룺ͼ��ָ�룬���ֱ��ͼ�������׵�ַ
****************************************/
#pragma once
#ifndef HISTOGRAM_H
#define HISTOGRAM_H
#include<opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp> 
#include<opencv2/highgui/highgui_c.h>
#include <opencv2/core/core.hpp>

class Histogram {
private:

public:
	Histogram(IplImage* img, int* hist);
};
#endif // !HISTOGRAM_H
