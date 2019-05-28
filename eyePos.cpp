#include"eyePos.h"
#include<iostream>
using namespace std;

int EyePos::getEyePos(int* project, int size, int region)
{
	projectArr* projectStruct = nullptr;
	projectArr* projectTemp = nullptr;
	int i, j, pos, sizeTemp, temp;

	// ����projectStruct�ڴ�ռ�
	projectStruct = (projectArr*)malloc(size * sizeof(projectArr));
	projectTemp = (projectArr*)malloc(sizeof(projectArr));

	// ��ʼ���ڴ�ռ�
	for (i = 0; i < size; i++) {
		(projectStruct + i)->data = *(project + i);
		(projectStruct + i)->index = i;
	}

	// ��project��С�����������
	for (i = 0; i <= size - 2; i++) {
		for (j = 0; j < size - i - 1; j++) {
			if ((projectStruct + j)->data > (projectStruct + j + 1)->data) {
				*projectTemp = *(projectStruct + j);
				*(projectStruct + j) = *(projectStruct + j + 1);
				*(projectStruct + j + 1) = *projectTemp;
			}
		}
	}

	// Ѱ���м��������Сֵ����λ��
	sizeTemp = size / 2;
	temp = 0;
	for (i = 0; i < size; i++) {
		temp = (projectStruct + i)->index;
		if ((temp > sizeTemp - region) && (temp < sizeTemp + region)) {
			pos = (projectStruct + i)->index;
			// ��ָֹ��Խ�����λ��Ԫ�س��ָ���
			if (pos < 0)
				return -1;
			break;
		}
		else {
			// ��ֹ�������в����ڷ���������Ԫ��
			if (i == size - 1)
				return -1;
		}
	}
	free(projectTemp);
	return pos;
}


/************************************************************
���ܣ���������ͶӰͼ�е���͵㣬�Ӷ�����üë�ĺ���
���룺
	int* horiProject: ���е�ָ��
	int width:  ���еĿ��
	int height: ���еĸ߶�
	int threshold���ָ�üë����ֵ�����
�����
	�����ҵ�����͵���λ�ã����Ϊint���ͣ���üë���۾��ķָ���
˵����
		1. ����üëʱ���Ե���eyeBrowThreshold������ȥ����Ч��
		2. ͬʱ���Ե�������������ֵ�Ĵ���count������Ч����
************************************************************/
int EyePos::removeEyebrow(int* horiProject, int width, int height, int threshold)
{
	int temp, temp1, count, flag, i;
	int eyeRow;
	int eyeBrowThreshold;

	// ��λ����λ��
	eyeBrowThreshold = (width - threshold) * 255;// Ϊ�˷�ֹ�޷������۾���üë����������ʵ�������ֵ

	// ����üë����
	temp = 100000000;
	temp1 = 0;
	count = 0;
	flag = 0;										// ��ʾ��ǰ������λ���ڵ�һ����͹�֮ǰ
	eyeRow = 0;
	for (i = 0; i < height; i = i + 3) {
		count++;
		// �൱�ڵ����˲�
		temp1 = *(horiProject + i) + *(horiProject + i + 1) + *(horiProject + i + 2);
		if ((temp1 < temp) & (flag == 0)) {
			temp = temp1;
			eyeRow = i;
			count = 0;
		}
		if (count >= 3 || i >= height - 2) {
			flag = 1;
			break;
		}
	}

	// ������һ�������۾���üë�ָ���ֵ�ĵ�
	count = 0;
	for (i = eyeRow; i < height; i++) {
		if (*(horiProject + i) > eyeBrowThreshold) {
			eyeRow = i;
			count++;
			if (count >= 3) {				// count: ͳ�ƹ��ж����������е�ͶӰֵ������ֵ��
				eyeRow = i;
				break;
			}
		}
		else
			count = 0;
	}

	// ��ֹû��üë��ɾ�۾������������ʵ������������
	if (eyeRow >= height / 2) {
		eyeRow = 0;
	}

	return eyeRow;
}


/************************************************************
���ܣ��ض����ܺ������������۵����Ĵ��¼����ۿ�������
���룺
	CvRect* eyeRect: �ۿ����������ָ��
	int width:  ���еĿ��
	int height: ���еĸ߶�
	int EyeCol����Ĥ�������ڵ���λ��
	int EyeRow����Ĥ�������ڵ���λ��
�����
	��ָ��ķ�ʽ�����ۿ��Ĵ�������eyeRect
************************************************************/
void EyePos::calEyeSocketRegion(Rect* eyeRect, int width, int height, int EyeCol, int EyeRow) {
	int temp, temp1;
	temp = EyeCol - width / 4;
	temp1 = EyeRow - height / 4;
	if ((temp < 0) && (temp1 < 0)) {
		eyeRect->x = 0;
		eyeRect->width = width / 2 + temp;
		eyeRect->y = 0;
		eyeRect->height = height / 2 + temp1;
	}
	else if ((temp < 0) && (temp1 > 0)) {
		eyeRect->x = 0;
		eyeRect->width = width / 2 + temp;
		eyeRect->y = temp1;
		eyeRect->height = height / 2;
	}
	else if ((temp > 0) && (temp1 < 0)) {
		eyeRect->x = temp;
		eyeRect->width = width / 2;
		eyeRect->y = 0;
		eyeRect->height = height / 2 + temp1;
	}
	else if ((temp > 0) && (temp1 > 0)) {
		eyeRect->x = temp;
		eyeRect->width = width / 2;
		eyeRect->y = temp1;
		eyeRect->height = height / 2;
	}
}



