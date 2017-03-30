// PixFontTranDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PixFontTran.h"
#include "PixFontTranDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPixFontTranDlg dialog

CPixFontTranDlg::CPixFontTranDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPixFontTranDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPixFontTranDlg)
	m_strFont = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPixFontTranDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPixFontTranDlg)
	DDX_Control(pDX, IDC_STATIC_SHOW, m_FontShow);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPixFontTranDlg, CDialog)
	//{{AFX_MSG_MAP(CPixFontTranDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_BN_CLICKED(IDC_RADIO1, OnRadio1)
	ON_BN_CLICKED(IDC_RADIO2, OnRadio2)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPixFontTranDlg message handlers

BOOL CPixFontTranDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	memset(FontBuffer_16,0,32);	
	memset(FontBuffer_24,0,72);
	pBuf = NULL;

	((CButton *)GetDlgItem(IDC_RADIO1))->SetCheck(1);
	((CButton *)GetDlgItem(IDC_RADIO2))->SetCheck(0);
	m_SelectType = 0;

	str_FontName = "宋体";



	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPixFontTranDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPixFontTranDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);


	}
	else
	{
		

		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPixFontTranDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

//生成
void CPixFontTranDlg::OnButton1() 
{
	// TODO: Add your control notification handler code here
	CString strText;
	GetDlgItem(IDC_EDIT1)->GetWindowText(strText);
	if (strText.GetLength()>2)
	{
		AfxMessageBox("是不是多输了东西了啊");
		return;
	}


 //	GetGridFromHZK24(strText,FontBuffer_24);
 //	m_FontShow.SetDrawDate_24(FontBuffer_24);
	if (m_SelectType == 0)
	{
		GetGridFromHZK16(strText,FontBuffer_16);
		m_FontShow.SetDrawDate_16(FontBuffer_16);
	}
	else if (m_SelectType == 1)
	{
		GetGridFormWinSDK(strText,FontBuffer_16,str_FontName);
		m_FontShow.SetDrawDate_16(FontBuffer_16);
	}

}

BOOL CPixFontTranDlg::GetGridFromHZK16(CString strFont,BYTE buffer[])
{
	BYTE AnsiCode[2];   
	BYTE qh,wh;   
	UINT location;   
	//BYTE buffer[32];   
	FILE* fp = NULL;
	
    AnsiCode[0]=strFont.GetAt(0);//将对话框中的字符转换为内码，先读取第一个字节，判断是汉字还是字母或字符   
	if(AnsiCode[0]<160)  //英文字母和字符，转换为全角的字符内码   
	{   
		BYTE temp=AnsiCode[0];   
		AnsiCode[0]=HIBYTE(temp-0x21+0xA3A1); //0xA3A1 是ASCII 码为0X21的字符对应的全角字符的内码，这是一个偏移量的计算   
		AnsiCode[1]=LOBYTE(temp-0x21+0xA3A1);   
	}   
	else    //是汉字，则直接转换   
	{   
		AnsiCode[1]=strFont.GetAt(1);   
	}
	
	qh=AnsiCode[0]-160; //区码和位码   
	wh=AnsiCode[1]-160;
	printf("qh is %d, wh is %d\n", qh, wh);
	TRACE("qh is %x, wh is %x\n", qh, wh);
	location=(94*(qh-1)+(wh-1))*32;  //HZK16 中的索引 32是一个字的字模字节数
	TRACE("location is: %x\n", location);
	
	fp=fopen("modal\\HZK16","rb");   
	if(fp==NULL)   
	{   
		MessageBox("文件打开错误");   
		return FALSE;   
	}   
	else
	{
		fseek(fp,location,SEEK_SET); //定位到指定地方，然后读取   
		fread(buffer,1,16,fp);    
		fseek(fp,location+16,SEEK_SET);    
		fread(buffer+16,1,16,fp); 
		for(int i=0; i<32; i++)
			TRACE("0x%x ", *(buffer + i));
        fclose(fp);  
	}
	return TRUE;
}

BOOL CPixFontTranDlg::GetGridFromHZK24(CString strFont,BYTE buffer[])
{

	return TRUE;
}

BOOL CPixFontTranDlg::GetGridFormWinSDK(CString strFont,BYTE buffer[],CString strFontName)
{
	CFont newfont;
	newfont.CreateFont(16,0,0,0,0,0,0,0,DEFAULT_CHARSET,OUT_CHARACTER_PRECIS,CLIP_CHARACTER_PRECIS,
		DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,strFontName); //根据字体的大小和名称创建一个字体，m_strFontName就是WINDOWS中的字体名称

	CDC* pdc= GetDlgItem(IDC_STATIC_Font_1)->GetDC();
	CFont *poldfont=pdc->SelectObject(&newfont);
	MAT2 mmat2={0,1,0,0,0,0,0,1};
	GLYPHMETRICS glpm;
	DWORD nLen;
	UINT chartemp;
	BYTE temp=0;

	temp=strFont.GetAt(0);
	if(temp>=128)//汉字
	{
		chartemp=((UINT)temp)<<8;
		temp=strFont.GetAt(1);
		chartemp+=temp;
	}
	else //字符
		chartemp=temp;

	nLen=::GetGlyphOutline(pdc->GetSafeHdc(),chartemp,GGO_BITMAP,&glpm,0,NULL,&mmat2);
	//根据MSDN,cbBuffer 传入参数0，则返回值是BUFFER的大小
	if((signed)nLen>0)
	{
		if(this->pBuf!=NULL)
			delete []this->pBuf;
		this->pBuf=new BYTE[nLen];
		int nLentemp;		
		nLentemp=::GetGlyphOutline(pdc->GetSafeHdc(),chartemp,GGO_BITMAP,&glpm,nLen,pBuf,&mmat2);//重新运行，获取BUFFER的数据，pBUF就是字符对应位图的BUFFER指针
		
	}
	
	BYTE rows=(BYTE)nLen/4; //字占的行数
	printf("**** rows: %d\n", rows);
	memset(buffer,0,32);
	BYTE i,offset;
	
	offset=8-rows/2; //从点阵第一行到字的第一行的偏移量
	
	for(i=0;i<rows;i++)
	{
		memcpy(buffer+(offset+i)*2,pBuf+i*4,2); //将得到的字的BUFFER转换为16*16点阵BUFFER
	}
	newfont.DeleteObject();
	ReleaseDC(pdc);
	return TRUE;
}

void CPixFontTranDlg::OnRadio1() 
{
	m_SelectType = 0;
	((CButton *)GetDlgItem(IDC_RADIO1))->SetCheck(1);
	((CButton *)GetDlgItem(IDC_RADIO2))->SetCheck(0);
}

void CPixFontTranDlg::OnRadio2() 
{
	m_SelectType = 1;
	((CButton *)GetDlgItem(IDC_RADIO1))->SetCheck(0);
	((CButton *)GetDlgItem(IDC_RADIO2))->SetCheck(1);

	CFontDialog f_Dlg;
	if (f_Dlg.DoModal() == IDOK)
	{
		str_FontName = f_Dlg.GetFaceName();
	}
	
}
