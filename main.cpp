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
#include"Hist.h"
#include"eyePos.h"

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
	int WIDTH = 0;									// ͼ��Ŀ��
	int HEIGHT = 0;									// ͼ��ĸ߶�
	int* horiProject = NULL;						// ˮƽ�����ͶӰ���(����ָ��)
	int* vertProject = NULL;						// ��ֱ�����ͶӰ���(����ָ��)
	int* subhoriProject = NULL;						// ˮƽ�����ͶӰ���(����ָ��)
	int* subvertProject = NULL;						// ��ֱ�����ͶӰ���(����ָ��)
	int rEyeCol = 0;								// �������ڵ�����
	int lEyeCol = 0;								// �������ڵ�����
	int lEyeRow = 0;								// �������ڵ�����
	int rEyeRow = 0;								// �������ڵ�����
	IplImage* lEyeImgNoEyebrow = NULL;				// �洢ȥ��üë֮�������ͼ��
	IplImage* rEyeImgNoEyebrow = NULL;				// �洢ȥ��üë֮�������ͼ��
	IplImage* lEyeballImg = NULL;					// �洢���շָ�����ۿ��ͼ��
	IplImage* rEyeballImg = NULL;					// �洢���շָ�����ۿ��ͼ��
	IplImage* lMinEyeballImg = NULL;				// �洢���շָ����С�����ۿ��ͼ��
	IplImage* rMinEyeballImg = NULL;				// �洢���շָ����С�����ۿ��ͼ��
	double lMinEyeballRectShape;					// �洢��С���ۿ��ľ��γ����ֵ
	double rMinEyeballRectShape;					// �洢��С���ۿ��ľ��γ����ֵ
	double lMinEyeballBeta;							// �洢��С���ۿ����м�1/2����ĺ����ر�ֵ
	double rMinEyeballBeta;							// �洢��С�ұ��ۿ����м�1/2����ĺ����ر�ֵ
	int lMinEyeballBlackPixel;						// �洢���շָ����С�����ۿ�İ�ɫ���ظ���
	int rMinEyeballBlackPixel;						// �洢���շָ����С�����ۿ�İ�ɫ���ظ���
	double lMinEyeballBlackPixelRate;				// �洢���շָ����С�����ۿ�ĺ�ɫ����ռ�ı���
	double rMinEyeballBlackPixelRate;				// �洢���շָ����С�����ۿ�ĺ�ɫ����ռ�ı���
	float TIMETOTAL = 0;							// ʱ������
	int TempBow = 0;								// ��ʱ��ͷ����
	int TempBlink = 0;								// ��ʱգ�۴���


	Detect *detect = new Detect();
	EyePos* eyepos = new EyePos();
	Fatigue* recoFatigueState = new Fatigue(FATIGUETHRESHOLD, eyeCloseNum, maxEyeCloseDuration, failFaceNum, maxFailFaceDuration);
	while (1) {
		for (globalK; globalK <= DETECTTIME; globalK++) {
			start = clock();
			TIMETOTAL = start / 10000;

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
				//int tempBow = detect->getBow();
				//tempBow++;
				//detect->setBow(tempBow);
				if (TempBow == 0) {
					TempBow++;//��ʱ��ͷ������1
				}

				cout << "û�л�ȡ������" << endl;
				failFaceNum++;							// ͳ��δ��⵽�����Ĵ���
				failFaceDuration++;						// ͳ������δ��⵽�����Ĵ���
				// ���������ж�ȫ�Ǳ��ۺͼ�ⲻ�������������û�������۵����
				(eyeCloseDuration > maxEyeCloseDuration) ? maxEyeCloseDuration = eyeCloseDuration : maxEyeCloseDuration;
				eyeCloseDuration = 0;					// ͳ��һ�μ�������������⵽����״̬�Ĵ���
				if (globalK == DETECTTIME) {
					// ��һ�μ������У���û�м�⵽��������Ҫ�ڴ˸��� maxFailFaceDuration 
					(failFaceDuration > maxFailFaceDuration) ? maxFailFaceDuration = failFaceDuration : maxFailFaceDuration;

					//cout << "�ﵽƣ��״̬" << endl;
					cout << "���۴���:" << eyeCloseNum << "����������۴���:" << maxEyeCloseDuration << endl;
					cout << "��ȡ����ʧ�ܴ���:" << failFaceNum << "���������ȡ����ʧ�ܴ���:" << maxFailFaceDuration << endl;

					// ����ƣ��״̬���б�
					recoFatigueState = new Fatigue(FATIGUETHRESHOLD, eyeCloseNum, maxEyeCloseDuration, failFaceNum, maxFailFaceDuration);
					fatigueState = recoFatigueState->getisTired(FATIGUETHRESHOLD);
					if (fatigueState == 1) {
						cout << "��ʻԱ����ƣ�ͼ�ʻ״̬" << endl << endl;
						cout << "�ѽ���ƣ��״̬�޷�ͳ��գ�ۺ͵�ͷƵ��" << endl;
						//detect->setBlink(0);
						//detect->setBow(0);

						/*cout << "գ��Ƶ��:" << detect->getBlinkper(TIMETOTAL) << "��/10��" << endl;
						cout << "��ͷƵ��:" << detect->getBowper(TIMETOTAL) << "��/10��" << endl;
						//TempBow++;
						detect->setBlink(0);
						detect->setBow(0);*/
					}
					else if (fatigueState == 0) {
						cout << "��ʻԱ����������ʻ״̬" << endl << endl;
						cout << "գ��Ƶ��:" << detect->getBlinkper(TIMETOTAL) << "��/10��" << endl;
						cout << "��ͷƵ��:" << detect->getBowper(TIMETOTAL) << "��/10��" << endl;
						//TempBow++;
						detect->setBlink(0);
						detect->setBow(0);
					}
					cvWaitKey(0);

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

				//cout << "��ͷƵ��:" << detect->getBowper(TIMETOTAL) << "��/10��" << endl;
				//TempBow++;
				if (TempBow == 1) {//�����ʱ��ͷ����Ϊ1
					int Bow = detect->getBow();
					Bow++;
					detect->setBow(Bow);
					TempBow = 0;
				}

				// ͳ������δ��⵽�����Ĵ����е������ֵ
				(failFaceDuration > maxFailFaceDuration) ? maxFailFaceDuration = failFaceDuration : maxFailFaceDuration;
				failFaceDuration = 0;
				// �ҵ���⵽������������������
				temp = 0; //�������������������
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
				//cvShowImage("�ָ�����������", faceImg);
				//aitKey(0);

				eyeRectTemp = *largestFaceRect;
				// ��������������֪ʶ�ָ�����µ���������
				largestFaceRect->width /= 2;
				cvSetImageROI(img, *largestFaceRect);				// ����ROIΪ��⵽��������������
				lEyeImg = cvCreateImage(cvSize(largestFaceRect->width, largestFaceRect->height), IPL_DEPTH_8U, 1);
				cvCopy(img, lEyeImg, NULL);
				cvResetImageROI(img);								// �ͷ�ROI
				//cvShowImage("���µ���������", lEyeImg);
				//waitKey(0);

				// ��������������֪ʶ�ָ�����µ���������
				eyeRectTemp.x += eyeRectTemp.width / 2;
				eyeRectTemp.width /= 2;
				cvSetImageROI(img, eyeRectTemp);					// ����ROIΪ��⵽��������������
				rEyeImg = cvCreateImage(cvSize(eyeRectTemp.width, eyeRectTemp.height), IPL_DEPTH_8U, 1);
				cvCopy(img, rEyeImg, NULL);
				cvResetImageROI(img);								// �ͷ�ROI
				//cvShowImage("���µ���������", rEyeImg);
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
				//cvShowImage("��ֵ���������", lEyeImg);
				

				/*** ��ֵ�����۴��������ͼ�� ***/
				cvSmooth(rEyeImg, rEyeImg, CV_MEDIAN);		// ��ֵ�˲� Ĭ�ϴ��ڴ�СΪ3*3
				Trans RnonlineTrans(rEyeImg, rEyeImg, 0.8);	// �����Ե�����
				memset(hist, 0, sizeof(hist));				// ��ʼ��ֱ��ͼ������Ϊ0
				Histogram Rhistogram(rEyeImg, hist);		// ����ͼƬֱ��ͼ
				// ���������ֵ
				pixelSum = rEyeImg->width * rEyeImg->height;
				OstuThreshold RostuThreshold(hist, pixelSum, 45);
				cvThreshold(rEyeImg, rEyeImg, threshold, 255, CV_THRESH_BINARY);// ��ͼ���ֵ��
				// ��ʾ��ֵ�����ͼ��
				//cvShowImage("��ֵ���������", rEyeImg);
				//waitKey(0);
				/***************************************** ������� ********************************************/
				/** ��������Ե�üë������ָ�ȥ��üë **/
				// �ָ�����üë
				HEIGHT = lEyeImg->height;
				WIDTH = lEyeImg->width;
				// �����ڴ�
				horiProject = (int*)malloc(HEIGHT * sizeof(int));
				vertProject = (int*)malloc(WIDTH * sizeof(int));
				if (horiProject == NULL || vertProject == NULL) {
					cout << "�����ڴ�ʧ��" << endl;
					cvWaitKey(0);
					return -1;
				}
				// �ڴ�����
				for (int i = 0; i < HEIGHT; i++) {
					*(horiProject + i) = 0;
				}
				for (int i = 0; i < WIDTH; i++) {
					*(vertProject + i) = 0;
				}

				Hist LhistProject;
				LhistProject.histProject(lEyeImg, horiProject, vertProject);
				//Hist LhistProject(lEyeImg, horiProject, vertProject);				// ����ֱ��ͼͶӰ
				lEyeRow = eyepos->removeEyebrow(horiProject, WIDTH, HEIGHT, 10);	// ����ָ�üë���ۿ��λ��

				// �ָ�����üë
				HEIGHT = rEyeImg->height;
				WIDTH = rEyeImg->width;
				// �����ڴ�
				horiProject = (int*)malloc(HEIGHT * sizeof(int));
				vertProject = (int*)malloc(WIDTH * sizeof(int));
				if (horiProject == NULL || vertProject == NULL) {
					cout << "�����ڴ�ʧ��" << endl;
					cvWaitKey(0);
					return -1;
				}
				// �ڴ�����
				for (int i = 0; i < HEIGHT; i++) {
					*(horiProject + i) = 0;
				}
				for (int i = 0; i < WIDTH; i++) {
					*(vertProject + i) = 0;
				}

				Hist RhistProject;
				RhistProject.histProject(rEyeImg, horiProject, vertProject);
				//Hist RhistProject(rEyeImg, horiProject, vertProject);							// ����ֱ��ͼͶӰ
				rEyeRow = eyepos->removeEyebrow(horiProject, WIDTH, HEIGHT, 10);				// ����ָ�üë���ۿ��λ��
				//eyepos->removeEyeglasses(vertProject, WIDTH, HEIGHT, 100);						// ȥ���۾�
				
																								// ��ʾȥ��üë������۴�������
				eyeRect = cvRect(0, lEyeRow, lEyeImg->width, (lEyeImg->height - lEyeRow));		// ȥüë���ۿ�������lEyeImg�еľ��ο�����
				cvSetImageROI(lEyeImg, eyeRect);												// ����ROIΪȥ��üë���ۿ����������ͷ�ROI
				lEyeImgNoEyebrow = cvCreateImage(cvSize(eyeRect.width, eyeRect.height), IPL_DEPTH_8U, 1);
				cvCopy(lEyeImg, lEyeImgNoEyebrow, NULL);
				//cvShowImage("ȥ��üë��ֵ������", lEyeImgNoEyebrow);

			/****************************************************************/
				//����ȥ������
				vector<vector<Point>> contour;

				Mat resulttemp;
				resulttemp = cvarrToMat(lEyeImgNoEyebrow);//ǳ����
				Mat result = resulttemp.clone();//���
				Mat result1 = result.clone();
				
				findContours(result,contour,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);//Ѱ������
				vector<vector<Point>>::iterator iter  = contour.begin();
				while (iter != contour.end()) {
					if (iter->size() < 30) {//ɾ���������С��30��
						iter = contour.erase(iter);
					}
					else {
						++iter;
					}
				}
				drawContours(result, contour, -1, Scalar(255), CV_FILLED);
				/*IplImage tempimg = result;
				IplImage* imgtmp = cvCloneImage(&tempimg);*/
				//cvErode(lEyeImgNoEyebrow, imgtmp, NULL, 2);		//��ʴͼ��  
				//cvDilate(lEyeballImg, lEyeballImg, NULL, 2);	//����ͼ��
				
				Mat dst;
				//absdiff(result1, result, dst);
				//bitwise_not(result, dst);
				bitwise_xor(result1, result, dst);
				//imshow("result1", result1);
				//imshow("result", result);
				bitwise_not(dst,dst);
				IplImage LIMGTEMP = dst;
				lEyeImgNoEyebrow = cvCloneImage(&LIMGTEMP);
				//imshow("ȥ������", dst);
				cvErode(lEyeImgNoEyebrow, lEyeImgNoEyebrow, NULL, 2);		//��ʴͼ��  
				cvDilate(lEyeImgNoEyebrow, lEyeImgNoEyebrow, NULL, 2);	//����ͼ��
				cvDilate(lEyeImgNoEyebrow, lEyeImgNoEyebrow, NULL, 1);	//����ͼ��
				cvShowImage("Lȥ������", lEyeImgNoEyebrow);

				/****************************************************************/
				//����ȥ��üë
				//cvWaitKey(0);


				eyeRectTemp = cvRect(0, rEyeRow, rEyeImg->width, (rEyeImg->height - rEyeRow));	// ȥüë���ۿ�������rEyeImg�еľ��ο�����
				cvSetImageROI(rEyeImg, eyeRectTemp);											// ����ROIΪȥ��üë���ۿ����������ͷ�ROI
				rEyeImgNoEyebrow = cvCreateImage(cvSize(eyeRectTemp.width, eyeRectTemp.height), IPL_DEPTH_8U, 1);
				cvCopy(rEyeImg, rEyeImgNoEyebrow, NULL);
				//cvShowImage("ȥ��üë��ֵ������", rEyeImgNoEyebrow);
				//cvWaitKey(0);
				/****************************************************************/
				//����ȥ������
				vector<vector<Point>> Rcontour;

				Mat Rresulttemp;
				Rresulttemp = cvarrToMat(rEyeImgNoEyebrow);//ǳ����
				Mat Rresult = Rresulttemp.clone();//���
				Mat Rresult1 = Rresult.clone();

				findContours(Rresult,Rcontour, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);//Ѱ������
				vector<vector<Point>>::iterator Riter = Rcontour.begin();
				while (Riter != Rcontour.end()) {
					if (Riter->size() < 20) {//ɾ���������С��30��
						Riter = Rcontour.erase(Riter);
					}
					else {
						++Riter;
					}
				}
				drawContours(Rresult, Rcontour, -1, Scalar(255), CV_FILLED);
				/*IplImage tempimg = result;
				IplImage* imgtmp = cvCloneImage(&tempimg);*/
				//cvErode(lEyeImgNoEyebrow, imgtmp, NULL, 2);		//��ʴͼ��  
				//cvDilate(lEyeballImg, lEyeballImg, NULL, 2);	//����ͼ��

				Mat Rdst;
				//absdiff(result1, result, dst);
				//bitwise_not(result, dst);
				bitwise_xor(Rresult1, Rresult, Rdst);
				//imshow("result1", Rresult1);
				//imshow("result", Rresult);
				bitwise_not(Rdst, Rdst);
				IplImage RLIMGTEMP = Rdst;
				rEyeImgNoEyebrow = cvCloneImage(&RLIMGTEMP);
				//imshow("ȥ������", Rdst);
				cvErode(rEyeImgNoEyebrow, rEyeImgNoEyebrow, NULL, 2);		//��ʴͼ��  
				cvDilate(rEyeImgNoEyebrow, rEyeImgNoEyebrow, NULL, 2);	//����ͼ��
				cvDilate(rEyeImgNoEyebrow, rEyeImgNoEyebrow, NULL, 1);	//����ͼ��
				cvShowImage("rȥ������", rEyeImgNoEyebrow);
				cvWaitKey(100);

				/****************************************************************/

				///////////////// ��λ�۾����ĵ���ȥ��üëͼ���е�����λ�� ///////////////////
				HEIGHT = lEyeImgNoEyebrow->height;
				WIDTH = lEyeImgNoEyebrow->width;
				// �����ڴ�
				subhoriProject = (int*)malloc(HEIGHT * sizeof(int));
				subvertProject = (int*)malloc(WIDTH * sizeof(int));
				if (subhoriProject == NULL || subvertProject == NULL) {
					cout << "�����ڴ�ʧ��" << endl;
					cvWaitKey(0);
					return -1;
				}
				// �ڴ�����
				for (int i = 0; i < HEIGHT; i++) {
					*(subhoriProject + i) = 0;
				}
				for (int i = 0; i < WIDTH; i++) {
					*(subvertProject + i) = 0;
				}

				RhistProject.histProject(lEyeImgNoEyebrow, subhoriProject, subvertProject);
				//Hist RhistProject1(lEyeImgNoEyebrow, subhoriProject, subvertProject);	// ���¶Էָ��������ͼ����л���ͶӰ
				lEyeRow = eyepos->getEyePos(subhoriProject, HEIGHT, HEIGHT / 5);	// ��λ�������ڵ���
				lEyeCol = eyepos->getEyePos(subvertProject, WIDTH, WIDTH / 5);	// ��λ�������ڵ���


				HEIGHT = rEyeImgNoEyebrow->height;
				WIDTH = rEyeImgNoEyebrow->width;
				// �����ڴ�
				subhoriProject = (int*)malloc(HEIGHT * sizeof(int));
				subvertProject = (int*)malloc(WIDTH * sizeof(int));
				if (subhoriProject == NULL || subvertProject == NULL) {
					cout << "�����ڴ�ʧ��" << endl;
					cvWaitKey(0);
					return -1;
				}
				// �ڴ�����
				for (int i = 0; i < HEIGHT; i++) {
					*(subhoriProject + i) = 0;
				}
				for (int i = 0; i < WIDTH; i++) {
					*(subvertProject + i) = 0;
				}
				RhistProject.histProject(rEyeImgNoEyebrow, subhoriProject, subvertProject);
				//RhistProject(rEyeImgNoEyebrow, subhoriProject, subvertProject);	// ���¶Էָ��������ͼ����л���ͶӰ
				rEyeRow = eyepos->getEyePos(subhoriProject, HEIGHT, HEIGHT / 5);	// ��λ�������ڵ���
				rEyeCol = eyepos->getEyePos(subvertProject, WIDTH, WIDTH / 5);	// ��λ�������ڵ���
				
				// ����۾���λ��
				cvCircle(lEyeImgNoEyebrow, cvPoint(lEyeCol, lEyeRow), 3, CV_RGB(0, 0, 255), 1, 8, 0);
				cvCircle(rEyeImgNoEyebrow, cvPoint(rEyeCol, rEyeRow), 3, CV_RGB(0, 0, 255), 1, 8, 0);
				//cvShowImage("��������", lEyeImgNoEyebrow);
				//cvShowImage("��������", rEyeImgNoEyebrow);
				//cvWaitKey(0);

				/********************************** �ж���������״̬ ***********************************/

			////////////////// �ָ�����ҵ�������Ϊ���ĵĴ����ۿ� /////////////////
			// ���ۿ�
				HEIGHT = lEyeImgNoEyebrow->height;
				WIDTH = lEyeImgNoEyebrow->width;
				// ��������ۿ�������: eyeRect
				eyeRect = cvRect(0, 0, WIDTH, HEIGHT);
				//Rect* tempRect = new Rect(0, 0, HEIGHT, WIDTH);
				eyepos->calEyeSocketRegion(&eyeRect, WIDTH, HEIGHT, lEyeCol, lEyeRow);
				
				cvSetImageROI(lEyeImgNoEyebrow, eyeRect);		// ����ROIΪ��⵽�ۿ�����
				
				//Mat MatlEyeballImg;
				lEyeballImg = cvCreateImage(cvSize(lEyeImgNoEyebrow->width, lEyeImgNoEyebrow->height), IPL_DEPTH_8U, 1);
				//cvCopy(lEyeImgNoEyebrow, lEyeballImg,NULL);
				lEyeballImg = cvCloneImage(lEyeImgNoEyebrow);
				cvResetImageROI(lEyeImgNoEyebrow);// �ͷ�ROI
				//cvShowImage("���ۿ�", lEyeballImg);

				// ���ۿ�
				HEIGHT = rEyeImgNoEyebrow->height;
				WIDTH = rEyeImgNoEyebrow->width;
				// ��������ۿ�������: eyeRectTemp
				eyeRect = cvRect(0, 0, WIDTH, HEIGHT);
				eyepos->calEyeSocketRegion(&eyeRect, WIDTH, HEIGHT, rEyeCol, rEyeRow);
				
				
				cvSetImageROI(rEyeImgNoEyebrow, eyeRect);		// ����ROIΪ��⵽�ۿ�����
				rEyeballImg = cvCreateImage(cvSize(rEyeImgNoEyebrow->width, rEyeImgNoEyebrow->height), IPL_DEPTH_8U, 1);
				//cvCopy(rEyeImgNoEyebrow, rEyeballImg, NULL);
				rEyeballImg = cvCloneImage(rEyeImgNoEyebrow);
				cvResetImageROI(rEyeImgNoEyebrow);
				//cvShowImage("���ۿ�", rEyeballImg);
				//cvWaitKey(0);
				/////////////////////////// ������ ///////////////////////////
				cvErode(lEyeballImg, lEyeballImg, NULL, 2);		//��ʴͼ��  
				cvDilate(lEyeballImg, lEyeballImg, NULL, 2);	//����ͼ��
				//cvShowImage("������", lEyeballImg);

				cvErode(rEyeballImg, rEyeballImg, NULL, 1);		//��ʴͼ��  
				//�ȸ�ʴͼ��ȥ�����
				cvDilate(rEyeballImg, rEyeballImg, NULL, 1);	//����ͼ��
				//������ͼ��ʹ����ͼ����
				//cvShowImage("������", rEyeballImg);
				//cvWaitKey(0);

				/////////////////// ������С�۾��ľ������� ////////////////////

			///////////////////////////����
				HEIGHT = lEyeballImg->height;
				WIDTH = lEyeballImg->width;

				// �����ڴ�
				subhoriProject = (int*)malloc(HEIGHT * sizeof(int));
				subvertProject = (int*)malloc(WIDTH * sizeof(int));
				if (subhoriProject == NULL || subvertProject == NULL) {
					cout << "�����ڴ�ʧ��" << endl;
					cvWaitKey(0);
					return -1;
				}
				// �ڴ�����
				for (int i = 0; i < HEIGHT; i++) {
					*(subhoriProject + i) = 0;
				}
				for (int i = 0; i < WIDTH; i++) {
					*(subvertProject + i) = 0;
				}
				//histProject(lEyeballImg, subhoriProject, subvertProject);
				LhistProject.histProject(lEyeballImg, subhoriProject, subvertProject);
				// ����������С�ľ�������
				eyeRectTemp = cvRect(0, 0, 1, 1);		// ��ʼ��
				eyepos->getEyeMinRect(&eyeRectTemp, subhoriProject, subvertProject, WIDTH, HEIGHT, 5, 3);
	
				// ������С���۾��εĳ����,  �ж��۾�״̬ʱ�õĵ�
				lMinEyeballRectShape = (double)eyeRectTemp.width / (double)eyeRectTemp.height;
				//printf("\nlMinEyeballRectShape: %f\n", lMinEyeballRectShape);

				cvSetImageROI(lEyeballImg, eyeRectTemp);		// ����ROIΪ��⵽��С������ۿ�
				lMinEyeballImg = cvCreateImage(cvSize(lEyeballImg->width, lEyeballImg->height), IPL_DEPTH_8U, 1);
				//cvCopy(lEyeballImg, lMinEyeballImg, NULL);
				lMinEyeballImg = cvCloneImage(lEyeballImg);
				cvResetImageROI(lEyeballImg);
				//cvShowImage("���ۿ���С��������", lMinEyeballImg);

				
				//cvShowImage("������", rEyeballImg);

				//cvWaitKey(0);
				

				////////////////////////  ͳ�����ۺ����ظ���  /////////////////////
				HEIGHT = lMinEyeballImg->height;
				WIDTH = lMinEyeballImg->width;

				// �����ڴ�
				subhoriProject = (int*)malloc(HEIGHT * sizeof(int));
				subvertProject = (int*)malloc(WIDTH * sizeof(int));
				if (subhoriProject == NULL || subvertProject == NULL) {
					cout << "�����ڴ�ʧ��" << endl;
					cvWaitKey(0);
					return -1;
				}
				// �ڴ�����
				for (int i = 0; i < HEIGHT; i++) {
					*(subhoriProject + i) = 0;
				}
				for (int i = 0; i < WIDTH; i++) {
					*(subvertProject + i) = 0;
				}

				//histProject(lMinEyeballImg, subhoriProject, subvertProject);
				LhistProject.histProject(lMinEyeballImg, subhoriProject, subvertProject);
				// ͳ��lEyeballImg�к�ɫ���صĸ���
				temp = 0;	// �����ظ���
				for (int i = 0; i < WIDTH; i++) {
					temp += *(subvertProject + i);
				}
				temp /= 255;
				lMinEyeballBlackPixel = WIDTH * HEIGHT - temp;
				lMinEyeballBlackPixelRate = (double)lMinEyeballBlackPixel / (double)(WIDTH * HEIGHT);

				// ͳ��lMinEyeballImg�е�1/2�����ں����صı���
				lMinEyeballBeta = 0;
				lMinEyeballBeta = eyepos->calMiddleAreaBlackPixRate(subvertProject, &eyeRectTemp, WIDTH, HEIGHT, lEyeCol, lMinEyeballBlackPixel);

				

				////////////////////////////////////����
				HEIGHT = rEyeballImg->height;
				WIDTH = rEyeballImg->width;
				// �����ڴ�
				subhoriProject = (int*)malloc(HEIGHT * sizeof(int));
				subvertProject = (int*)malloc(WIDTH * sizeof(int));
				if (subhoriProject == NULL || subvertProject == NULL) {
					cout << "�����ڴ�ʧ��" << endl;
					cvWaitKey(0);
					return -1;
				}
				// �ڴ�����
				for (int i = 0; i < HEIGHT; i++) {
					*(subhoriProject + i) = 0;
				}
				for (int i = 0; i < WIDTH; i++) {
					*(subvertProject + i) = 0;
				}
				//histProject(rEyeballImg, subhoriProject, subvertProject);
				RhistProject.histProject(rEyeballImg, subhoriProject, subvertProject);
				// ����������С�ľ�������
				eyeRectTemp = cvRect(0, 0, 1, 1);
				eyepos->getEyeMinRect(&eyeRectTemp, subhoriProject, subvertProject, WIDTH, HEIGHT, 5, 3);

				// ������С���۾��εĳ���ȣ��ж��۾�״̬ʱ�õĵ�
				rMinEyeballRectShape = (double)eyeRectTemp.width / (double)eyeRectTemp.height;

				cvSetImageROI(rEyeballImg, eyeRectTemp);		// ����ROIΪ��⵽��С������ۿ�
				rMinEyeballImg = cvCreateImage(cvSize(rEyeballImg->width, rEyeballImg->height), IPL_DEPTH_8U, 1);
				//cvCopy(rEyeballImg, rMinEyeballImg, NULL);
				rMinEyeballImg = cvCloneImage(rEyeballImg);
				cvResetImageROI(rEyeballImg);
				//cvShowImage("���ۿ���С��������", rMinEyeballImg);

				//cvWaitKey(0);
				////////////////////////  ͳ�����ۺ����ظ���  /////////////////////
				HEIGHT = rMinEyeballImg->height;
				WIDTH = rMinEyeballImg->width;

				// �����ڴ�
				subhoriProject = (int*)malloc(HEIGHT * sizeof(int));
				subvertProject = (int*)malloc(WIDTH * sizeof(int));
				if (subhoriProject == NULL || subvertProject == NULL) {
					cout << "�����ڴ�ʧ��" << endl;
					cvWaitKey(0);
					return -1;
				}
				// �ڴ�����
				for (int i = 0; i < HEIGHT; i++) {
					*(subhoriProject + i) = 0;
				}
				for (int i = 0; i < WIDTH; i++) {
					*(subvertProject + i) = 0;
				}
				//histProject(rMinEyeballImg, subhoriProject, subvertProject);// ����ֱ��ͼ����ͶӰ
				RhistProject.histProject(rMinEyeballImg, subhoriProject, subvertProject);
				// ͳ��REyeballImg�к�ɫ���صĸ���
				temp = 0;
				for (int i = 0; i < WIDTH; i++) {
					temp += *(subvertProject + i);
				}
				temp /= 255;
				rMinEyeballBlackPixel = WIDTH * HEIGHT - temp;
				rMinEyeballBlackPixelRate = (double)rMinEyeballBlackPixel / (double)(WIDTH * HEIGHT);

				// ͳ��lMinEyeballImg�е�1/2�����ں����صı���
				rMinEyeballBeta = 0;
				rMinEyeballBeta = eyepos->calMiddleAreaBlackPixRate(subvertProject, &eyeRectTemp, WIDTH, HEIGHT, rEyeCol, rMinEyeballBlackPixel);

				// �ж��۾��������
				lEyeState = 1;		// ����״̬��Ĭ�ϱ���
				rEyeState = 1;		// ����״̬��Ĭ�ϱ���
				eyeState = 1;		// �۾��ۺ�״̬��Ĭ�ϱ���
				//cvWaitKey(0);
				if (lMinEyeballBlackPixel > 100) {
					//lEyeState = eyepos->getEyeState(lMinEyeballRectShape, lMinEyeballBlackPixelRate, lMinEyeballBeta);
					lEyeState = 0;
				}
				else {//��С��ɫ��������С��50��ֱ����Ϊ������
					lEyeState = 1;
				}

				if (detect->getEyes().size() > 0) {
					lEyeState = 0;//���Haarֱ��ʶ�����۾�
				}


				if (rMinEyeballBlackPixel > 100) {
					//rEyeState = eyepos->getEyeState(rMinEyeballRectShape, rMinEyeballBlackPixelRate, rMinEyeballBeta);
					rEyeState = 0;
				}
				else {
					rEyeState = 1;
				}

				if (detect->getEyes().size() > 0) {
						rEyeState = 0;//���Haarֱ��ʶ�����۾�
				}

				(lEyeState + rEyeState) == 2 ? eyeState = 1 : eyeState = 0;//�����۶��պϲ���պ�

				// ͳ���۾��պϵĴ���
				if (eyeState == 1) {//����۾��պ�
					eyeCloseNum++;					// ͳ�� eyeCloseNum �۾��պϴ���
					eyeCloseDuration++;

					/*if (TempBlink == 1) {
						int tempBlink = detect->getBlink();
						tempBlink++;
						detect->setBlink(tempBlink);//����գ�۴���
					}*/
					if (TempBlink == 0) {
						TempBlink++;
					}
					

					if (globalK == DETECTTIME) {
						// ���������ж�ȫ�Ǳ��������û�����ۺͼ�ⲻ�����������
						(eyeCloseDuration > maxEyeCloseDuration) ? maxEyeCloseDuration = eyeCloseDuration : maxEyeCloseDuration;
						eyeCloseDuration = 0;
					}
				}
				else {
					(eyeCloseDuration > maxEyeCloseDuration) ? maxEyeCloseDuration = eyeCloseDuration : maxEyeCloseDuration;
					eyeCloseDuration = 0;

					if (TempBlink == 1) {
						int tempBlink = detect->getBlink();
						tempBlink++;
						detect->setBlink(tempBlink);//����գ�۴���
						TempBlink = 0;
					}
				}
		} // �н��ж��Ƿ��⵽������if���

		// ��ʱ��ִ��һ��ѭ����ʱ��
		stop = clock();
		if (eyeState == 0) {
			cout << "�۾�״̬:��"  << endl;
		}
		else {
			cout << "�۾�״̬:��" << endl;
		}

		// ����ѭ��������������һ�μ�����
		if (globalK == DETECTTIME) {
			//printf("eyeCloseNum: %d\tmaxEyeCloseDuration: %d\n", eyeCloseNum, maxEyeCloseDuration);
			//printf("failFaceNum: %d\tmaxFailFaceDuration: %d\n", failFaceNum, maxFailFaceDuration);

			// ����ƣ��״̬���б�
			//Fatigue isFatigue(FATIGUETHRESHOLD, eyeCloseNum, maxEyeCloseDuration, failFaceNum, maxFailFaceDuration);
			//fatigueState = recoFatigueState(FATIGUETHRESHOLD, eyeCloseNum, maxEyeCloseDuration, failFaceNum, maxFailFaceDuration);

			recoFatigueState = new Fatigue(FATIGUETHRESHOLD, eyeCloseNum, maxEyeCloseDuration, failFaceNum, maxFailFaceDuration);
			fatigueState = recoFatigueState->getisTired(FATIGUETHRESHOLD);
			if (fatigueState == 1) {
				cout << "��ʻԱ����ƣ�ͼ�ʻ״̬" << endl << endl;
				cout << "�ѽ���ƣ��״̬�޷�ͳ��գ�ۺ͵�ͷƵ��" << endl;
				//detect->setBlink(0);
				//detect->setBow(0);

				//cout << "գ��Ƶ��:" << detect->getBlinkper(TIMETOTAL) << "��/10��" << endl;
				//cout << "��ͷƵ��:" << detect->getBowper(TIMETOTAL) << "��/10��" << endl;
				//TempBow++;
				//detect->setBlink(0);
				//detect->setBow(0);
			}
			else if (fatigueState == 0) {
				cout << "��ʻԱ����������ʻ״̬" << endl << endl;
				cout << "գ��Ƶ��:" << detect->getBlinkper(TIMETOTAL) << "��/10��" << endl;
				cout << "��ͷƵ��:" << detect->getBowper(TIMETOTAL) << "��/10��" << endl;
				detect->setBlink(0);
				detect->setBow(0);
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
			char c = cvWaitKey(0);
			if (c == 27)
				break;
			else
				continue;
			}
		}
	}
	return 0;
}

