#pragma once


// CMessageHandleUI

class CMessageHandleUI : public CWnd
{
	DECLARE_DYNAMIC(CMessageHandleUI)

	const static int WM_MAIN_WECHAT_HWND_MSG = WM_USER + 1;			//接收到微信传过来的微信主要窗口句柄
	const static int WM_MAIN_WECHAT_CLOSE_MSG = WM_USER + 2;		//微信关闭窗口消息

public:
	CString TITLE_ID;
	CString CLASS_NAME_ID;

private:
	HWND m_WechatTabUI;

public:
	CMessageHandleUI();
	virtual ~CMessageHandleUI();
	void Init();
	void SetWechatTabUI(HWND hw);

private:
	void HandleMessageHWnd(WPARAM wp, LPARAM lp);

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnWeChatHWNDMessage(WPARAM wParam, LPARAM lParam); 
	afx_msg LRESULT OnWeChatHWNDCloseMessage(WPARAM wParam, LPARAM lParam); 
};


