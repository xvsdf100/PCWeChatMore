// InjectDll.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "InjectDll.h"
#include <stdio.h>  
#include <cstdlib>
#include "ProcessHandleHelper.h"
#include <string>

const int WM_CORE_WECHAT_MESSAGE_ = WM_USER + 1;
const int WM_CORE_WECHAT_EXIT_MESSAGE = WM_USER + 2;	//对应的窗口关闭了

bool IsWeChat();
void CloseWxHandle(HANDLE mutex);
void UnHookLib();
bool isWechatClose();
void HandleFirstHook();
void HandleExitMessage(HWND hw);

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
bool g_is_init = false;


//钩子回调函数
LRESULT CALLBACK HOOKProc(
							  _In_  int code,
							  _In_  WPARAM wParam,
							  _In_  LPARAM lParam
							  )
{
	if(IsWeChat()){
		HandleWxInstanceMutex();
		PMSG pmsg = (PMSG)lParam;
		if (pmsg->message == WM_CLOSE){
			//窗口关闭卸载钩子
			//OutputDebugString(L"微信窗口关闭了");
			HandleExitMessage(pmsg->hwnd);
			UnHook();
		}else
		{
			if(pmsg->message != WM_DESTROY){
				HandleFirstHook();
			}
			
		}

		
		return CallNextHookEx(g_Hook, code, wParam, lParam);
	}else{


		//UnHook();
		return FALSE;
	}
}

bool StartHook()
{
	if (g_Hook == 0)
	{
		g_Hook = SetWindowsHookEx(WH_GETMESSAGE, HOOKProc, g_Module, 0);
		int error = GetLastError();

		wchar_t exeName[300] = {0};
		WCHAR szInfo[512] = { 0 };
		//swprintf_s(szInfo, _countof(szInfo), L"钩子失败了吗？:%d\r\n", error);
		//将内容输出到debug信息中
		//OutputDebugString(szInfo);

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
		if(!isWechatClose()){
			//_WeChat_App_Instance_Identity_Mutex_Name
			HANDLE hmutex = CreateMutex(NULL,TRUE,L"_WeChat_App_Instance_Identity_Mutex_Name");
			int err = GetLastError();
			if (err == ERROR_ALREADY_EXISTS)
			{
				
				CloseWxHandle(hmutex);
				CloseHandle(hmutex);
				isKilled = true;
			}else{
				
				//OutputDebugString(L"不存在，我也要关闭了句柄");
				CloseHandle(hmutex);	//也要关掉
			}
		}
	}
}


bool isWechatClose(){
	bool ret = true;
	HANDLE hmutex = CreateMutex(NULL,TRUE,L"_WeChat_App_Instance_Identity_Mutex_Name");
	int err = GetLastError();
	if (err == ERROR_ALREADY_EXISTS)
	{
		ret = false;
	}

	CloseHandle(hmutex);
	return ret;
}

void CloseWxHandle(HANDLE mutex){ 

	//OutputDebugString(L"杀死微信APP");
	CloseProcessHandle(GetCurrentProcessId(), L"_WeChat_App_Instance_Identity_Mutex_Name");
	//int err = GetLastError();
	//WCHAR buffer[256] = {0};
	//wsprintf(buffer, L"错误码:%d", err );
	//OutputDebugString(buffer);
}

HWND FindWechatMessageUI(){
	return ::FindWindow(NULL, L"CORE_MESSAGE_WX_WND");
}

//第一次Hook微信做的初始化工作
void HandleFirstHook(){
	HWND hw =  GetForegroundWindow();
	WCHAR text[256] = {0};
	GetWindowText(hw,text, 256);
	//OutputDebugString(text);

	GetClassName(hw,text, 256);
	std::wstring class_name = text;
	//OutputDebugString(text);
	//WeChatLoginWndForPC
	//WeChatMainWndForPC
	std::wstring check_class_name = L"WeChatMainWndForPC";
	if(class_name == check_class_name){
		HWND core_hwnd = FindWechatMessageUI();
		if(core_hwnd !=NULL){
			PostMessage(core_hwnd, WM_CORE_WECHAT_MESSAGE_, 0, (LPARAM)hw);
		}else{
			//OutputDebugString(L"没有找到窗口");
		}
	}
}

void HandleExitMessage(HWND hw){
	//WM_CORE_WECHAT_EXIT_MESSAGE
	HWND core_hwnd = FindWechatMessageUI();
	WCHAR text[256] = {0};
	if(core_hwnd !=NULL){
		swprintf_s(text,_countof(text), L"发送了消息-关闭消息:%d\r\n", hw);
		OutputDebugString(text);
		PostMessage(core_hwnd, WM_CORE_WECHAT_EXIT_MESSAGE, (WPARAM)hw, 0);	//不一定核心关闭窗口关闭了
	}else{
		OutputDebugString(L"没有找到窗口");
	}
}

//保存当前顶层窗口句柄
void SaveWechatHWND(){
	//
}


//获取业务消息消息HWND
HWND GetSuperMessageHWND(){
	return 0;
}