/*
* idispimp.CPP
* IDispatch for Extending Dynamic HTML Object Model
*
* Copyright (c)1995-1999 Microsoft Corporation, All Rights Reserved
*/ 

#include "stdafx.h"
#include "idispimp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString cszCB_IsOurCustomBrowser = _T( "CB_IsOurCustomBrowser");

#define DISPID_CB_IsOurCustomBrowser        1

CImpIDispatch::CImpIDispatch( void )
{
	m_cRef = 0;
}

CImpIDispatch::~CImpIDispatch( void )
{
	ASSERT( m_cRef == 0 );
}

STDMETHODIMP CImpIDispatch::QueryInterface( REFIID riid, void **ppv )
{
	*ppv = NULL;

	if ( IID_IDispatch == riid )
	{
		*ppv = this;
	}

	if ( NULL != *ppv )
	{
		((LPUNKNOWN)*ppv)->AddRef();
		return NOERROR;
	}

	return E_NOINTERFACE;
}


STDMETHODIMP_(ULONG) CImpIDispatch::AddRef(void)
{
	return ++m_cRef;
}

STDMETHODIMP_(ULONG) CImpIDispatch::Release(void)
{
	return --m_cRef;
}


//IDispatch
STDMETHODIMP CImpIDispatch::GetTypeInfoCount(UINT* /*pctinfo*/)
{
	return E_NOTIMPL;
}

STDMETHODIMP CImpIDispatch::GetTypeInfo(
										/* [in] */ UINT /*iTInfo*/,
										/* [in] */ LCID /*lcid*/,
										/* [out] */ ITypeInfo** /*ppTInfo*/)
{
	return E_NOTIMPL;
}

STDMETHODIMP CImpIDispatch::GetIDsOfNames(
	/* [in] */ REFIID riid,
	/* [size_is][in] */ OLECHAR** rgszNames,
	/* [in] */ UINT cNames,
	/* [in] */ LCID lcid,
	/* [size_is][out] */ DISPID* rgDispId)
{
	HRESULT hr;
	UINT	i;

	// Assume some degree of success
	hr = NOERROR;

	for ( i=0; i < cNames; i++) 
	{
		if( wcscmp( rgszNames[i], L"ShowMessage" ) == 0 )
		{
			rgDispId[i] = DISPID_CB_IsOurCustomBrowser;
		}
		else 
		{
			// One or more are unknown so set the return code accordingly
			hr = ResultFromScode(DISP_E_UNKNOWNNAME);
			rgDispId[i] = DISPID_UNKNOWN;
		}
	}

	return hr;
}

STDMETHODIMP CImpIDispatch::Invoke(
								   /* [in] */ DISPID dispIdMember,
								   /* [in] */ REFIID /*riid*/,
								   /* [in] */ LCID /*lcid*/,
								   /* [in] */ WORD wFlags,
								   /* [out][in] */ DISPPARAMS* pDispParams,
								   /* [out] */ VARIANT* pVarResult,
								   /* [out] */ EXCEPINFO* /*pExcepInfo*/,
								   /* [out] */ UINT* puArgErr)
{
	if ( dispIdMember == DISPID_CB_IsOurCustomBrowser )
	{
		//CVideoRoomPlayDlg* pDlg = (CVideoRoomPlayDlg*) AfxGetMainWnd();
		//if ( pDlg != NULL )
		{
			OutputDebugString( TEXT("Active External") );
		}
	}

	return S_OK;
}
