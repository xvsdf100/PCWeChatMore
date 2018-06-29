// InjectDll.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "InjectDll.h"
#include <stdio.h>  
#include <cstdlib>
#include "ProcessHandleHelper.h"


bool IsWeChat();
void CloseWxHandle(HANDLE mutex);
void UnHookLib();

// 这是导出变量的一个示例
INJECTDLL_API int nInjectDll=0;

// 这是导出函数的一个示例。
INJECTDLL_API int fnInjectDll(void)
{
	return 42;
}

// 这是已导出类的构造函数。
// 有关类定义的信息，请参阅 InjectDll.h
CInjectDll::CInjectDll()
{
	return;
}

void HandleWxInstanceMutex();
void CloseWxHandle(HANDLE mutex);

extern HMODULE g_Module;
HHOOK g_Hook = 0;
bool isKilled = false;			//是否把句柄杀死


//钩子回调函数
LRESULT CALLBACK HOOKProc(
							  _In_  int code,
							  _In_  WPARAM wParam,
							  _In_  LPARAM lParam
							  )
{
	HandleWxInstanceMutex();
	return CallNextHookEx(g_Hook, code, wParam, lParam);
}

bool StartHook()
{
	if (g_Hook == 0)
	{
		g_Hook = SetWindowsHookEx(WH_GETMESSAGE, HOOKProc, g_Module, 0);
		int error = GetLastError();

		wchar_t exeName[300] = {0};
		WCHAR szInfo[512] = { 0 };
		swprintf_s(szInfo, _countof(szInfo), L"钩子失败了吗？:%d", error);
		//将内容输出到debug信息中
		OutputDebugString(szInfo);

		return true;
	}
	return false;
}

bool UnHook()
{
	if (g_Hook!=0)
	{
		return UnhookWindowsHookEx(g_Hook) ? true : false;
	}
	return false;
}

/*
 *	如果不是微信的就释放掉---不能在回调释放，只能在消息里面才能啊
 */
void UnHookLib(){
	if(!IsWeChat()){
		FreeLibrary(g_Module);
	}
}

/*
 *	是否微信进程
 */
bool IsWeChat(){
	wchar_t exeName[300] = {0};
	GetModuleFileName(NULL,exeName, 300);
	if(wcsstr(exeName, L"WeChat.exe") != NULL){
		return true;

	}else{
		return false;
	}
}

void HandleWxInstanceMutex(){
	if(IsWeChat()){
		if(!isKilled){
			//_WeChat_App_Instance_Identity_Mutex_Name
			HANDLE hmutex = CreateMutex(NULL,TRUE,L"_WeChat_App_Instance_Identity_Mutex_Name");
			int err = GetLastError();
			if (err == ERROR_ALREADY_EXISTS)
			{
				
				CloseWxHandle(hmutex);
				CloseHandle(hmutex);
				isKilled = true;
			}else{
				
				OutputDebugString(L"不存在，我也要关闭了句柄");
				CloseHandle(hmutex);	//也要关掉
			}
		}
	}
}

void CloseWxHandle(HANDLE mutex){ 

	OutputDebugString(L"杀死微信APP");
	CloseProcessHandle(GetCurrentProcessId(), L"_WeChat_App_Instance_Identity_Mutex_Name");
}


