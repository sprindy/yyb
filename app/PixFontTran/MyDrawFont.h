#if !defined(AFX_MYDRAWFONT_H__43D48A98_12CE_4FAF_BCD4_FBBEAF3F6A6D__INCLUDED_)
#define AFX_MYDRAWFONT_H__43D48A98_12CE_4FAF_BCD4_FBBEAF3F6A6D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyDrawFont.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMyDrawFont window

class CMyDrawFont : public CStatic
{
// Construction
public:
	CMyDrawFont();
	void SetDrawDate_16(BYTE m_Array[]);	
	void SetDrawDate_24(BYTE m_Array[]);


// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyDrawFont)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMyDrawFont();
	BYTE m_DrawArray_16[32];
	BYTE m_DrawArray_24[72];
	int nModalType ;
	// Generated message map functions
protected:
	//{{AFX_MSG(CMyDrawFont)
	afx_msg void OnPaint();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYDRAWFONT_H__43D48A98_12CE_4FAF_BCD4_FBBEAF3F6A6D__INCLUDED_)
