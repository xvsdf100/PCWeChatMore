
// WxMoreInstanceDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "WxMoreInstance.h"
#include "WxMoreInstanceDlg.h"
#include "../InjectDll/InjectDll.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BOOL checkMutex();
BOOL checkEvent();
void checkInstance();

HANDLE mutex;
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CWxMoreInstanceDlg 对话框




CWxMoreInstanceDlg::CWxMoreInstanceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWxMoreInstanceDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWxMoreInstanceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CWxMoreInstanceDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CWxMoreInstanceDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_MORE, &CWxMoreInstanceDlg::OnBnClickedBtnMore)
	ON_BN_CLICKED(IDC_BUTTON_ADVANCE, &CWxMoreInstanceDlg::OnBnClickedButtonAdvance)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


// CWxMoreInstanceDlg 消息处理程序

BOOL CWxMoreInstanceDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	checkInstance();		//测试自己的程序去的，跟微信没有任何关系
	m_MessageHandleUI.Init();
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CWxMoreInstanceDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CWxMoreInstanceDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CWxMoreInstanceDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CWxMoreInstanceDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//杀死句柄
	StartHook();	//测试屏蔽
}

BOOL checkMutex(){
	mutex = ::CreateMutex(NULL,FALSE,L"_WeChat_App_Instance_Identity_Mutex_Name1");
	int err = GetLastError();
	if (err == ERROR_ALREADY_EXISTS)
	{
		MessageBox(NULL, _T("already exist"), _T("ERROR"), SW_NORMAL);
		return FALSE;
	}
	return TRUE;
}


void checkInstance(){

	if(!checkMutex()){
		MessageBox(NULL, _T("已经有了单例，将要退出程序"), _T("ERROR"), SW_NORMAL);
		ExitProcess(0);
	}else{
		
		//CloseHandle(mutex);
		//OpenMutex(NULL, FALSE, )
	}
}
void CWxMoreInstanceDlg::OnBnClickedBtnMore()
{
	// TODO: 在此添加控件通知处理程序代码
	ShellExecute(m_hWnd, _T("open"), _T("http://www.xiaoyutang.net/toptic/%e5%be%ae%e4%bf%a1/pc%e5%be%ae%e4%bf%a1%e5%a4%9a%e5%bc%80%e6%9c%80%e6%96%b0%e4%bb%a3%e7%a0%81%e3%80%90%e5%85%bc%e5%ae%b9%e6%80%a7%e9%9d%9e%e5%b8%b8%e9%ab%98%e3%80%91/"), NULL, NULL, SW_SHOWNORMAL);	//测试屏蔽
}

//高级版本，可以tab管理现有微信功能
//点击逻辑：
//1：隐藏主界面对话框
//2：创建或者显示高级tab界面
//3：同时启动hook功能，然后初始化微信hook函数功能
void CWxMoreInstanceDlg::OnBnClickedButtonAdvance()
{
	if(!IsWindow(m_tab_ui.GetSafeHwnd())){
		m_tab_ui.Create(m_tab_ui.IDD, GetDesktopWindow());
		m_MessageHandleUI.SetWechatTabUI(m_tab_ui.GetSafeHwnd());
		m_tab_ui.ShowWindow(SW_SHOW);
	}

	if(!m_tab_ui.IsWindowVisible()){
		m_tab_ui.ShowWindow(SW_SHOW);
	}
}


void CWxMoreInstanceDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	//AfxMessageBox(L"hello world");
	CDialog::OnLButtonDown(nFlags, point);
}
