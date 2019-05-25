#include<iostream>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp> 
#include<opencv2/opencv.hpp>
#include<opencv2/objdetect.hpp>
#include"detect.h"
#include"Fatigue.h"
#include"Trans.h"
#include"histogram.h"
#include"ostuThreshold.h"

using namespace std;
using namespace cv;

#define DETECTTIME	30								// һ�μ����̵�ʱ�䳤�ȣ��ü���������
#define FATIGUETHRESHOLD	180						// �ж��Ƿ�ƣ�ͼ�ʻ����ֵ

int main() {
	
	int failFaceNum = 0;							// ͳ��һ�μ�������δ��⵽����������
	int failFaceDuration = 0;						// ͳ��һ�μ�����������δ��⵽�����Ĵ���
	int maxFailFaceDuration = 0;					// һ�μ�����������δ��⵽�����Ĵ��������ֵ
	int fatigueState = 1;							// ��ʻԱ�ļ�ʻ״̬��ƣ�ͼ�ʻ��1����������ʻ��0��
	int eyeState;									// �۾��ۺ�����0�����գ�1��״̬
	int eyeCloseNum = 0;							// ͳ��һ�μ������б��۵�����
	int eyeCloseDuration = 0;						// ͳ��һ�μ�������������⵽����״̬�Ĵ���
	int maxEyeCloseDuration = 0;					// һ�μ�������������⵽����״̬�Ĵ��������ֵ
	int lEyeState;									// ��������0�����գ�1��״̬
	int rEyeState;									// ��������0�����գ�1��״̬
	int globalK = 0, globali = 0;
	int temp = 0;									//�������������������
	cv::Rect* largestFaceRect = nullptr;			// �洢��⵽�������������ο�
	IplImage* srcImg = nullptr;						// ��Ŵ�����ͷ��ȡ��ÿһ֡��ɫԴͼ��
	IplImage* img = nullptr;						// ��Ŵ�����ͷ��ȡ��ÿһ֡�Ҷ�Դͼ��
	IplImage* faceImg = nullptr;					// �洢����������ͼ��
	clock_t start, stop;							// ��ʱ����
	cv::Rect eyeRect;								// �洢�ü�������۵ľ�������
	cv::Rect eyeRectTemp;							// ��ʱ��������
	IplImage* lEyeImg = nullptr;					// �洢���۵�ͼ��
	IplImage* rEyeImg = nullptr;					// �洢���۵�ͼ��
	int hist[256];									// ���ֱ��ͼ������
	int pixelSum;									//��ֵ����ͼƬ���ص�����
	int threshold;									// �洢��ֵ��������ֵ


	Detect *detect = new Detect();
	for (globalK; globalK <= DETECTTIME; globalK++) {
		start = clock();

		detect->startDetect();

		Mat tempcapimg;
		detect->getCap() >> tempcapimg;
		IplImage tempimg = tempcapimg;
		srcImg = cvCloneImage(&tempimg);

		Mat tempgrayimg;
		tempgrayimg.create(tempcapimg.size(), tempcapimg.type());
		cvtColor(tempcapimg, tempgrayimg, cv::COLOR_BGR2GRAY);
		IplImage tempgray = tempgrayimg;
		img = cvCloneImage(&tempgray);
		cvCvtColor(srcImg, img, CV_BGR2GRAY);
		
		/************************************* ������� ****************************************/
		cvWaitKey(20);

		// ��ȡ��������
		if ((detect->getFaces().size()) == 0) {
			cout << "û�л�ȡ������" << endl;		
			failFaceNum++;							// ͳ��δ��⵽�����Ĵ���
			failFaceDuration++;						// ͳ������δ��⵽�����Ĵ���
			// ���������ж�ȫ�Ǳ��ۺͼ�ⲻ�������������û�������۵����
			(eyeCloseDuration > maxEyeCloseDuration) ? maxEyeCloseDuration = eyeCloseDuration : maxEyeCloseDuration;
			eyeCloseDuration = 0;					// ͳ��һ�μ�������������⵽����״̬�Ĵ���
			if (globalK == DETECTTIME) {
				// ��һ�μ������У���û�м�⵽��������Ҫ�ڴ˸��� maxFailFaceDuration 
				(failFaceDuration > maxFailFaceDuration) ? maxFailFaceDuration = failFaceDuration : maxFailFaceDuration;

				cout << "�ﵽƣ��״̬" << endl;
				cout << "���۴���:" << eyeCloseNum << "����������۴���:" << maxEyeCloseDuration << endl;
				cout << "��ȡ����ʧ�ܴ���:" << failFaceNum << "���������ȡ����ʧ�ܴ���:" << maxFailFaceDuration << endl;

				// ����ƣ��״̬���б�
				Fatigue *recoFatigueState = new Fatigue(FATIGUETHRESHOLD, eyeCloseNum, maxEyeCloseDuration, failFaceNum, maxFailFaceDuration);
				fatigueState = recoFatigueState->getcompreValue();
				if (fatigueState == 1) {
					cout << "��ʻԱ����ƣ�ͼ�ʻ״̬" << endl << endl;
				}
				else if (fatigueState == 0) {
					cout << "��ʻԱ����������ʻ״̬" << endl << endl;
				}

				// ������һ�μ�����ǰ������������
				globalK = 0;
				lEyeState = 1;
				rEyeState = 1;
				eyeState = 1;
				eyeCloseNum = 0;
				eyeCloseDuration = 0;
				maxEyeCloseDuration = 0;
				failFaceNum = 0;
				failFaceDuration = 0;
				maxFailFaceDuration = 0;
				fatigueState = 1;

				cvWaitKey(1000);
			}
			continue;
		}
		else {//��⵽����
			// ͳ������δ��⵽�����Ĵ����е������ֵ
			(failFaceDuration > maxFailFaceDuration) ? maxFailFaceDuration = failFaceDuration : maxFailFaceDuration;
			failFaceDuration = 0;
			// �ҵ���⵽������������������
			temp = 0;
			for (int i = 0; i < detect->getFaces().size(); i++) {
				if ((detect->getFaces().at(i).width * detect->getFaces().at(i).height) > temp) {
					temp = detect->getFaces().at(i).width * detect->getFaces().at(i).height;
					largestFaceRect = new Rect(detect->getFaces().at(i));
				}
			}

			// ��������������֪ʶ�ָ�����µ���������
			temp = largestFaceRect->width / 8;
			largestFaceRect->x = largestFaceRect->x + temp;
			largestFaceRect->width = largestFaceRect->width - 3 * temp / 2;
			largestFaceRect->height = largestFaceRect->height / 2;
			largestFaceRect->y = largestFaceRect->y + largestFaceRect->height / 2;
			largestFaceRect->height = largestFaceRect->height / 2;

			cvSetImageROI(img, *largestFaceRect);				// ����ROIΪ��⵽��������������
			faceImg = cvCreateImage(cvSize(largestFaceRect->width, largestFaceRect->height), IPL_DEPTH_8U, 1);
			cvCopy(img, faceImg, NULL);
			cvResetImageROI(img);								// �ͷ�ROI
			cvShowImage("�ָ�����������", faceImg);
			//aitKey(0);

			eyeRectTemp = *largestFaceRect;
			// ��������������֪ʶ�ָ�����µ���������
			largestFaceRect->width /= 2;
			cvSetImageROI(img, *largestFaceRect);				// ����ROIΪ��⵽��������������
			lEyeImg = cvCreateImage(cvSize(largestFaceRect->width, largestFaceRect->height), IPL_DEPTH_8U, 1);
			cvCopy(img, lEyeImg, NULL);
			cvResetImageROI(img);								// �ͷ�ROI
			cvShowImage("���µ���������", lEyeImg);
			//waitKey(0);

			// ��������������֪ʶ�ָ�����µ���������
			eyeRectTemp.x += eyeRectTemp.width / 2;
			eyeRectTemp.width /= 2;
			cvSetImageROI(img, eyeRectTemp);					// ����ROIΪ��⵽��������������
			rEyeImg = cvCreateImage(cvSize(eyeRectTemp.width, eyeRectTemp.height), IPL_DEPTH_8U, 1);
			cvCopy(img, rEyeImg, NULL);
			cvResetImageROI(img);								// �ͷ�ROI
			cvShowImage("���µ���������", rEyeImg);
			//waitKey(0);

			/********************************** ��ֵ������ ***********************************/
			cvSmooth(lEyeImg, lEyeImg, CV_MEDIAN);				// ��ֵ�˲� Ĭ�ϴ��ڴ�СΪ3*3
			Trans nonlineTrans(lEyeImg, lEyeImg, 0.8);			// �����Ե�����
			memset(hist, 0, sizeof(hist));						// ��ʼ��ֱ��ͼ������Ϊ0
			Histogram histogram(lEyeImg, hist);					// ����ͼƬֱ��ͼ
			// ���������ֵ
			pixelSum = lEyeImg->width * lEyeImg->height;

			OstuThreshold ostuThreshold(hist, pixelSum, 45);
			threshold = ostuThreshold.getostu();
			cvThreshold(lEyeImg, lEyeImg, threshold, 255, CV_THRESH_BINARY);// ��ͼ���ֵ��
			// ��ʾ��ֵ�����ͼ��
			cvShowImage("��ֵ������۾�", lEyeImg);
			waitKey(0);
		}
	}
	return 0;
}

