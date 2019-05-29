#include"detect.h"
#include<vector>
#include"Trans.h"
#include"histogram.h"
#include"ostuThreshold.h"

using namespace std;

Detect::Detect()
{
	cap = VideoCapture(0);
	face_cascade.load("Haar\\haarcascade_frontalface_default.xml");
	eye_cascade.load("Haar\\haarcascade_eye_tree_eyeglasses.xml");
}

void Detect::startDetect()
{
	if (checkisOpened()) {
		detectFace();
		showDetect();
	}
}

bool Detect::checkisOpened()
{
	if (cap.isOpened()) {
		return true;
	}
	return false;
}

void Detect::detectFace()
{
	//Mat img;
	cap >> img;
	//bool ret = cap.read(img);
	//Mat gray;
	gray.create(img.size(), img.type());
	cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
	//vector<Rect> faces;
	face_cascade.detectMultiScale(gray, faces, scale_factor, min_neighbors, flags);
	//cout << "��⵽��������:" << faces.size() << endl;
	for (int i = 0; i < faces.size(); i++) {
		Point Center;
		int radius;//�뾶
		Center.x = cvRound(faces[i].x + faces[i].width * 0.5);
		Center.y = cvRound(faces[i].y + faces[i].height * 0.5);
		radius = cvRound((faces[i].width + faces[i].height) * 0.25);

		circle(img, Center, radius, colors[5]);

		detectEyes(i);//����۾�
		detectMouth(gray,i);
		detectEyeswithoutHaar(i);
	}
	/*imshow("ƣ�ͼ�ʻ���", img);
	if (cv::waitKey(1) && 1 == 2) {
		break;
	}*/	
}

void Detect::detectEyes(const int index)
{
	Mat faceROI = gray(faces[index]);
	//vector<Rect>eyes;
	eye_cascade.detectMultiScale(faceROI, eyes, 1.3, 3, 0);
	//cout << "��⵽�۾�����:" << eyes.size() << endl;
	for (int j = 0; j < eyes.size(); j++) {
		//Rect rect(faces[i].x + eyes[j].x, faces[i].y + eyes[j].y, eyes[j].width, eyes[j].height);
		//rectangle(img, rect, colors[3], 2, 8, 0);
		Point Center;
		int radius;
		Center.x = cvRound(faces[index].x + eyes[j].x + eyes[j].width * 0.5);
		Center.y = cvRound(faces[index].y + eyes[j].y + eyes[j].height * 0.5);
		radius = cvRound(eyes[j].width + eyes[j].height) * 0.25;
		circle(img, Center, radius, colors[3]);
	}
}

void Detect::detectMouth(Mat grayimg, const int index)//����첿
{
	int hist[256];									// ���ֱ��ͼ������
	int pixelSum;									// ��ֵ����ͼƬ���ص�����
	int threshold;									// �洢��ֵ��������ֵ
	// ��������������֪ʶ�ָ�����µ��첿����
	Rect MyMouTh = faces.at(index);
	//Rect MyMouTh = MyFace;
	//Center.x = cvRound(MyMouTh.x + faces[index].width * 0.15);
	//Center.y = cvRound(MyMouTh.y + faces[index].height * 0.01);
	float TempWidth = MyMouTh.width / 3;//��������Ϊ3����
	MyMouTh.x = MyMouTh.x + TempWidth;//�첿����ʼX����
	//MyMouTh.height / 3
	MyMouTh.y = faces[index].y + faces[index].height*0.72;//�첿��ʼY����
	//MyMouTh.width = MyMouTh.width - 3 * TempWidth / 2;
	MyMouTh.width = TempWidth + faces[index].width * 0.1;//�첿���ο��
	MyMouTh.height = MyMouTh.height / 6 + faces[index].height * 0.001;//�첿���θ߶�
	//MyMouTh.height = MyMouTh.height / 2;

	IplImage Tempgray = grayimg;
	cvSetImageROI(&Tempgray, MyMouTh);				// ����ROIΪ��⵽���첿����
	IplImage* mouthImg;
	mouthImg = cvCreateImage(cvSize(MyMouTh.width, MyMouTh.height), IPL_DEPTH_8U, 1);
	cvCopy(&Tempgray, mouthImg, NULL);
	cvResetImageROI(&Tempgray);								// �ͷ�ROI
	cvShowImage("�첿", mouthImg);

	/********************************** ��ֵ������ ***********************************/
	cvSmooth(mouthImg, mouthImg, CV_MEDIAN);				// ��ֵ�˲� Ĭ�ϴ��ڴ�СΪ3*3
	Trans nonlineTrans(mouthImg, mouthImg, 0.8);			// �����Ե�����
	memset(hist, 0, sizeof(hist));						// ��ʼ��ֱ��ͼ������Ϊ0
	Histogram histogram(mouthImg, hist);					// ����ͼƬֱ��ͼ
	// ���������ֵ
	pixelSum = mouthImg->width * mouthImg->height;

	OstuThreshold ostuThreshold(hist, pixelSum, 50);	//�����ֵ���������ֵ
	threshold = ostuThreshold.getostu();
	cvThreshold(mouthImg, mouthImg, threshold, 255, CV_THRESH_BINARY);// ��ͼ���ֵ��
	// ��ʾ��ֵ�����ͼ��
	cvErode(mouthImg, mouthImg, NULL, 1);		//��ʴͼ��  
	cvDilate(mouthImg, mouthImg, NULL, 1);	//����ͼ��
	cvShowImage("��ֵ������첿", mouthImg);

	/*Mat Tempimg = cvarrToMat(mouthImg);
	Mat TempMouth = Tempimg.clone();
	vector<vector<Point>>contours;
	findContours(TempMouth, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);//Ѱ������
	vector<vector<Point>>::iterator iter = contours.begin();
	while (iter != contours.end()) {
		if (iter->size() < 5) {
			iter = contours.erase(iter);
		}
		else {
			++iter;
		}
	}
	drawContours(TempMouth, contours, -1, Scalar(255), CV_FILLED);
	//cvShowImage("ȥ������첿", mouthImg);
	imshow("ȥ������첿", TempMouth);*/

	Point Center;
	int radius;
	Center.x = cvRound(MyMouTh.x+ faces[index].width * 0.15);
	Center.y = cvRound(MyMouTh.y+ faces[index].height * 0.05);
	radius = cvRound(MyMouTh.width + MyMouTh.height) * 0.22;
	//img = cvarrToMat(grayimg);
	circle(img, Center, radius, colors[6]);

	cvReleaseImage(&mouthImg);
	//cvWaitKey(0);
}

