// PixFontTran.h : main header file for the PIXFONTTRAN application
//

#if !defined(AFX_PIXFONTTRAN_H__AB3A8D5C_B6B5_439D_8DC8_92BF389B671B__INCLUDED_)
#define AFX_PIXFONTTRAN_H__AB3A8D5C_B6B5_439D_8DC8_92BF389B671B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CPixFontTranApp:
// See PixFontTran.cpp for the implementation of this class
//

class CPixFontTranApp : public CWinApp
{
public:
	CPixFontTranApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPixFontTranApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CPixFontTranApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PIXFONTTRAN_H__AB3A8D5C_B6B5_439D_8DC8_92BF389B671B__INCLUDED_)
