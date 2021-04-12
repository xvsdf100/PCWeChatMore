#include "stdafx.h"
#include "ProcessHandleHelper.h"
#include "stdio.h"
#include <assert.h>
//win8 以后
NTQUERYSYSTEMINFORMATION NtQuerySystemInformation = (NTQUERYSYSTEMINFORMATION)GetProcAddress(GetModuleHandleA("ntdll.dll"),"NtQuerySystemInformation");
NTQUERYOBJECT    NtQueryObject = (NTQUERYOBJECT)GetProcAddress(GetModuleHandleA("ntdll.dll"),"NtQueryObject");

static SYSTEM_INFORMATION_CLASS ex_information = SystemExtendedHandleInformation;//SystemExtendedHandleInformation;

//进程提权
BOOL ElevatePrivileges()
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;
	tkp.PrivilegeCount = 1;
	if(!OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY,&hToken))
		return FALSE;
	LookupPrivilegeValue(NULL,SE_DEBUG_NAME,&tkp.Privileges[0].Luid);
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	if(!AdjustTokenPrivileges(hToken,FALSE,&tkp,sizeof(TOKEN_PRIVILEGES),NULL,NULL))
	{
		return FALSE;
	}

	return TRUE;
}

/*
 *	方便复制任何进程句柄
 */
HANDLE DuplicateHandleEx(DWORD pid, HANDLE h, DWORD flags)
{
	HANDLE hHandle = NULL;

	HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if(hProc)
	{
		if(!DuplicateHandle(hProc,
			(HANDLE)h, GetCurrentProcess(),
			&hHandle, 0, FALSE, /*DUPLICATE_SAME_ACCESS*/flags))
		{
			hHandle = NULL;
		}
	}

	CloseHandle(hProc);
	return hHandle;
}

/*
 *	遍历系统所有的句柄
 */
PVOID ZwQuerySystemInformationEx(){

	static ULONG initialBufferSize = 0x10000;
	DWORD dwSize = initialBufferSize;
	PVOID pbuffer = NULL;
	NTSTATUS Status;
	

	pbuffer = VirtualAlloc(NULL, dwSize, MEM_COMMIT, PAGE_READWRITE);
	Status = NtQuerySystemInformation(ex_information, pbuffer, dwSize, &dwSize);
	if(!NT_SUCCESS(Status) && Status != STATUS_INFO_LENGTH_MISMATCH){
		ex_information = SystemHandleInformation;	//逆向微软 handle.exe 代码，兼容性可能更加好
	}

	dwSize = initialBufferSize;

	while ((Status = NtQuerySystemInformation(ex_information, pbuffer, dwSize, &dwSize)) == STATUS_INFO_LENGTH_MISMATCH)
	{
		VirtualFree(pbuffer, 0, MEM_RELEASE);
		dwSize *= 2;

		// Fail if we're resizing the buffer to something very large.
		if (dwSize > PH_LARGE_BUFFER_SIZE)
			return NULL;

		pbuffer = VirtualAlloc(NULL, dwSize, MEM_COMMIT, PAGE_READWRITE);
	}

	if(!NT_SUCCESS(Status))
	{
		return NULL;
	}

	if (dwSize <= 0x200000) initialBufferSize = dwSize;	//增加下次查询效率

	return pbuffer;
}

/**
* @说明 兼容2种情况
* @参数 
* @返回 
* @时间 2021/1/21/10/20/25
*/
ULONG_PTR GetHandleProcessId(void* p_info, ULONG index){
	if(ex_information == SystemHandleInformation){
		PSYSTEM_HANDLE_INFORMATION1 info = (PSYSTEM_HANDLE_INFORMATION1)p_info;
		return info->Handles[index].UniqueProcessId;
	}else
	{
		PSYSTEM_HANDLE_INFORMATION_EX info = (PSYSTEM_HANDLE_INFORMATION_EX)p_info;
		return info->Handles[index].UniqueProcessId;
	}
}


