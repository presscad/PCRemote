// testdll2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>

int main(int argc, char* argv[])
{
		char strHost[] = "192.168.128.1";//"127.0.0.1";          //�������ߵ�ַ
	int  nPort = 8888;                     //�������߶˿�
	//��������dll
	HMODULE hServerDll = LoadLibrary("MainDll.dll");
	//����������������--�鿴��һ�ڵ�����TestRun����
	typedef void(_cdecl* TestRunT)(char* strHost, int nPort);
	//Ѱ��dll�е�������
	TestRunT pTestRunT = (TestRunT)GetProcAddress(hServerDll, "TestRun");
	//�жϺ����Ƿ�Ϊ��
	if (pTestRunT != NULL)
	{
		pTestRunT(strHost, nPort);   //�����������
	}
	printf("Hello World!\n");
	return 0;
}

