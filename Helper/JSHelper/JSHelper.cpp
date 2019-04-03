#include "StdAfx.h"
#include "JSHelper.h"

// 发布擂台回调
BOOL func_arena_publish_success(DISPPARAMS * params, DWORD_PTR dw_context, VARIANT *pRet)
{
	if (params->cArgs == 3)
	{
	}
	return TRUE;
}

// 信息更新回调
BOOL func_profile_update(DISPPARAMS * params, DWORD_PTR dw_context, VARIANT *pRet)
{
	if (params->cArgs == 5)
	{
	}
	return TRUE;
}

// 头像设置回调
BOOL func_avatar_update(DISPPARAMS * params, DWORD_PTR dw_context, VARIANT *pRet)
{
	if (params->cArgs == 2)
	{
	}
	return TRUE;
}

CJSHelper::CJSHelper()
{
	m_External.RegisterHandle("func_arena_publish_success", func_arena_publish_success, (DWORD_PTR)CMainFrm::GetInstance().GetHWND());
	m_External.RegisterHandle("func_profile_update", func_profile_update, (DWORD_PTR)CMainFrm::GetInstance().GetHWND());
	m_External.RegisterHandle("func_avatar_update", func_avatar_update, (DWORD_PTR)CMainFrm::GetInstance().GetHWND());

}


CJSHelper::~CJSHelper()
{
}
