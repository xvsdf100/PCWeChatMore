#pragma once
#include <string>

// WechatTab 界面的数据层

class WechatTabUIData
{
public:
	WechatTabUIData();
	~WechatTabUIData(void);

	void Init(HWND hw);
	void AddItem(std::wstring name);	//增加一个table 子项

private:
	HWND m_UIHwnd;		//界面HWND
	int m_max_width;	//最大宽度
};

