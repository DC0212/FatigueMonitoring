/************************************************************
���ܣ��ҳ��������޶������ڵ���͵��λ��
���룺
	int* project:  ����
	int size: ���еĳ���
	int region: �м�����ķ�Χ
�����
	����int���λ��, ����ʱ����-1
************************************************************/
#pragma once
#ifndef EYEPOS_H
#define EYEPOS_H
#include<opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp> 
#include<opencv2/highgui/highgui_c.h>
#include <opencv2/core/core.hpp>
using namespace cv;
class EyePos {
private:
	//Point* point = new Point();
	typedef struct
	{
		int data;
		int index;
	}projectArr;

public:
	// qsort�ĺ�������
	int cmpInc(const void* a, const void* b){
		return (*(projectArr*)a).data - (*(projectArr*)b).data;
	}
	int getEyePos(int* project, int size, int region);
	int removeEyebrow(int* horiProject, int width, int height, int threshold);//ȥ��üë
	void calEyeSocketRegion(Rect* eyeRect, int width, int height, int EyeCol, int EyeRow);
	void getEyeMinRect(Rect* eyeRect, int* horiProject, int* vertProject, int width, int height, int horiThreshold = 5, int vertThreshold = 3);
	double calMiddleAreaBlackPixRate(int* vertProject, Rect* eyeRect, int width, int height, int eyeCol, int MinEyeballBlackPixel);
	int getEyeState(double MinEyeballRectShape, double MinEyeballBlackPixelRate, double MinEyeballBeta); \
	int removeEyeglasses(int* horiProject, int width, int height, int threshold);//ȥ���۾�
};
#endif // !EYEPOS_H