/************************************************************
���ܣ��ض����ܺ���������������С�ľ�������
���룺
	CvRect* eyeRect: ������С�ľ��������ָ��
	int* horiProject
	int* vertProject
	int width:  ���еĿ��
	int height: ���еĸ߶�
	int horiThreshold��ˮƽ�������ֵ
	int vertThreshold����ֱ�������ֵ
�����
	ͨ��ָ�뷵��CvRect* eyeRect: ������С�ľ��������ָ��
************************************************************/
void EyePos::getEyeMinRect(Rect* eyeRect, int* horiProject, int* vertProject, int width, int height, int horiThreshold, int vertThreshold)
{
	int temp, temp1, i;

	temp1 = (width - horiThreshold) * 255;
	for (i = 0; i < height; i++) {
		if (*(horiProject + i) < temp1) {
			eyeRect->y = i;
			break;
		}
	}
	temp = i;				// ��¼eyeRectTemp.y��λ��
	//printf("eyeRectTemp->y: %d\n", eyeRect->y);

	if (temp != height) {
		// temp != HEIGHT: ��ֹû�з���*(subhoriProject + i) < temp1������λ�ã����temp != HEIGHT��һ��������������λ�ô���
		for (i = height - 1; i >= 0; i--) {
			if (*(horiProject + i) < temp1) {
				temp = i;
				break;
			}
		}
		if (temp == eyeRect->y)
			eyeRect->height = 1;
		else
			eyeRect->height = temp - eyeRect->y;
	}
	else {
		eyeRect->height = 1;
	}

	temp1 = (height - vertThreshold) * 255;
	for (i = 0; i < width; i++) {
		if (*(vertProject + i) < temp1) {
			eyeRect->x = i;
			break;
		}
	}
	temp = i;			// ��¼eyeRectTemp.x��λ��

	if (temp != width) {
		for (i = width - 1; i >= 0; i--) {
			if (*(vertProject + i) < temp1) {
				temp = i;
				break;
			}
		}
		// ��ֹ���Ϊ0����ʾͼ��ʱ����
		if (temp == eyeRect->x)
			eyeRect->width = 1;
		else
			eyeRect->width = temp - eyeRect->x;
	}
	else {
		eyeRect->width = 1;
	}
}


/****************************** �ж��۾�״̬ ************************************************
���ܣ�ͳ��lMinEyeballImg�е�1/2�����ں����صı���
���룺
	int* vertProject����ֱͶӰ���
	CvRect* eyeRect����С�ۿ�������
	int width����ֱͶӰ���еĳ���
	int eyeCol����С�ۿ��к�Ĥ���������ڵ�����
	int MinEyeballBlackPixel����С�ۿ��еĺ�����������
�����
	double MinEyeballBeta���۾�����1/2�����ɫ���ص�ռ�ܺ����ص�ı���
********************************************************************************************/
double EyePos::calMiddleAreaBlackPixRate(int* vertProject, Rect* eyeRect, int width, int height, int eyeCol, int MinEyeballBlackPixel) {
	int temp, temp1, count, i;
	double MinEyeballBeta;
	temp1 = 0;				// �м�1/2����İ����ظ���
	temp = 0;				// �м�1/2����ĺ����ظ���
	count = 0;				// ��ʱ����
	MinEyeballBeta = 0;	// ��ʼ���м�1/2����ĺ����صı���
	if (width >= 8) {
		temp = eyeCol - eyeRect->x;// ��С�ۿ��ĺ�Ĥ���ĵ����ڵ���
		// ����if else �Ǳ����Դ���
		if (width / 2 > temp) {
			// ��ֹ��߽�Խ������
			count = temp + width / 4;
			temp1 = temp - width / 4;
			if (temp1 < 0)
				i = 0;
			else
				i = temp1;
		}
		else {
			// ��ֹ�ұ߽�Խ������
			i = temp - width / 4;
			temp1 = temp + width / 4;
			if (temp1 < width)
				count = temp1;
			else
				count = width;
		}
		temp1 = 0;
		temp = 0;
		for (; i < count; i++)
			temp1 += *(vertProject + i);
		temp1 /= 255;
		temp = height * (width / 2) - temp1;								// �м�1/2����ĺ����ظ���
		MinEyeballBeta = (double)temp / (double)MinEyeballBlackPixel;		// �м�1/2����ĺ�����ռ���к����صı���
	}
	else {
		MinEyeballBeta = 0;
	}

	return MinEyeballBeta;
}



