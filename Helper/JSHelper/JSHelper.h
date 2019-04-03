#pragma once

// JS-VC调用
#include "BrowserExternal.h"
#include "Custsite.h"
#include "../SingletonObj.h"

class CJSHelper
{
	DECLARE_SINGLETON(CJSHelper);
private:
	CJSHelper();
	virtual ~CJSHelper();

public:
	// 注册web扩展接口
	CBrowserExternal m_External;
};

