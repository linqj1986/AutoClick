#pragma once

// JS-VC����
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
	// ע��web��չ�ӿ�
	CBrowserExternal m_External;
};

