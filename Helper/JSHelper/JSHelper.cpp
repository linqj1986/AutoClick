#include "StdAfx.h"
#include "JSHelper.h"

// ������̨�ص�
BOOL func_arena_publish_success(DISPPARAMS * params, DWORD_PTR dw_context, VARIANT *pRet)
{
	if (params->cArgs == 3)
	{
	}
	return TRUE;
}

// ��Ϣ���»ص�
BOOL func_profile_update(DISPPARAMS * params, DWORD_PTR dw_context, VARIANT *pRet)
{
	if (params->cArgs == 5)
	{
	}
	return TRUE;
}

// ͷ�����ûص�
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
