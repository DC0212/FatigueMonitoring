/******************************************************
���ܣ���Ostu�����䷽������ֵ����ֵ
���룺
	hist��ͼ���ֱ��ͼ����
	pixelSum��ͼ��������ܺ�
	CONST: һ��������Ϊ����Ӧ���������Ҫ�󣬿�ʵ�����ҵ������ŷָ���ֵ�Ļ����ϼ�ȥ�ó���
�����
	threshold��������ֵ
******************************************************/
#pragma once
#ifndef ostuThreshold_h
#define ostuThreshold_h

class OstuThreshold {//�����ֵ����
private:
	int threshold;
	int CON;
public:
	OstuThreshold(int* hist, int pixelSum, const int CONST);//�����ֵ����ֵ
	int getostu() { return (threshold - CON); };
};
#endif // !ostuThreshold_h
