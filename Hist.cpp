#include"Hist.h"

Hist::Hist(IplImage* srcImg, int* horiProj, int* vertProj)
{
	int i, j;
	uchar* ptr = NULL;					// ָ��ͼ��ǰ���׵�ַ��ָ��
	uchar* temp = NULL;
	int HEIGHT = srcImg->height;
	int WIDTH = srcImg->width;


	for (i = 0; i < HEIGHT; i++) {
		ptr = (uchar*)(srcImg->imageData + i * srcImg->widthStep);
		for (j = 0; j < WIDTH; j++) {
			temp = ptr + j;				// ���ټ�����
			*(horiProj + i) += *temp;	// ����ˮƽ�����ͶӰ
			*(vertProj + j) += *temp;	// ���㴹ֱ�����ͶӰ
		}
	}

}

void Hist::histProject(IplImage* srcImg, int* horiProj, int* vertProj) {
	int i, j;
	uchar* ptr = NULL;					// ָ��ͼ��ǰ���׵�ַ��ָ��
	uchar* temp = NULL;
	int HEIGHT = srcImg->height;
	int WIDTH = srcImg->width;


	for (i = 0; i < HEIGHT; i++) {
		ptr = (uchar*)(srcImg->imageData + i * srcImg->widthStep);
		for (j = 0; j < WIDTH; j++) {
			temp = ptr + j;				// ���ټ�����
			*(horiProj + i) += *temp;	// ����ˮƽ�����ͶӰ
			*(vertProj + j) += *temp;	// ���㴹ֱ�����ͶӰ
		}
	}
}
