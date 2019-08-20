// MessageHandleUI.cpp : 实现文件
//

#include "stdafx.h"
#include "WxMoreInstance.h"
#include "MessageHandleUI.h"


// CMessageHandleUI

IMPLEMENT_DYNAMIC(CMessageHandleUI, CWnd)

CMessageHandleUI::CMessageHandleUI()
{
	CLASS_NAME_ID = AfxRegisterWndClass( CS_VREDRAW | CS_HREDRAW);
	TITLE_ID = L"CORE_MESSAGE_WX_WND";
	m_WechatTabUI = 0;
}

CMessageHandleUI::~CMessageHandleUI()
{
}


void CMessageHandleUI::Init()
{
	CRect rect(0,0,0,0);
	CreateEx( 0, CLASS_NAME_ID, TITLE_ID, 0 , -1000 , -1000 , -800 , -200 , NULL , (HMENU )NULL, NULL);
	ShowWindow(SW_HIDE);

}


void CMessageHandleUI::SetWechatTabUI(HWND hw)
{
	m_WechatTabUI = hw;
}

LRESULT CMessageHandleUI::OnWeChatHWNDMessage(WPARAM wParam, LPARAM lParam)
{
	//if()
	//TRACE("收到微信发送过来的消息");
	::PostMessage(m_WechatTabUI, WM_MAIN_WECHAT_HWND_MSG, wParam, lParam);
	return 0;
}

LRESULT CMessageHandleUI::OnWeChatHWNDCloseMessage(WPARAM wParam, LPARAM lParam)
{
	::PostMessage(m_WechatTabUI, WM_MAIN_WECHAT_CLOSE_MSG, wParam, lParam);
	return 0;
}

BEGIN_MESSAGE_MAP(CMessageHandleUI, CWnd)
	ON_MESSAGE(WM_MAIN_WECHAT_HWND_MSG,OnWeChatHWNDMessage)
	ON_MESSAGE(WM_MAIN_WECHAT_CLOSE_MSG,OnWeChatHWNDCloseMessage)
END_MESSAGE_MAP()



// CMessageHandleUI 消息处理程序


