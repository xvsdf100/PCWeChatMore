#pragma once

#include <string>
class WeChatProcessInfo
{
public:
	WeChatProcessInfo(void);
	~WeChatProcessInfo(void);

public:
	HWND WechatHwnd;
	std::wstring RawName;
	std::wstring MarkName;
	int ProcessID;
	CButton* PButton;
};

