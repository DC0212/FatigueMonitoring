/*************************************************
���ܣ��ض����ܺ������������۾��պ�״̬���Ƿ��⵽����
					  �жϼ�ʻ״̬��������ƣ�ͣ�
���룺
	int eyeCloseNum�����������۾���״̬���ܴ���
	int maxEyeCloseDuration�����������۾������պϵ�������
	int failFaceNum����������δ��⵽�������ܴ���
	int maxFailFaceDuration��������������δ��⵽������������
**************************************************/
#pragma once
#ifndef FATIGUE_H
#define FATIGUE_H



class Fatigue {
private:
	int eyeCloseValue;						// �۾��պϴ�����Ȩֵ
	int eyeCloseDurationValue;				// �۾������պϴ�����Ȩֵ
	int failFaceValue;						// δ��⵽�������ܴ�����Ȩֵ
	int failFaceDurationValue;				// ����δ��⵽������Ȩֵ
	int compreValue;						// �ۺ�Ȩֵ
	int eyeCloseNumTab[32] = { 2,2,4,6,9,14,20,29,39,50,61,72,80,86,91,94,96,98,98,99,99,100,100,100,100,100,100,100,100,100, 100 ,100};
	//�۾��պϴ���Ȩֵ�ֲ�
	int eyeCloseDurationTab[32] = { 2, 4, 9, 18, 32, 50, 68, 82, 91, 95, 98, 99, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,100 };
	//�۾������պϴ���Ȩֵ�ֲ�
	int failFaceDurationTab[32] = { 2, 6, 14, 29, 50, 71, 86, 94, 98, 99, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,100 };
	//�����������ʧ�ܴ���Ȩֵ�ֲ�
	//���ۺ�δ��⵽��������Խ��,�ۺ�ȨֵԽ�󣬽���ƣ��״̬�Ŀ�����Խ��
public:
	Fatigue(int thresh, int eyeCloseNum, int maxEyeCloseDuration, int failFaceNum, int maxFailFaceDuration);
	int getisTired(int thresh);				//��ȡ�Ƿ�ƣ��
};
#endif
