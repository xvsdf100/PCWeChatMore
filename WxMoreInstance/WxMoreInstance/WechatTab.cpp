// WechatTab.cpp : 实现文件
//

#include "stdafx.h"
#include "WxMoreInstance.h"
#include "WechatTab.h"
#include "afxdialogex.h"


// WechatTab 对话框
IMPLEMENT_DYNAMIC(WechatTab, CDialogEx)

WechatTab::WechatTab(CWnd* pParent /*=NULL*/)
	: CDialogEx(WechatTab::IDD, pParent)
{
	m_last_tab_width = 100;
}

WechatTab::~WechatTab()
{
}

void WechatTab::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

LRESULT WechatTab::OnWeChatHWNDMessage(WPARAM wParam, LPARAM lParam)
{
	HWND hw = (HWND)lParam;
	if(!IsCotain(hw)){
		CString str;
		int index = m_tab_button_list.size() + 1;
		str.Format(L"微信%d", index);
		AddItemUI(str, hw);
	}
	return 0;
}

//遍历所有微信窗口，是否微信窗口已经关闭了，如果关闭就关掉tab
//这里逻辑可以扩展
LRESULT WechatTab::OnWeChatHWNDCloseMessage(WPARAM wParam, LPARAM lParam)
{
	OutputDebugString(L"收到微信关闭窗口消息\r\n");
	HWND hw = (HWND)wParam;
	RemoveItemUI(hw);
	return 0;
}

void WechatTab::OnAutoTabButtonClick()
{
	HWND hw = FindWechatFromTab();
	::SetForegroundWindow(hw);
	::ShowWindow(hw, SW_SHOW);
	//MessageBox(L"hello world");
}

//判断当前增加一个tab会不会显示完整，如果能显示完整那么当前宽度就是默认值
//如果不能显示，那么就重新计算当前的宽度
int WechatTab::CalcTabWidth()
{
	int width = 100;
	int new_size = m_tab_button_list.size() + 1;
	int start_x = 0;
	int total_widht = 0;
	int gap = 2;
	total_widht = start_x + new_size * width + (new_size - 1) * gap;

	//获取UI显示宽度
	CRect parent_rect;
	this->GetWindowRect(&parent_rect);
	if(total_widht > parent_rect.Width()){
		width = parent_rect.Width() / new_size - gap;
		//TRACE(L"w:%d p:%d \n", width, parent_rect.Width());
	}
	return width;
}

CRect WechatTab::CalcTabRect(int index)
{
	CRect parent_rect;
	this->GetWindowRect(&parent_rect);

	int gap = 2;	//间隔10px
	int start_x = 0;
	int start_y = 1;
	int width = CalcTabWidth();
	int height = parent_rect.Height() - 1 - 50;
	int size = m_tab_button_list.size();
	int last_index = size - 1;
	int current_index = index;

	int new_x = start_x + current_index*gap + current_index* width;
	return CRect(new_x, start_y, new_x + width, start_y + height);
}



CRect WechatTab::CalcNewTabRect()
{
	int size = m_tab_button_list.size();
	int last_index = size - 1;
	int current_index = last_index + 1;
	return CalcTabRect(current_index);
}

CButton* WechatTab::CreateTabItem(CString name)
{
	CButton* p = new CButton();
	CRect parent_rect;
	this->GetWindowRect(&parent_rect);
	CRect rect = CalcNewTabRect();

	//TRACE(L"parent:%d x: %d y:%d width:%d heidht:%d\n", parent_rect.Width(), rect.left, rect.top , rect.Width(), rect.Height());
	p->Create(name, 0 , rect, this, IDC_BUTTON_AUTO_TAB);
	//赋值，同时需要重新刷新界面
	if(rect.Width() != m_last_tab_width){
		m_last_tab_width = rect.Width();
		//需要重新刷新界面
		RefreshTabUI();
	}

	return p;
}

void WechatTab::RefreshTabUI()
{
	CString str;
	for(unsigned int i = 0 ; i < m_tab_button_list.size(); i++){
		CRect rect = CalcTabRect(i);
		m_tab_button_list[i].PButton->MoveWindow(rect);
		str.Format(L"微信%d", i + 1);
		m_tab_button_list[i].PButton->SetWindowTextW(str);
	}
}

bool WechatTab::IsCotain(HWND hw)
{
	bool is_find = false;
	for(unsigned int i = 0 ; i < m_tab_button_list.size(); i++){
		//TRACE("LIST raw_wechat:%d hw:%d \r\n", m_tab_button_list[i].WechatHwnd, hw);

		WCHAR text[256] = {0};
		GetClassName(hw,text, 256);
		//OutputDebugString(text);
		if(m_tab_button_list[i].WechatHwnd == hw){
			is_find = true;
		}
	}
	return is_find;
}

HWND WechatTab::FindWechatFromTab()
{
	CPoint pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	//TRACE("鼠标位置rect: %d %d\r\n", pt.x, pt.y);

	for(unsigned int i = 0 ; i < m_tab_button_list.size(); i++){
		CRect rect;
		m_tab_button_list[i].PButton->GetWindowRect(&rect);
		ScreenToClient(&rect);
		if(pt.x >= rect.left && pt.x <= rect.right && pt.y >= rect.top && pt.y <= rect.bottom){
			return m_tab_button_list[i].WechatHwnd;
		}
	}
	return 0;
}

std::vector<WeChatProcessInfo> WechatTab::CheckWechatIsBad()
{
	std::vector<WeChatProcessInfo> info_list;
	for(std::vector<WeChatProcessInfo>::iterator it = m_tab_button_list.begin(); it !=m_tab_button_list.end(); it++){
		if(!::IsWindow(it->WechatHwnd)){
			info_list.push_back(*it);
		}
	}
	return info_list;
}

BEGIN_MESSAGE_MAP(WechatTab, CDialogEx)
	ON_MESSAGE(WM_MAIN_WECHAT_HWND_MSG,OnWeChatHWNDMessage)
	ON_MESSAGE(WM_MAIN_WECHAT_HWND_CLOSE_MSG,OnWeChatHWNDCloseMessage)
	ON_BN_CLICKED(IDC_BUTTON_AUTO_TAB, OnAutoTabButtonClick)
	//ON_MESSAGE(WM_BUTT)
END_MESSAGE_MAP()


// WechatTab 消息处理程序
bool WechatTab::AddItemUI(CString name, HWND hw_chat)
{
	CButton* p = CreateTabItem(name);
	WeChatProcessInfo info;
	info.PButton = p;
	info.WechatHwnd = hw_chat;
	
	m_tab_button_list.push_back(info);
	p->ShowWindow(SW_SHOW);
	return false;
} 

//按钮删除销毁
//同时需要重新排列按钮
void WechatTab::RemoveItemUI(HWND hw_chat)
{
	for(std::vector<WeChatProcessInfo>::iterator it = m_tab_button_list.begin(); it !=m_tab_button_list.end(); it++){
		if(it->WechatHwnd == hw_chat){
			it->PButton->ShowWindow(SW_HIDE);
			it->PButton->CloseWindow();
			it->PButton->DestroyWindow();
			delete it->PButton;	//释放，防止内存泄漏
			m_tab_button_list.erase(it);
			break;
		}
	}

	RefreshTabUI();
}