int EyePos::getEyeState(double MinEyeballRectShape, double MinEyeballBlackPixelRate, double MinEyeballBeta)
{
	//�ۿ����εĳ����  ��С�ۿ���ɫ������ռ����  1/2�����ɫ������ռ����
	int eyeState;//�۾�״̬
	int funcResult;//ģ�����۽��
	int shapeFuzzyLv, pixelFuzzyLv, betaFuzzyLv;	// ����������Ӧ��ģ�������ֵ

	// �ж��۾���������ĳ���ȵ�ģ������
	shapeFuzzyLv = 0;
	//�������ȹ��� ����Ϊ�۾��Ѿ�����
	/*if ((MinEyeballRectShape >= 0) && (MinEyeballRectShape <= 0.8)) {
		shapeFuzzyLv = 0;
	}
	else if (MinEyeballRectShape <= 1.2) {
		shapeFuzzyLv = 2;
	}
	else if (MinEyeballRectShape <= 1.5) {
		shapeFuzzyLv = 6;
	}
	else if (MinEyeballRectShape <= 2.5) {
		shapeFuzzyLv = 8;
	}
	else if (MinEyeballRectShape <= 3) {
		shapeFuzzyLv = 6;
	}
	else {
		shapeFuzzyLv = 0;
	}*/

	if ((MinEyeballRectShape >= 0) && (MinEyeballRectShape <= 0.8)) {
		shapeFuzzyLv = 8;
	}
	else if (MinEyeballRectShape <= 1.2) {
		shapeFuzzyLv = 6;
	}
	else if (MinEyeballRectShape <= 1.5) {
		shapeFuzzyLv = 4;
	}
	else if (MinEyeballRectShape <= 2.5) {
		shapeFuzzyLv = 2;
	}
	else if (MinEyeballRectShape <= 3) {
		shapeFuzzyLv = 0;
	}
	else {
		shapeFuzzyLv = 0;
	}

	// �ж��۾�������������ص���ռ������ģ������
	pixelFuzzyLv = 0;
	if ((MinEyeballBlackPixelRate >= 0) && (MinEyeballBlackPixelRate <= 0.4)) {
		pixelFuzzyLv = 0;
	}
	else if (MinEyeballBlackPixelRate <= 0.50) {
		pixelFuzzyLv = 2;
	}
	else if (MinEyeballBlackPixelRate <= 0.60) {
		pixelFuzzyLv = 6;
	}
	else if (MinEyeballBlackPixelRate <= 1) {
		pixelFuzzyLv = 8;
	}

	// �ж��۾�����1/2�����ɫ���ص�ռ�ܺ����ص�ı�����ģ������
	betaFuzzyLv = 0;
	if ((MinEyeballBeta >= 0) && (MinEyeballBeta <= 0.3)) {
		betaFuzzyLv = 0;
	}
	else if (MinEyeballBeta <= 0.45) {
		betaFuzzyLv = 2;
	}
	else if (MinEyeballBeta <= 0.6) {
		betaFuzzyLv = 6;
	}
	else if (MinEyeballBeta <= 1) {
		betaFuzzyLv = 8;
	}

	// ģ�����ۺ���
	eyeState = 1;		// Ĭ���Ǳ��۵�
	funcResult = 2 * shapeFuzzyLv + 4 * pixelFuzzyLv + 4 * betaFuzzyLv;

	if (funcResult >= 42) {//���ģ�����۽������40��û����
		eyeState = 0;
	}
	return eyeState;
}

int EyePos::removeEyeglasses(int* vertProject, int width, int height, int threshold)
{
	int temp, temp1, count, flag, i;
	int eyeRow;
	int eyeBrowThreshold;

	// ��λ����λ��
	eyeBrowThreshold = (width - threshold) * 255;// Ϊ�˷�ֹ�޷������۾����۾�����������ʵ�������ֵ

	// �����۾�����
	temp = 100000000;
	temp1 = 0;
	count = 0;
	flag = 0;										// ��ʾ��ǰ������λ���ڵ�һ����͹�֮ǰ
	eyeRow = 0;
	for (i = 0; i < width; i = i + 3) {
		count++;
		// �൱�ڵ����˲�
		temp1 = *(vertProject + i) + *(vertProject + i + 1) + *(vertProject + i + 2);
		if ((temp1 < temp) & (flag == 0)) {
			temp = temp1;
			eyeRow = i;
			count = 0;
		}
		if (count >= 3 || i >= width - 2) {
			flag = 1;
			break;
		}
	}

	// ������һ�������۾����۾��ָ���ֵ�ĵ�
	count = 0;
	for (i = eyeRow; i < width; i++) {
		if (*(vertProject + i) > eyeBrowThreshold) {
			eyeRow = i;
			count++;
			if (count >= 3) {				// count: ͳ�ƹ��ж����������е�ͶӰֵ������ֵ��
				eyeRow = i;
				break;
			}
		}
		else
			count = 0;
	}

	// ��ֹû��üë��ɾ�۾������������ʵ������������
	if (eyeRow >= width / 1.5) {
		eyeRow = 0;
	}

	return eyeRow;
}
