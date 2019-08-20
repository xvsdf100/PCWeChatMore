#pragma once

#include <vector>
#include "WeChatProcessInfo.h"
// WechatTab 对话框
// 微信tab整理界面
class WechatTab : public CDialogEx
{

	const static int WM_MAIN_WECHAT_HWND_MSG = WM_USER + 1;			//自定义协议
	const static int WM_MAIN_WECHAT_HWND_CLOSE_MSG = WM_USER + 2;			//自定义协议
	const static int IDC_BUTTON_AUTO_TAB = WM_USER + 3;				//动态创建的ID

	DECLARE_DYNAMIC(WechatTab)

public:
	WechatTab(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~WechatTab();

	bool AddItemUI(CString name, HWND hw_chat);
	void RemoveItemUI(HWND hw_chat);



// 对话框数据
	enum { IDD = IDD_DIALOG_TAB };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	afx_msg LRESULT OnWeChatHWNDMessage(WPARAM wParam, LPARAM lParam); 
	afx_msg LRESULT OnWeChatHWNDCloseMessage(WPARAM wParam, LPARAM lParam); 
	afx_msg void OnAutoTabButtonClick();

	DECLARE_MESSAGE_MAP()

private:
	int CalcTabWidth();								//计算当前tab的宽度
	CRect CalcTabRect(int index);					//计算Tab的高度和宽度
	CRect CalcNewTabRect();							//计算下一个新的Tab的rect
	CButton* CreateTabItem(CString name);
	void RefreshTabUI();
	bool IsCotain(HWND hw);
	HWND FindWechatFromTab();						//通过坐标获取对应的微信坐标
	std::vector<WeChatProcessInfo> CheckWechatIsBad();		//检测微信窗口是否有效，返回无效的窗口

private:
	std::vector<WeChatProcessInfo>	m_tab_button_list;
	int	m_begin_tab_index;							//当前界面第一个开始的索引
	int m_last_tab_width;							//记录上次tab宽度
};
