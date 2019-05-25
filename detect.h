#pragma once
#ifndef DETECT_H
#define DETECT_H

#include<opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp> 
#include<opencv2/highgui/highgui_c.h>
#include <opencv2/core/core.hpp>
#include<vector>
using namespace cv;
using namespace std;
//���� �۲����
class Detect {
private:
	VideoCapture cap;
	cv::CascadeClassifier face_cascade;
	//face_cascade.load("Haar\\haarcascade_frontalface_default.xml");
	cv::CascadeClassifier eye_cascade;
	//eye_cascade.load("Haar\\haarcascade_eye_tree_eyeglasses.xml");

	//IplImage* srcImg;					// �Ҷ�ͼ��
	//CvSeq* objects;					// �����������⵽�����ľ��ο�
	//CvMemStorage* storage;			// �洢���ο���ڴ�����
	Mat img;//ԭͼ
	Mat gray;//�Ҷ�ͼ
	vector<Rect> faces;//��������
	vector<Rect>eyes;//�۲�����
	double scale_factor = 1.3;		// �������ڵı���ϵ��
	int min_neighbors = 5;			// ���ɼ��Ŀ������ھ��ε���С����
	int flags = 0;					// ������ʽ
	//CvSize min_size = cvSize(20, 20);// ��ⴰ�ڵ���С�ߴ�

	Scalar colors[7] = {
		// ��Ȼ���������		
		CV_RGB(255, 0, 0),
		CV_RGB(255, 97, 0),
		CV_RGB(255, 255, 0),
		CV_RGB(0, 255, 0),
		CV_RGB(0, 255, 255),
		CV_RGB(0, 0, 255),
		CV_RGB(160, 32, 240)
	};

public:
	Detect();
	void startDetect();//��ʼ���
	bool checkisOpened();//�������ͷ�Ƿ��
	void detectFace();//�������
	void detectEyes(const int index);//����۾�
	void showDetect();//��ʾ���ͼ��
	vector<Rect> getFaces() { return faces; }
	VideoCapture getCap() { return cap; }
};
#endif 
