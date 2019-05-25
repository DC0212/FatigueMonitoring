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
	int eyeCloseValue;				// �۾��պϴ����Ĺ���ֵ
	int eyeCloseDurationValue;		// �۾������պϴ����Ĺ���ֵ
	int failFaceValue;				// δ��⵽�������ܴ����Ĺ���ֵ
	int failFaceDurationValue;		// ����δ��⵽�����Ĺ���ֵ
	int compreValue;				// �ۺϹ���ֵ
	int eyeCloseNumTab[31] = { 2,2,4,6,9,14,20,29,39,50,61,72,80,86,91,94,96,98,98,99,99,100,100,100,100,100,100,100,100,100, 100 };
	int eyeCloseDurationTab[31] = { 2, 4, 9, 18, 32, 50, 68, 82, 91, 95, 98, 99, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100 };
	int failFaceDurationTab[31] = { 2, 6, 14, 29, 50, 71, 86, 94, 98, 99, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100 };

public:
	Fatigue(int thresh, int eyeCloseNum, int maxEyeCloseDuration, int failFaceNum, int maxFailFaceDuration);
	int getcompreValue();
};
#endif
