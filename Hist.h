/**************************************************
���ܣ�����ͼ��ֱ��ͼ��ˮƽ����ʹ�ֱ�����ͶӰ
���룺
	srcImg��Դͼ��
�����
	horiProj: ˮƽ�����ͶӰ�����1 * height�����ָ�룬����ǰ�ǵó�ʼ��
	vertProj����ֱ�����ͶӰ�����1 * width�����ָ�룬����ǰ�ǵó�ʼ��
**************************************************/
#pragma once
#ifndef HIST_H
#define HIST_H
#include<opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp> 
#include<opencv2/highgui/highgui_c.h>
#include <opencv2/core/core.hpp>

class Hist {
private:

public:
	Hist(){}
	Hist(IplImage* srcImg, int* horiProj, int* vertProj);
	void histProject(IplImage* srcImg, int* horiProj, int* vertProj);//����ͼ��ֱ��ͼ��ˮƽ����ʹ�ֱ�����ͶӰ
};
#endif // !HIST_H