ULONG_PTR GetHandle(void* p_info, ULONG index){
	if(ex_information == SystemHandleInformation){
		PSYSTEM_HANDLE_INFORMATION1 info = (PSYSTEM_HANDLE_INFORMATION1)p_info;
		return info->Handles[index].HandleValue;
	}else
	{
		PSYSTEM_HANDLE_INFORMATION_EX info = (PSYSTEM_HANDLE_INFORMATION_EX)p_info;
		return info->Handles[index].HandleValue;
	}
}

HANDLE GetProcessHandleInternal(PVOID pbuffer, int pid, wchar_t* name){

	DWORD dwSize = 0;
	POBJECT_NAME_INFORMATION pNameInfo;
	POBJECT_NAME_INFORMATION pNameType;
	NTSTATUS Status;
	ULONG nIndex = 0;
	DWORD dwFlags = 0;
	char szType[128] = {0};
	char szName[512] = {0};

	if(pbuffer == NULL || pid == 0 || name == NULL){
		return NULL;
	}


	PSYSTEM_HANDLE_INFORMATION1 pHandleInfo = (PSYSTEM_HANDLE_INFORMATION1)pbuffer;
	ULONG last_process = 0;

	ULONG handle_numer = pHandleInfo->NumberOfHandles;	//number都是第一成员变量，所以随便用哪个都可以
	for(nIndex = 0; nIndex < handle_numer; nIndex++)
	{
		ULONG_PTR UniqueProcessId = GetHandleProcessId(pHandleInfo, nIndex);
		ULONG_PTR HandleValue = GetHandle(pHandleInfo, nIndex);
		if(UniqueProcessId == pid)
		{
			HANDLE hHandle = DuplicateHandleEx(UniqueProcessId, 
				(HANDLE)HandleValue,
				DUPLICATE_SAME_ACCESS
				);
			if(hHandle == NULL) continue;


			Status = NtQueryObject(hHandle, ObjectNameInformation, szName, 512, &dwFlags);

			if (!NT_SUCCESS(Status))
			{
				CloseHandle(hHandle);
				continue;
			}


			Status = NtQueryObject(hHandle, ObjectTypeInformation, szType, 128, &dwFlags);

			CloseHandle(hHandle);	//不需要句柄了
			hHandle = NULL;

			if (!NT_SUCCESS(Status))
			{
				continue;
			}


			pNameInfo = (POBJECT_NAME_INFORMATION)szName;
			pNameType = (POBJECT_NAME_INFORMATION)szType;

			WCHAR TypName[1024] = {0};		//类型，可以判断也可以不判断，可能出现不同类型同一个名字吧
			WCHAR Name[1024] = {0};

			wcsncpy(TypName, (WCHAR*)pNameType->Name.Buffer, pNameType->Name.Length/2);
			wcsncpy(Name, (WCHAR*)pNameInfo->Name.Buffer, pNameInfo->Name.Length/2);

			if (wcsstr(Name, name))//_WeChat_App_Instance_Identity_Mutex_Name
			{
				return (HANDLE)HandleValue;
			}

		}

	}

	return NULL;
}

/*
 *	关闭指定进程的某个句柄，也可以自己的
 */
void CloseProcessHandle(DWORD pid, HANDLE handle){
	HANDLE hHandle = DuplicateHandleEx(pid, 
		handle,
		DUPLICATE_CLOSE_SOURCE		//复制过来的关闭原来的句柄,如果没有这个能力，基本做不到减少引用的作用
		);

	CloseHandle(hHandle);
}

/****************************************public部分，c申明感觉好麻烦，习惯别的语言，有点不习惯了******************************************************/

HANDLE GetProcessHandle(int pid, wchar_t* name){
	PVOID pbuffer = NULL;
	BOOL r = ElevatePrivileges();


	pbuffer = ZwQuerySystemInformationEx();
	if(pbuffer == NULL){
		return NULL;
	}

	HANDLE h = GetProcessHandleInternal(pbuffer, pid, name);
	VirtualFree(pbuffer, 0, MEM_RELEASE);
	return h;
}


bool CloseProcessHandle(int pid, wchar_t* name){
	HANDLE h = GetProcessHandle(pid, name);
	if(h != NULL){
		CloseProcessHandle(pid,h);
		return true;
	}else{
		return false;
	}
}