#include"Fatigue.h"
#include<iostream>

using namespace std;
Fatigue::Fatigue(int thresh, int eyeCloseNum, int maxEyeCloseDuration, int failFaceNum, int maxFailFaceDuration) {
	// �������ĸ�ָ���Ȩֵ
	eyeCloseValue = eyeCloseNumTab[eyeCloseNum];
	eyeCloseDurationValue = eyeCloseDurationTab[maxEyeCloseDuration];
	failFaceValue = eyeCloseNumTab[failFaceNum];
	failFaceDurationValue = failFaceDurationTab[maxFailFaceDuration];
	// �ۺ�Ȩֵ
	compreValue = eyeCloseValue + eyeCloseDurationValue + failFaceValue + failFaceDurationValue;

	cout << "�۾��պϴ�����Ȩֵ:" << eyeCloseValue << endl;
	cout << "�۾������պϴ�����Ȩֵ:" << eyeCloseDurationValue << endl;
	cout << "δ��⵽�������ܴ�����Ȩֵ:" << failFaceValue << endl;
	cout << "����δ��⵽������Ȩֵ" << failFaceDurationValue << endl;
	cout << "�ۺ�Ȩֵ" << compreValue << endl;
}

int Fatigue::getcompreValue()
{
	return compreValue;
}