void Detect::detectEyeswithoutHaar(const int index)
{
	if (faces.size() > 0 && eyes.size() == 0) {//���Haar��⵽������û�м�⵽�۾�
		Rect* largestFaceRect = nullptr;			// �洢��⵽�������������ο�
		float temp = 0;
		Point Center;
		int radius;
		IplImage* Srcimg;

		
		for (int i = 0; i < faces.size(); i++) {
			if ((faces.at(i).width * faces.at(i).height) > temp) {
				temp = faces.at(i).width * faces.at(i).height;
				largestFaceRect = new Rect(faces.at(i));
			}
		};
		temp = largestFaceRect->width / 8;
		// ��������������֪ʶ�ָ�����µ���������
		
		largestFaceRect->x = largestFaceRect->x + temp;
		largestFaceRect->width = largestFaceRect->width - 3 * temp / 2;
		largestFaceRect->height = largestFaceRect->height / 2;
		largestFaceRect->y = largestFaceRect->y + largestFaceRect->height / 2;
		largestFaceRect->height = largestFaceRect->height / 2;


		IplImage Tempimg = img;
		Srcimg = cvCloneImage(&Tempimg);
		cvSetImageROI(Srcimg, *largestFaceRect);				// ����ROIΪ��⵽��������������
		IplImage *faceImg = cvCreateImage(cvSize(largestFaceRect->width, largestFaceRect->height), IPL_DEPTH_8U, 1);
		//cvCopy(Srcimg, faceImg, NULL);
		faceImg = cvCloneImage(Srcimg);
		cvResetImageROI(Srcimg);								// �ͷ�ROI
		//cvShowImage("�ָ�����������", faceImg);
		//aitKey(0);

		Rect eyeRectTemp = *largestFaceRect;
		// ��������������֪ʶ�ָ�����µ���������
		largestFaceRect->width /= 2;
		cvSetImageROI(Srcimg, *largestFaceRect);				// ����ROIΪ��⵽��������������
		IplImage *lEyeImg = cvCreateImage(cvSize(largestFaceRect->width, largestFaceRect->height), IPL_DEPTH_8U, 1);
		//cvCopy(Srcimg, lEyeImg, NULL);
		lEyeImg = cvCloneImage(Srcimg);
		cvResetImageROI(Srcimg);	// �ͷ�ROI

		/********************��ʶ****************************/
		Center.x = cvRound(eyeRectTemp.x + faces[index].width *0.15);
		Center.y = cvRound(eyeRectTemp.y + faces[index].height * 0.15);
		radius = cvRound(eyeRectTemp.width + eyeRectTemp.height) * 0.1;
		circle(img, Center, radius, colors[3]);
		/************************************************/

		//cvShowImage("���µ���������", lEyeImg);
		//waitKey(0);

		// ��������������֪ʶ�ָ�����µ���������
		eyeRectTemp.x += eyeRectTemp.width / 2;
		eyeRectTemp.width /= 2;
		cvSetImageROI(Srcimg, eyeRectTemp);					// ����ROIΪ��⵽��������������
		IplImage *rEyeImg = cvCreateImage(cvSize(eyeRectTemp.width, eyeRectTemp.height), IPL_DEPTH_8U, 1);
		//cvCopy(Srcimg, rEyeImg, NULL);
		faceImg = cvCloneImage(Srcimg);
		rEyeImg = cvCloneImage(Srcimg);
		cvResetImageROI(Srcimg);								// �ͷ�ROI

		/********************��ʶ****************************/
		Center.x = cvRound(eyeRectTemp.x + faces[index].width * 0.15);
		Center.y = cvRound(eyeRectTemp.y + faces[index].height * 0.15);
		radius = cvRound(eyeRectTemp.width + eyeRectTemp.height) * 0.17;
		circle(img, Center, radius, colors[3]);
		/************************************************/
		//cvShowImage("���µ���������", rEyeImg);
		//waitKey(0);

		cvReleaseImage(&Srcimg);
		cvReleaseImage(&faceImg);
		cvReleaseImage(&rEyeImg);
		cvReleaseImage(&lEyeImg);
		delete largestFaceRect;
	}
}

void Detect::showDetect()
{
	imshow("ƣ�ͼ�ʻ���", img);
}
