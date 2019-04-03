#ifndef __BROWSEREXTERNAL_H__
#define __BROWSEREXTERNAL_H__

#include <afxmt.H>//同步操作时需要包含的头文件
#include <map>
// -------------------------------------------------------------------------
typedef BOOL (*external_callbackfun)(DISPPARAMS * ,DWORD_PTR dw_context , VARIANT *);

class CBrowserExternal : public IDispatch
{
public:	
	struct funDelegate
	{		
		external_callbackfun fun;
		DWORD_PTR dw_context;
	};
	typedef std::map<DWORD,funDelegate> funMap;

protected:
	ULONG               m_cRef;

public:
	CBrowserExternal(void);
	~CBrowserExternal(void);

	STDMETHODIMP QueryInterface(REFIID, void **);
	STDMETHODIMP_(ULONG) AddRef(void);
	STDMETHODIMP_(ULONG) Release(void);//

	//IDispatch
	STDMETHODIMP GetTypeInfoCount(UINT* pctinfo);
	STDMETHODIMP GetTypeInfo(/* [in] */ UINT iTInfo,
		/* [in] */ LCID lcid,
		/* [out] */ ITypeInfo** ppTInfo);
	STDMETHODIMP GetIDsOfNames(
		/* [in] */ REFIID riid,
		/* [size_is][in] */ LPOLESTR *rgszNames,
		/* [in] */ UINT cNames,
		/* [in] */ LCID lcid,
		/* [size_is][out] */ DISPID *rgDispId);
	STDMETHODIMP Invoke(
		/* [in] */ DISPID dispIdMember,
		/* [in] */ REFIID riid,
		/* [in] */ LCID lcid,
		/* [in] */ WORD wFlags,
		/* [out][in] */ DISPPARAMS  *pDispParams,
		/* [out] */ VARIANT  *pVarResult,
		/* [out] */ EXCEPINFO *pExcepInfo,
		/* [out] */ UINT *puArgErr);
public:
	BOOL Init(/*CMainDlg* pMainDlg*/);
	BOOL RegisterHandle( LPCSTR lpMethodNam , external_callbackfun , DWORD_PTR , DWORD reserve = 0);

protected:
	funMap m_funMap;
	CMutex mutex;  //此时互斥量受到保护
	
};

// -------------------------------------------------------------------------
// $Log: $

#endif /* __BROWSEREXTERNAL_H__ */
