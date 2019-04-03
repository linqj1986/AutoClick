
#include "stdafx.h"
#include "BrowserExternal.h"
#include "Crc32.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
* CBrowserExternal::CBrowserExternal
* CBrowserExternal::~CBrowserExternal
*
* Parameters (Constructor):
*  pSite           PCSite of the site we're in.
*  pUnkOuter       LPUNKNOWN to which we delegate.
*/ 

DWORD hashstr( const char* nam )
{
	CCrc32 crc32;
	return crc32.GetValueCrc((LPVOID)nam,strlen(nam));
}

CBrowserExternal::CBrowserExternal( void ):mutex(FALSE,TEXT("CBrowserExternal"))
{
	m_cRef = 0;
}

CBrowserExternal::~CBrowserExternal( void )
{
	//ASSERT( m_cRef == 0 );
}


/*
* CBrowserExternal::QueryInterface
* CBrowserExternal::AddRef
* CBrowserExternal::Release
*
* Purpose:
*  IUnknown members for CBrowserExternal object.
*/ 

STDMETHODIMP CBrowserExternal::QueryInterface( REFIID riid, void **ppv )
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


STDMETHODIMP_(ULONG) CBrowserExternal::AddRef(void)
{
	return ++m_cRef;
}

STDMETHODIMP_(ULONG) CBrowserExternal::Release(void)
{
	return --m_cRef;
}


//IDispatch
STDMETHODIMP CBrowserExternal::GetTypeInfoCount(UINT* /*pctinfo*/)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBrowserExternal::GetTypeInfo(
	/* [in] */ UINT /*iTInfo*/,
	/* [in] */ LCID /*lcid*/,
	/* [out] */ ITypeInfo** /*ppTInfo*/)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBrowserExternal::GetIDsOfNames(
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

	mutex.Lock();
	for ( i=0; i < cNames; i++) {
		char szBuf[64] = {0};
        WideCharToMultiByte( CP_ACP , 0 , rgszNames[i] , -1 , szBuf , 64 , NULL , NULL );
        funMap::iterator it = m_funMap.find(hashstr(szBuf) );
		if ( it != m_funMap.end() )
		{
			rgDispId[i] = it->first;
		}else{
			hr = ResultFromScode(DISP_E_UNKNOWNNAME);
			rgDispId[i] = DISPID_UNKNOWN;
		}

	}
	mutex.Unlock();
	return hr;
}

STDMETHODIMP CBrowserExternal::Invoke(
									  /* [in] */ DISPID dispIdMember,
									  /* [in] */ REFIID /*riid*/,
									  /* [in] */ LCID /*lcid*/,
									  /* [in] */ WORD wFlags,
									  /* [out][in] */ DISPPARAMS* pDispParams,
									  /* [out] */ VARIANT* pVarResult,
									  /* [out] */ EXCEPINFO* /*pExcepInfo*/,
									  /* [out] */ UINT* puArgErr)
{
	mutex.Lock();
	funMap::iterator it = m_funMap.find( dispIdMember );
	if ( it != m_funMap.end() )
	{
		if(wFlags & DISPATCH_PROPERTYGET)
		{
			if(pVarResult != NULL)
			{
				VariantInit(pVarResult);
				V_VT(pVarResult)=VT_BOOL;
				V_BOOL(pVarResult) = true;
			}
		}
		if ( wFlags & DISPATCH_METHOD )
		{
			it->second.fun( pDispParams , it->second.dw_context , pVarResult );
		}
	}
mutex.Unlock();
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
// public method

BOOL CBrowserExternal::Init(/*CMainDlg* pMainDlg*/)
{
	//if (NULL == pMainDlg)
	//	return FALSE;
	//m_pMainDlg = pMainDlg;
	return TRUE;
}



BOOL CBrowserExternal::RegisterHandle( LPCSTR lpMethodNam , external_callbackfun fun , DWORD_PTR dw_context ,  DWORD reserve )
{
	mutex.Lock();

	DWORD dwKey = hashstr(lpMethodNam);
	std::pair<funMap::iterator,bool> ret;
	funDelegate funItem;
	funItem.dw_context = dw_context;
	funItem.fun = fun;
	ret = m_funMap.insert( std::make_pair( dwKey , funItem ) );
	ret.first->second.dw_context = dw_context;
	mutex.Unlock();
	return ret.second;
}
