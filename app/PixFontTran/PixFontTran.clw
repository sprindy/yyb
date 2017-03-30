; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CPixFontTranDlg
LastTemplate=CStatic
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "PixFontTran.h"

ClassCount=4
Class1=CPixFontTranApp
Class2=CPixFontTranDlg
Class3=CAboutDlg

ResourceCount=3
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Class4=CMyDrawFont
Resource3=IDD_PIXFONTTRAN_DIALOG

[CLS:CPixFontTranApp]
Type=0
HeaderFile=PixFontTran.h
ImplementationFile=PixFontTran.cpp
Filter=N

[CLS:CPixFontTranDlg]
Type=0
HeaderFile=PixFontTranDlg.h
ImplementationFile=PixFontTranDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=IDC_STATIC_SHOW

[CLS:CAboutDlg]
Type=0
HeaderFile=PixFontTranDlg.h
ImplementationFile=PixFontTranDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_PIXFONTTRAN_DIALOG]
Type=1
Class=CPixFontTranDlg
ControlCount=7
Control1=IDC_STATIC,static,1342308352
Control2=IDC_EDIT1,edit,1350631552
Control3=IDC_STATIC_SHOW,static,1350566150
Control4=IDC_BUTTON1,button,1342242816
Control5=IDC_STATIC_Font_1,static,1073741828
Control6=IDC_RADIO1,button,1342177289
Control7=IDC_RADIO2,button,1342177289

[CLS:CMyDrawFont]
Type=0
HeaderFile=MyDrawFont.h
ImplementationFile=MyDrawFont.cpp
BaseClass=CStatic
Filter=W
VirtualFilter=WC
LastObject=CMyDrawFont

