// PixFontTranDlg.h : header file
//

#if !defined(AFX_PIXFONTTRANDLG_H__FC323783_C336_4E7F_846A_D678C3DAFB99__INCLUDED_)
#define AFX_PIXFONTTRANDLG_H__FC323783_C336_4E7F_846A_D678C3DAFB99__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CPixFontTranDlg dialog
#include "MyDrawFont.h"

class CPixFontTranDlg : public CDialog
{
// Construction
public:
	BOOL GetGridFromHZK16(CString strFont,BYTE buffer[]);	
	BOOL GetGridFromHZK24(CString strFont,BYTE buffer[]);

	BOOL GetGridFormWinSDK(CString strFont,BYTE buffer[],CString strFontName);

	CPixFontTranDlg(CWnd* pParent = NULL);	// standard constructor

	BYTE FontBuffer_16[32];	
	BYTE FontBuffer_24[72];

	USHORT	m_SelectType;//选择生成的方式 0：字模方式 1：Windows字体方式
	BYTE *pBuf;

	CString str_FontName;


// Dialog Data
	//{{AFX_DATA(CPixFontTranDlg)
	enum { IDD = IDD_PIXFONTTRAN_DIALOG };
	CMyDrawFont	m_FontShow;
	CString	m_strFont;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPixFontTranDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CPixFontTranDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButton1();
	afx_msg void OnRadio1();
	afx_msg void OnRadio2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PIXFONTTRANDLG_H__FC323783_C336_4E7F_846A_D678C3DAFB99__INCLUDED_)
