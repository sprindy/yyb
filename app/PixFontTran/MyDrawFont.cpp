// MyDrawFont.cpp : implementation file
//

#include "stdafx.h"
#include "PixFontTran.h"
#include "MyDrawFont.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyDrawFont

CMyDrawFont::CMyDrawFont()
{
	memset(m_DrawArray_16,0,32);
	memset(m_DrawArray_24,0,72);
}

CMyDrawFont::~CMyDrawFont()
{
}


BEGIN_MESSAGE_MAP(CMyDrawFont, CStatic)
	//{{AFX_MSG_MAP(CMyDrawFont)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyDrawFont message handlers

void CMyDrawFont::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CBrush newBrush;
	newBrush.CreateSolidBrush(RGB(0x0, 0x64,0x0)); 

	//填充框格
	CBrush fillBrush;
	fillBrush.CreateSolidBrush(RGB(0xFF, 0xFF,0x0)); 
	
	CRect m_DrawPannelRect;
	GetClientRect(m_DrawPannelRect);
	
	dc.SetBkMode(TRANSPARENT);        // 背景透明
	dc.SelectObject(newBrush);
	dc.FillRect(m_DrawPannelRect, &newBrush);   // 填充方块

	CPen m_LinePen(PS_SOLID,1,RGB(0,0,0));

	float preWidth  =0;
	float preHeight =0;
	if (nModalType == 16)
	{
		//绘制网格线
		preWidth = 0.01 * (m_DrawPannelRect.Width() /(16 * 0.01));
		preHeight= 0.01 * (m_DrawPannelRect.Height()/(16 * 0.01));
		
		
		dc.SelectObject(m_LinePen);
		for (int i =0;i<16;i++)
		{
			dc.MoveTo(int(i * preWidth),0);
			dc.LineTo(int(i * preWidth),m_DrawPannelRect.bottom);
		}
		
		for (int j =0;j<16;j++)
		{
			dc.MoveTo(0,int(j * preHeight));
			dc.LineTo(m_DrawPannelRect.right,int(j * preHeight));
		}

		for(int y = 0; y < 16; y++)
			for(int x = 0; x < 16; x++) 		
				if (m_DrawArray_16[y * 2 + x / 8]  & (0x80 >> (x % 8)))
				{
					//准备填充框格
					CRect m_FileRect;
					m_FileRect.left = int(x * preWidth);
					m_FileRect.right= int((x+1) * preWidth);
					m_FileRect.top = int(y * preHeight);
					m_FileRect.bottom = int((y+1) * preHeight);
					dc.FillRect(m_FileRect,&fillBrush);
			}
	}
	else if (nModalType == 24)
	{
		//绘制网格线
		preWidth = 0.01 * (m_DrawPannelRect.Width() /(36 * 0.01));
		preHeight= 0.01 * (m_DrawPannelRect.Height()/(36 * 0.01));
		
		
		dc.SelectObject(m_LinePen);
		for (int i =0;i<36;i++)
		{
			dc.MoveTo(int(i * preWidth),0);
			dc.LineTo(int(i * preWidth),m_DrawPannelRect.bottom);
		}
		
		for (int j =0;j<36;j++)
		{
			dc.MoveTo(0,int(j * preHeight));
			dc.LineTo(m_DrawPannelRect.right,int(j * preHeight));
		}
		
		for(int y = 0; y < 36; y++)
			for(int x = 0; x < 36; x++) 		
				if (m_DrawArray_24[y * 2 + x / 8]  & (0x80 >> (x % 8)))
				{
					//准备填充框格
					CRect m_FileRect;
					m_FileRect.left = int(x * preWidth);
					m_FileRect.right= int((x+1) * preWidth);
					m_FileRect.top = int(y * preHeight);
					m_FileRect.bottom = int((y+1) * preHeight);
					dc.FillRect(m_FileRect,&fillBrush);
			}
	}




	newBrush.DeleteObject();
	m_LinePen.DeleteObject();
}

void CMyDrawFont::SetDrawDate_16(BYTE m_Array[])
{
	nModalType = 16;
	memset(m_DrawArray_16,0,32);
	memcpy(m_DrawArray_16,m_Array,32);
	this->Invalidate();
}
void CMyDrawFont::SetDrawDate_24(BYTE m_Array[])
{
	nModalType = 24;
	memset(m_DrawArray_24,0,72);
	memcpy(m_DrawArray_24,m_Array,72);
	this->Invalidate();
}