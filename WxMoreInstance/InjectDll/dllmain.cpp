// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include <stdio.h>  
#include <cstdlib>


HMODULE g_Module = NULL;

void DoSome(HMODULE hModule);


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		g_Module = hModule;
		DoSome(hModule);
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

/*
 *	是否微信进程
 */
bool IsWeChat111(){
	wchar_t exeName[300] = {0};
	GetModuleFileName(NULL,exeName, 300);
	if(wcsstr(exeName, L"WeChat.exe") != NULL){
		return true;

	}else{
		return false;
	}
}


void DoSome(HMODULE hModule){
	if(!IsWeChat111()){
		//FreeLibrary(hModule);
	}
	
}
