// DllValidateDlg.cpp : implementation file
//
#include "stdafx.h"
#include "DllValidate.h"
#include "DllValidateDlg.h"
#include "sdtapi.h"
#include "wincrypt.h"
#include <process.h>

#define SERVER_IP  "www.modernsky.com" //定义了服务器的地址 
#undef SAFE_DELETE
#define SAFE_DELETE(ptr) if (ptr) { delete ptr; ptr = NULL; }

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/*
1.int InitComm(int Port);       						 	初始化串口;
2.int CloseComm();			      						 	关闭串口;
3.int Authenticate();									 	卡认证;
4.int ReadBaseMsg( unsigned char * pMsg, int * len);		读卡信息内容;
5.int ReadBaseInfos( char * Name, char * Gender, char * Folk,
char *BirthDay, char * Code, char * Address,
char *Agency, char * ExpireStart，char* ExpireEnd);      读卡信息内容;
6.int ReadBaseMsgW( unsigned char * pMsg, int * len);    读卡信息内容;
7.int ReadNewAppMsg( unsigned char * pMsg, int * num );  读追加地址;
8.int ReadIINSNDN( char * pMsg );                        读卡体管理号;
9.int  GetSAMIDToStr( char *pcSAMID );                   读模块序列号;
*/
/*
static		HINSTANCE hIDR_Dll = NULL;
typedef int(*pIDR_InitComm)(int);
typedef int(*pIDR_CloseComm)();
typedef int(*pIDR_Authenticate)();
typedef int(*pIDR_ReadBaseInfos)(char * ,char * ,char * ,char * ,char * ,char * ,char * ,char * ,char * );
//typedef int(*pIDR_InitComm)(int);
//typedef int(*pIDR_InitComm)(int);
//typedef int(*pIDR_InitComm)(int);

pIDR_InitComm  IDR_InitComm;
pIDR_CloseComm IDR_CloseComm;
pIDR_Authenticate IDR_Authenticate;
pIDR_ReadBaseInfos IDR_ReadBaseInfos;

int LoadMylib()
{
	//==========================LoadLibrary(Sdtapi.dll)==============================================//
	if(hIDR_Dll == NULL) 
	{
		if((hIDR_Dll = LoadLibrary("Sdtapi.dll")) == NULL) return	0;
		if((IDR_InitComm = (pIDR_InitComm)GetProcAddress(hIDR_Dll,"InitComm")) == NULL ) return 0;
		if((IDR_CloseComm = (pIDR_CloseComm)GetProcAddress(hIDR_Dll,"CloseComm")) == NULL ) return 0;
		if((IDR_Authenticate = (pIDR_Authenticate)GetProcAddress(hIDR_Dll,"Authenticate")) == NULL ) return 0;
		if((IDR_ReadBaseInfos = (pIDR_ReadBaseInfos)GetProcAddress(hIDR_Dll,"ReadBaseInfos")) == NULL ) return 0;
	}  	
	return 1;
}
*/
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

#define WM_READ WM_USER+1000
#define WM_NEW_CUST WM_USER+1002
#define WM_UP_PASS WM_USER+1003
#define WM_VALIDATE WM_USER+1004


/////////////////////////////////////////////////////////////////////////////
// CDllValidateDlg dialog

CDllValidateDlg::CDllValidateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDllValidateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDllValidateDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

int CDllValidateDlg::Thread=TRUE;
int CDllValidateDlg::AID=0;
CString CDllValidateDlg::IDinfo_post="";

void CDllValidateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDllValidateDlg)
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_TREE1, ActivitiesTree);
	DDX_Control(pDX, IDC_LIST2, FuzzyList);
	DDX_Control(pDX, IDC_TAB3, tabs);
	DDX_Control(pDX, IDC_BUTTON5, button);
}

BEGIN_MESSAGE_MAP(CDllValidateDlg, CDialog)
	//{{AFX_MSG_MAP(CDllValidateDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CDllValidateDlg::OnTvnSelchangedTree1)
	ON_BN_CLICKED(IDC_BUTTON1, &CDllValidateDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CDllValidateDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, &CDllValidateDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CDllValidateDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CDllValidateDlg::OnBnClickedButton5)
	ON_WM_NCHITTEST()

	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB3, &CDllValidateDlg::OnTcnSelchangeTab3)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON6, &CDllValidateDlg::OnBnClickedButton6)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDllValidateDlg message handlers

BOOL CDllValidateDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
/*	if(!LoadMylib())
	{
		if(hIDR_Dll != NULL) 
		{
			FreeLibrary(hIDR_Dll);
			hIDR_Dll = NULL;
		}
		MessageBox("DLL ERROR","ERROR");
		return FALSE; 
	}*/
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);
	//m_brush.CreateSolidBrush(RGB(110,122,230));
	m_font.CreatePointFont(200,"微软雅黑");

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
	DWORD dwStyles=GetWindowLong(ActivitiesTree,GWL_STYLE);//获取树控制原风格
	dwStyles|=TVS_SHOWSELALWAYS;
	SetWindowLong(ActivitiesTree,GWL_STYLE,dwStyles);//设置风格
	HTREEITEM hRoot;     // 树的根节点的句柄   
//    HTREEITEM hCataItem; // 可表示任一分类节点的句柄   
//    HTREEITEM hArtItem;  // 可表示任一文章节点的句柄
	// 插入根节点   
    hRoot = ActivitiesTree.InsertItem(_T("所有活动"), 0, 0); 
	ActivitiesTree.SetItemData(hRoot, 0); 

	getActs(hRoot);

	SetDlgItemText(IDC_EDIT1, "未选择");
	FuzzyList.SetExtendedStyle(FuzzyList.GetExtendedStyle()|LVS_EX_FULLROWSELECT);
	GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);

	//初始化TAB CONTROL
	tabs.InsertItem(0,"二代证刷卡信息");
	tabs.InsertItem(1,"手动输入信息");

	readcardDlg.Create(IDD_DIALOG1, &tabs);
	inputDlg.Create(IDD_DIALOG2,&tabs);

	CRect tabRect;
	tabs.GetClientRect(&tabRect);
	tabRect.left += 1;                  
    tabRect.right -= 1;   
    tabRect.top += 25;   
    tabRect.bottom -= 1;   

	readcardDlg.SetWindowPos(NULL,tabRect.left,tabRect.top,tabRect.Width(),tabRect.Height(),SWP_SHOWWINDOW);
	inputDlg.SetWindowPos(NULL,tabRect.left,tabRect.top,tabRect.Width(),tabRect.Height(),SWP_HIDEWINDOW);

//	HANDLE hThread = (HANDLE)
	_beginthread(ReadThreadProc,0,this);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDllValidateDlg::getActs(HTREEITEM root)
{
	CString post_data;
//	char top[10];
//	itoa(ActivitiesTree.GetItemData(root),top,10);
//	AfxMessageBox(top);
//	string str_top=MultiByteToUtf8(top);
//	AfxMessageBox(str_top.data());
	post_data.Format("top='%d'",
		ActivitiesTree.GetItemData(root));
	CString result;
	CString post_page = "test_id_validater/GetActivities.php";
//	AfxMessageBox("post_page:"+post_page+", "+"post_data:"+post_data);
	PostHttpPage(result, post_page, post_data);
	CStringArray strarr;
	int x;
	while(x=result.Find(',')+1)
	{
		strarr.Add(result.Left(x-1));
		result.Delete(0,x);
	};

	for(int i=0;i<strarr.GetSize();i++)
	{
		CString str = strarr.GetAt(i);
		int l = str.Find(':');
		int r = str.GetLength()-1-l;
	    // 在根节点下插入子节点   
		HTREEITEM hCataItem;
		hCataItem = ActivitiesTree.InsertItem(str.Right(r), 1, 1, root, TVI_LAST);   
	    // 为子节点添加附加的编号数据，在鼠标划过该节点时显示   
		ActivitiesTree.SetItemData(hCataItem, atoi(str.Left(l))); 
		getActs(hCataItem);
	}
}


void CDllValidateDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDllValidateDlg::OnPaint() 
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
HCURSOR CDllValidateDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

int CDllValidateDlg::OnReadcard() 
{
	int ret=0;
	int iPort = 1001;
	if(InitComm(iPort))
	{
		if(Authenticate())
		{
			char Gender[4];
			char Folk[11];
			char BirthDay[9];
			char Address[71];
			char Agency[31];
			char ExpireStart[9];
			char ExpireEnd[9];
			char pcSAMID[37];
			char pIINSNDN[17];
			TCHAR *ImgString,*JpgBack,*JpgFront;

			
			ret = ReadBaseInfos(Name, Gender,Folk,BirthDay,Code,Address,Agency,ExpireStart,ExpireEnd);
			if(ret > 0 && ReadIINSNDN(pIINSNDN) && GetSAMIDToStr(pcSAMID))
			{
				readcardDlg.GetDlgItem(IDC_Name)->SetWindowText(Name);
				readcardDlg.GetDlgItem(IDC_Gender)->SetWindowText(Gender);
				readcardDlg.GetDlgItem(IDC_Folk)->SetWindowText(Folk);
				readcardDlg.GetDlgItem(IDC_BirthDay)->SetWindowText(BirthDay);
				readcardDlg.GetDlgItem(IDC_Code)->SetWindowText(Code);
				readcardDlg.GetDlgItem(IDC_Address)->SetWindowText(Address);
				readcardDlg.GetDlgItem(IDC_Agency)->SetWindowText(Agency);
				readcardDlg.GetDlgItem(IDC_ExpireStart)->SetWindowText(ExpireStart);
				readcardDlg.GetDlgItem(IDC_ExpireEnd)->SetWindowText(ExpireEnd);




				//二进制导入图片文件
				char *buffer;  //BUF_SIZE大小自己定义
				DWORD length;

				//照片图片
				FILE * pFile;
				fopen_s(&pFile, "photo.bmp", "rb");
				fseek(pFile,0,SEEK_END);
				long fl = ftell(pFile);
				rewind(pFile);
				buffer = new char[fl];
				int img = fread(buffer, sizeof(char), fl, pFile);
				fclose(pFile);

				CryptBinaryToString((const BYTE *)buffer,img,CRYPT_STRING_BASE64,NULL,&length);
				ImgString = new TCHAR[length+1];
				CryptBinaryToString((const BYTE *)buffer,img,CRYPT_STRING_BASE64,ImgString,&length);

				delete[] buffer;
				stripCR(&ImgString);


				//身份证正面
				fopen_s(&pFile,"1.jpg","rb");
				fseek(pFile,0,SEEK_END);
				fl = ftell(pFile);
				rewind(pFile);
				buffer = new char[fl];
				img = fread(buffer,sizeof(char),fl,pFile);
				fclose(pFile);

				CryptBinaryToString((const BYTE *)buffer,img,CRYPT_STRING_BASE64,NULL,&length);
				JpgFront = new TCHAR[length+1];
				CryptBinaryToString((const BYTE *)buffer,img,CRYPT_STRING_BASE64,JpgFront,&length);
				delete[] buffer;
				stripCR(&JpgFront);

				//身份证反面
				fopen_s(&pFile, "2.jpg","rb");
				fseek(pFile,0,SEEK_END);
				fl = ftell(pFile);
				rewind(pFile);
				buffer = new char[fl];
				img = fread(buffer,sizeof(char),fl,pFile);
				fclose(pFile);
				CryptBinaryToString((const BYTE *)buffer,img,CRYPT_STRING_BASE64,NULL,&length);
				JpgBack = new TCHAR[length+1];
				CryptBinaryToString((const BYTE *)buffer,img,CRYPT_STRING_BASE64,JpgBack,&length);

				delete[] buffer;
				stripCR(&JpgBack);

				//二进制导入图片文件结束

				HBITMAP hBitmap =(HBITMAP)LoadImage(NULL, _T("photo.bmp"), IMAGE_BITMAP,
					0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE);
				readcardDlg.m_picture.ShowWindow(TRUE);
				readcardDlg.m_picture.ModifyStyle(0xF,SS_BITMAP|SS_CENTERIMAGE);
				readcardDlg.m_picture.SetBitmap(hBitmap);

				readcardDlg.GetDlgItem(IDC_SAMID)->SetWindowText(pcSAMID);


				//准备身份证所有信息的字串
				IDinfo_post.Format("name='%s'&cardid='%s'&gender='%d'&folk='%s'&birthday='%s'&addr='%s'&expire='%s'&expirebegin='%s'&agency='%s'&SAMID='%s'&IINSNDN='%s'&face='%s'&cardfront='%s'&cardback='%s'",Name,Code,abs(_stricmp(Gender,"女")),Folk,BirthDay,Address, ExpireEnd, ExpireStart, Agency,pcSAMID,pIINSNDN,ImgString,JpgFront,JpgBack);
				//准备完毕
				delete[] ImgString,JpgBack,JpgFront;
				CloseComm();
				return 1;
			}
			else
			{
				AfxMessageBox("读取身份证信息失败！");
				CloseComm();
				return 0;
			}




		}
		else
		{
			AfxMessageBox("没有找到卡！");
			CloseComm();
			return 0;
		}
	}
	else
	{
		AfxMessageBox("端口初始化失败！");
		CloseComm();
		return 0;
	}




//	return ret;
	
}

void CDllValidateDlg::stripCR(TCHAR **BASE64)
{
	TCHAR *sourceStr = *BASE64, *destinationStr = *BASE64;
	for(unsigned i=0;i<_tcslen(*BASE64);i+=66)
	{
		memcpy(destinationStr,sourceStr,64);
		destinationStr+=64;
		sourceStr+=66;
	}
}

void CDllValidateDlg::newCustomerThread(LPVOID buy)
{
	int b =(int)buy; 
	CString post_data;
	post_data.Format("aid='%d'&buy='%d'&%s",
		AID,b,IDinfo_post); //请求的附加参数
	CString result; //返回的结果
	CString post_page = "test_id_validater/new_cust.php"; //请求的php
	PostHttpPage(result,post_page,post_data);
	char* p = new char[result.GetLength()+1];
	strcpy(p,result);
	extern CDllValidateApp theApp;
	HWND hWnd = theApp.m_pMainWnd ->GetSafeHwnd();
	::PostMessage(hWnd,WM_NEW_CUST,(WPARAM)p,0);
}



void CDllValidateDlg::ValidateThread(LPVOID lParam)
{
	CDllValidateDlg *pDlg = (CDllValidateDlg *)lParam;
	CString post_data;
	post_data.Format("userid='%s'&name='%s'&aid='%d'",pDlg->Code,pDlg->Name,AID); //请求的附加参数
	CString result; //返回的结果
	CString post_page = "test_id_validater/validateid.php"; //请求的php
	PostHttpPage(result,post_page,post_data);
	char* p = new char[result.GetLength()+1];
	strcpy(p,result);
	extern CDllValidateApp theApp;
	HWND hWnd = theApp.m_pMainWnd -> GetSafeHwnd();
	::PostMessage(hWnd,WM_VALIDATE,(WPARAM)p,0);
}
/*
CString CWordChainSAApp::UTF8ToUnicode(char* UTF8)
{
	DWORD dwUnicodeLen;        //转换后Unicode的长度
	TCHAR *pwText;            //保存Unicode的指针
	CString strUnicode;        //返回值
	dwUnicodeLen = MultiByteToWideChar(CP_UTF8,0,UTF8,-1,NULL,0);
	pwText = new TCHAR[dwUnicodeLen];
	if (!pwText)
		return strUnicode;

	MultiByteToWideChar(CP_UTF8,0,UTF8,-1,pwText,dwUnicodeLen);
	strUnicode.Format(_T("%s"),pwText);
	delete []pwText;
	return strUnicode;
}

string CWordChainSAApp::UnicodeToUTF8(const wchar_t *buf)
{
	if ( NULL == buf || 0 == _tcslen(buf) )
	{
		return "";
	}
	int nlen = WideCharToMultiByte( CP_UTF8, 0, buf, -1, NULL, 0, NULL, NULL );
	char *pTempBuf = new char[nlen + 1];
	memset( pTempBuf, 0, nlen + 1 );
	WideCharToMultiByte( CP_UTF8, 0, buf, -1, pTempBuf, nlen, NULL, NULL );

	string strResult( pTempBuf );
	delete []pTempBuf;

	return strResult;
}
*/
BOOL CDllValidateDlg::PostHttpPage(CString &result,CString &PageName,CString &postData) // 请求Http.
{
	CInternetSession session("SighAgent");
	try
	{
		INTERNET_PORT nPort = 80;
		DWORD dwRet = 0;

		/*以下为wininet 网络请求流程*/
		CHttpConnection* pServer = session.GetHttpConnection(SERVER_IP, nPort);
		CHttpFile* pFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_POST,PageName); //打开一个请求
		CString strHeaders = "Content-Type: application/x-www-form-urlencoded"; // 请求头

		//开始发送请求
		
		string data_post = MultiByteToUtf8(postData.GetBuffer(0));
		pFile->SendRequest(strHeaders,(LPVOID)data_post.data(),data_post.size()); //发送请求,注意此为堵塞函数
		pFile->QueryInfoStatusCode(dwRet); //发送请求后,就查询发送的结果

		if (dwRet == HTTP_STATUS_OK) //这个代表请求正确
		{
			CString str;
			while(pFile->ReadString(str)) //开始返回返回结果 
			{
				char *pStr = (char*)str.GetBuffer(str.GetLength()); //取得str对象的原始字符串 
				result = result + str; 
			}
			result = Utf8ToMultiByte(result.GetBuffer(0)).c_str();
		}
		else
		{
			SAFE_DELETE(pFile);
			SAFE_DELETE(pServer);
			return FALSE;
		}

		SAFE_DELETE(pFile);
		SAFE_DELETE(pServer);
	}
	catch (CInternetException* pEx)
	{
		//catch errors from WinInet
		TCHAR pszError[200];
		pEx->GetErrorMessage(pszError, 200);
		OutputDebugString(pszError);
		return FALSE;
	}

	session.Close();
	return TRUE;
}

string CDllValidateDlg::MultiByteToUtf8( const char* pszMultiByte )  
{  
    string strUtf8;  
   
    if( NULL == pszMultiByte )  
    {  
        return strUtf8;  
    }  
   
    int iNumCharacter = 0;  
   
    //convert from MultiByte to WideChar  
    wstring wcsText;  
    iNumCharacter = MultiByteToWideChar( CP_ACP, 0, pszMultiByte, -1, NULL, 0 );  
    wcsText.resize( iNumCharacter + 1 );//addition 1 for '\0'  
   
    MultiByteToWideChar( CP_ACP, 0, pszMultiByte, -1, &wcsText[ 0 ], iNumCharacter );  
   
    //convert from WideChar to utf8  
    iNumCharacter = WideCharToMultiByte( CP_UTF8, 0, &wcsText[ 0 ], -1, NULL, 0, NULL, NULL );  
    strUtf8.resize( iNumCharacter + 1 );  
    WideCharToMultiByte( CP_UTF8, 0, &wcsText[ 0 ], -1, &strUtf8[ 0 ], iNumCharacter, NULL, NULL );  
	DWORD error = GetLastError();
   
    return strUtf8;  
}

string CDllValidateDlg::Utf8ToMultiByte(string pszUtf8)
{
	string strMultiByte;
	
	if("" == pszUtf8)
	{
		return "";
	}
	
	int iNumCharacter = 0;
	
	//convert from utf8 to WideChar
	wstring wcsText;
	iNumCharacter = MultiByteToWideChar(CP_UTF8, 0, &pszUtf8[0], -1, NULL, 0);
	wcsText.resize(iNumCharacter+1);
	MultiByteToWideChar(CP_UTF8, 0, &pszUtf8[0], -1, &wcsText[0], iNumCharacter);
	
	//convert from WideChar to MultiByte
	iNumCharacter = WideCharToMultiByte(CP_ACP, 0, &wcsText[0], -1, NULL,0,NULL,NULL);
	strMultiByte.resize(iNumCharacter + 1);
	WideCharToMultiByte(CP_ACP,0,&wcsText[0],-1,&strMultiByte[0],iNumCharacter,NULL,NULL);
	
	return strMultiByte;
}

void CDllValidateDlg::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	HTREEITEM item = ActivitiesTree.GetSelectedItem();
	AID = ActivitiesTree.GetItemData(item); //获取节点序号
	if(AID)SetDlgItemText(IDC_EDIT1, ActivitiesTree.GetItemText(item));
	else SetDlgItemText(IDC_EDIT1, "未选择");
}


void CDllValidateDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	//验证入场次数
	CString matchName,matchID; 
	int nId; 
	//首先得到点击的位置 
	POSITION pos=FuzzyList.GetFirstSelectedItemPosition(); 
	if(pos==NULL) 
	{ 
		MessageBox("请至少选择一项","模糊匹配",MB_ICONEXCLAMATION); 
		return; 
	} 
	//得到行号，通过POSITION转化 
	nId=(int)FuzzyList.GetNextSelectedItem(pos);
	//得到列中的内容（0表示第一列，同理1,2,3...表示第二，三，四...列） 
	matchID=FuzzyList.GetItemText(nId,0); 
	matchName=FuzzyList.GetItemText(nId,1); 
	updateAndPass(matchName,matchID,1);
}

void CDllValidateDlg::updateAndPass(CString matchName, CString matchID, int ticket)
{
	struct PASSdata *data = new PASSdata;
	data->matchName=matchName;
	data->matchID=matchID;
	data->ticket=ticket;
	Thread=FALSE;
	_beginthread(UPndPASSThread,0,(void *)data);
}

void CDllValidateDlg::UPndPASSThread(LPVOID lParam)
{
	struct PASSdata *data = (struct PASSdata *)lParam;
	CString post_data;
	post_data.Format("matchid='%s'&matchname='%s'&aid='%d'&ticket='%d'&%s",
		data->matchID,data->matchName,AID,data->ticket,IDinfo_post); //请求的附加参数
	delete data;
	CString result; //返回的结果
	CString post_page = "test_id_validater/update_num.php"; //请求的php
	PostHttpPage(result,post_page,post_data);
	char* p = new char[result.GetLength()+1];
	strcpy(p,result);
	extern CDllValidateApp theApp;
	HWND hWnd = theApp.m_pMainWnd ->GetSafeHwnd();
	::PostMessage(hWnd,WM_UP_PASS,(WPARAM)p,0);
	//Thread=TRUE;
}

BOOL CDllValidateDlg::IsNum(CString &str)
{
  int n=str.GetLength();
  for(int i=0;i<n;i++)
    if (str[i]<'0'||str[i]>'9') 
      return FALSE;
  return TRUE;
}

void CDllValidateDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	if(NAME.GetSize()==0&&ID.GetSize()==0)
	{
		OnBnClickedButton4();
		return;
	}
	int BUY = AfxMessageBox("该购票者无“当日购票信息”。是否更正？\r\n\r\n注意！更正前，会先给出“未购票用户”的模糊匹配。\r\n校对该信息不影响购票。", MB_ICONEXCLAMATION|MB_YESNO);
		if(BUY==IDYES)
		{
			int num = 0;
			FuzzyList.DeleteAllItems();

			for(int i=0;i<NAME.GetSize();i++)
			{
				CString str = NAME.GetAt(i);
				//向listview中插入所有模糊匹配姓名
				int nRow = FuzzyList.InsertItem(num++, "");
				FuzzyList.SetItemText(nRow,0,Code);
				FuzzyList.SetItemText(nRow,1,str);
			}
			for(int i=0;i<ID.GetSize();i++)
			{
				CString str = ID.GetAt(i);
				//向listview中插入所有模糊匹配身份证号
				int nRow = FuzzyList.InsertItem(num++, "");
				FuzzyList.SetItemText(nRow,0,str);
				FuzzyList.SetItemText(nRow,1,Name);
			}

			GetDlgItem(IDC_BUTTON3)->ShowWindow(TRUE);
			GetDlgItem(IDC_BUTTON4)->ShowWindow(TRUE);
			GetDlgItem(IDC_BUTTON1)->ShowWindow(FALSE);
			GetDlgItem(IDC_BUTTON2)->ShowWindow(FALSE);
			GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
			
			if(!strcmp(Name,"")&&strcmp(Code,"")&&NAME.GetCount()==1&&ID.GetCount()==0)
			{
				//显示出二代证信息页，向服务器请求数据，并显示在该页上

				readcardDlg.ShowWindow(TRUE);
				inputDlg.ShowWindow(FALSE);
				_beginthread(getInfoThread,0,this);

				FuzzyList.SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
				OnBnClickedButton3();//默认选中仅有一项，并点击“更正该用户/更新购票”
			}

		}
		else if(BUY==IDNO)
		{
			AfxMessageBox("不能通过！");
			GotoEnd();
		}
}


void CDllValidateDlg::OnBnClickedButton3()
{
	// TODO: Add your control notification handler code here
	//验证购票信息
	CString matchName,matchID; 
	int nId; 
	//首先得到点击的位置 
	POSITION pos=FuzzyList.GetFirstSelectedItemPosition(); 
	if(pos==NULL) 
	{ 
		MessageBox("请至少选择一项","模糊匹配",MB_ICONEXCLAMATION); 
		return; 
	} 
	//得到行号，通过POSITION转化 
	nId=(int)FuzzyList.GetNextSelectedItem(pos);
	//得到列中的内容（0表示第一列，同理1,2,3...表示第二，三，四...列） 
	matchID=FuzzyList.GetItemText(nId,0); 
	matchName=FuzzyList.GetItemText(nId,1); 
	updateAndPass(matchName,matchID,0);
	FuzzyList.DeleteAllItems();
}


void CDllValidateDlg::OnBnClickedButton4()
{
	// TODO: Add your control notification handler code here
	//购票时无匹配结果


	int BUY = AfxMessageBox("您的姓名、身份证和购票信息均未查到。是否添加姓名身份证，并更正购票信息？", MB_ICONEXCLAMATION|MB_YESNO);
	switch(BUY){
		case IDYES:
		{
			Thread=FALSE;
			_beginthread(newCustomerThread,0,(void *)1);
			break;
		}
		case IDNO:
		{
			Thread=FALSE;
			_beginthread(newCustomerThread,0,(void *)0);
			break;
		}
		default:AfxMessageBox("消息框的返回值有误");
	}
	GotoEnd();
	



}
void CDllValidateDlg::GotoEnd()
{
	inputDlg.NameInput.SetWindowText("");
	inputDlg.IDInput.SetWindowText("");

	FuzzyList.DeleteAllItems();
	GetDlgItem(IDC_BUTTON1)->ShowWindow(TRUE);
	GetDlgItem(IDC_BUTTON2)->ShowWindow(TRUE);
	GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON3)->ShowWindow(FALSE);
	GetDlgItem(IDC_BUTTON4)->ShowWindow(FALSE);
	readcardDlg.GetDlgItem(IDC_Name)->SetWindowText("");
	readcardDlg.GetDlgItem(IDC_Gender)->SetWindowText("");
	readcardDlg.GetDlgItem(IDC_Folk)->SetWindowText("");
	readcardDlg.GetDlgItem(IDC_BirthDay)->SetWindowText("");
	readcardDlg.GetDlgItem(IDC_Code)->SetWindowText("");
	readcardDlg.GetDlgItem(IDC_Address)->SetWindowText("");
	readcardDlg.GetDlgItem(IDC_Agency)->SetWindowText("");
	readcardDlg.GetDlgItem(IDC_ExpireStart)->SetWindowText("");
	readcardDlg.GetDlgItem(IDC_ExpireEnd)->SetWindowText("");
	readcardDlg.m_picture.ShowWindow(FALSE);
	readcardDlg.GetDlgItem(IDC_SAMID)->SetWindowText("");
	tabs.EnableWindow(TRUE);
	if(tabs.GetCurSel()==0)
	Thread = TRUE;
	//*处：可在GotoEnd()中判断，如果标签在手动输入页而子窗口显示的是二代证信息，则改变显示的子窗口
	if(tabs.GetCurSel()==1)
	{
		readcardDlg.ShowWindow(FALSE);
		inputDlg.ShowWindow(TRUE);
	}
}

void CDllValidateDlg::OnBnClickedButton5()
{
	// TODO: Add your control notification handler code here
	CString buttontext;
	button.GetWindowText(buttontext);
	if(buttontext=="锁定")
	{
		if(!AID){AfxMessageBox("请先选择一个活动/人员类别！");return;}
		else ActivitiesTree.EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON3)->ShowWindow(FALSE);
		GetDlgItem(IDC_BUTTON4)->ShowWindow(FALSE);
		GetDlgItem(IDC_BUTTON1)->ShowWindow(TRUE);
		GetDlgItem(IDC_BUTTON2)->ShowWindow(TRUE);
		button.SetWindowTextA("解锁");
		//Thread=TRUE;
	}
	else if(buttontext=="解锁")
	{
		ActivitiesTree.EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON3)->ShowWindow(FALSE);
		GetDlgItem(IDC_BUTTON4)->ShowWindow(FALSE);
		GetDlgItem(IDC_BUTTON1)->ShowWindow(TRUE);
		GetDlgItem(IDC_BUTTON2)->ShowWindow(TRUE);
		GotoEnd();
		//Thread=FALSE;
		button.SetWindowTextA("锁定");
	}
}

void CDllValidateDlg::ReadThreadProc(LPVOID lParam)
{
	CDllValidateDlg *p=(CDllValidateDlg *)lParam;
	int iPort = 1001;
	while(1)
	{
		if(Thread)
		{
		if(InitComm(iPort))
			Sleep(30);
			if(Authenticate())
			{
				p->PostMessage(WM_READ,0,0);
				Thread = FALSE;
			}
		CloseComm();
		}
	}
}

void CDllValidateDlg::READ()
{
	int ret = OnReadcard();
	if(ret)
	{
		tabs.EnableWindow(FALSE);
		Thread=FALSE;
		_beginthread(ValidateThread,0,this);
	}
	else if(!ret) Thread = TRUE;
}

LRESULT CDllValidateDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message == WM_READ)
	{
		CString lock;
		GetDlgItem(IDC_BUTTON5)->GetWindowText(lock);
		if(lock=="锁定"&&AID!=0)
		{
			HTREEITEM hItem = ActivitiesTree.GetRootItem();
			CString path = getCurActPath(hItem);
			int LOCK = AfxMessageBox("请先锁定活动/人员类别！是否锁定当前选中？\r\n当前类别：\r\n"+path,MB_ICONEXCLAMATION|MB_YESNO);
			if(LOCK == IDYES)
			{
				OnBnClickedButton5();
				READ();
			}
			else if(LOCK==IDNO)
			{
				AfxMessageBox("无法进行验证！");
				Thread = TRUE;
			}
		}
		else if(lock=="锁定"&&AID==0)
		{
			AfxMessageBox("请先选择一个活动/人员类别，并锁定！");
			Thread = TRUE;
		}
		else READ();
	}
	if(message == WM_INPUT_HAND)
	{
		CString lock;
		GetDlgItem(IDC_BUTTON5)->GetWindowText(lock);
		if(lock=="锁定"&&AID!=0)
		{
			HTREEITEM hItem = ActivitiesTree.GetRootItem();
			CString path = getCurActPath(hItem);
			int LOCK = AfxMessageBox("请先锁定活动/人员类别！是否锁定当前选中？\r\n当前类别：\r\n"+path,MB_ICONEXCLAMATION|MB_YESNO);
			if(LOCK == IDYES)
			{
				OnBnClickedButton5();
				inputhand();
			}
			else if(LOCK==IDNO)AfxMessageBox("无法进行验证！");
		}
		else if(lock=="锁定"&&AID==0)
			AfxMessageBox("请先选择一个活动/人员类别，并锁定！");
		else inputhand();
	}
	if(message == WM_NEW_CUST)
	{
		char *p = (char *)wParam;
		CString result;
		result.Format("%s",p);
		delete p;
		if(result=="")
		{
			AfxMessageBox("该用户不允许通过！若用身份证刷卡，则将导入证件信息，但未购票。");
		}
		else if(result=="1")
		{
			AfxMessageBox("已作为新用户添加，并更新购票信息\r\n允许通过！");
		}
		else if(result=="0")
		{
			AfxMessageBox("购票信息添加不成功，请检查new_cust.php更新index_b2a的错误！");
		}
	}
	if(message == WM_VALIDATE)
	{
		char *p = (char *)wParam;
		CString result;
		result.Format("%s",p);
		delete p;
		ValidateAndParse(result);
	}
	if(message == WM_UP_PASS)
	{
		char *p = (char *)wParam;
		CString result;
		result.Format("%s",p);
		delete p;
		if(result=="0"){AfxMessageBox("允许通过");GotoEnd();}
		else{
			int space = result.Find(" ");
			if(space!=-1&&IsNum(result.Left(space)))
			{
				int PASS = AfxMessageBox("进入次数超过上限，是否破例准入？", MB_ICONEXCLAMATION|MB_YESNO);
				if(PASS==IDYES)
				{
					int i = result.Find(" ");
					CString post_data="id='"+result.Left(i)+"'";
					_SYSTEMTIME st;
					GetLocalTime(&st);
					char strtime[11];
					sprintf_s(strtime, "%04d-%02d-%02d", st.wYear, st.wMonth, st.wDay);
					CString limit = result.Right(result.GetLength()-i-1);
					int begin = limit.Find(strtime)+11;
					CString a = limit.Right(limit.GetLength() - begin);
					char* cstr = (LPSTR)(LPCTSTR)a;
					char* suffixstr = new char[limit.GetLength()];
					char* finalstr = new char[limit.GetLength()];
					suffixstr[0] = 0;
					int num;
					sscanf_s(cstr, "%d%s", &num, suffixstr);
					sprintf(finalstr, "%d%s", num+1, suffixstr);
					CString newlimit = limit.Left(begin)+finalstr;
					delete[] suffixstr;
					delete[] finalstr;
					CString post_page = "test_id_validater/add_num.php";
					post_data  +=  "&limit='"+newlimit+"'";
					CString result;
					PostHttpPage(result,post_page,post_data);
					AfxMessageBox("允许通过");
					GotoEnd();
				}
				else if(PASS=IDNO){AfxMessageBox("不能通过！");GotoEnd();}
			}
			else {AfxMessageBox("update_num.php文件的HTTP返回值异常："+result);GotoEnd();}
		}
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

void CDllValidateDlg::inputhand()
{
		CString N,C;
		inputDlg.NameInput.GetWindowText(N);
		inputDlg.IDInput.GetWindowText(C);
		if(N.GetLength()>31||C.GetLength()>19)AfxMessageBox("输入的姓名和身份证号长度有误！");
		else if(N.GetLength()==0&&C.GetLength()==0)AfxMessageBox("请至少输入一项！");
		else
		{
			IDinfo_post.Format("name='%s'&cardid='%s'",N,C);
			strcpy_s(Name,N);
			strcpy_s(Code,C);
			tabs.EnableWindow(FALSE);
			Thread=FALSE;
			_beginthread(ValidateThread,0,this);
		}
}

void CDllValidateDlg::ValidateAndParse(CString result)
{
	//开始按照数据库中有无用户、用户有无购票、购票有无入场超限等分类处理
	if(result=="1"||result=="-1")
	{
		if(tabs.GetCurSel()==1)
		{
			//显示出二代证信息页，向服务器请求数据，并显示在该页上
			readcardDlg.ShowWindow(TRUE);
			inputDlg.ShowWindow(FALSE);
			_beginthread(getInfoThread,0,this);
		}
	}
	if(result=="1")
	{
		updateAndPass(Name,Code,1);
	}
	else if(result=="-1")
	{
		int BUY = AfxMessageBox("您的身份证和姓名下没有当天的购票信息。是否更正购票信息？", MB_ICONEXCLAMATION|MB_YESNO);
		switch(BUY){
			case IDYES:{updateAndPass(Name,Code,0);	break;}
			case IDNO:{	AfxMessageBox("不能通过！");GotoEnd();break;}
			default:	{Thread=TRUE;AfxMessageBox("消息框的返回值有误");}
		}
	}
	else if(result.Left(3)=="ID:"||result.Left(5)=="NAME:"||result=="")
	{
	//更新模糊匹配购票者（窗口中显示），和模糊匹配未购票用户（暂存变量）
		CString NAMEs,IDs;
		int num = result.Find("ID:");
		//独立分出ID
		if(num==-1)//如果没有ID
		{
			IDs="";//赋值空串
			num = result.GetLength();
		}
		else IDs=result.Right(result.GetLength()-num-3);
		//独立分出NAME
		if(result.Find("NAME:")==-1) NAMEs="";//如果没有NAMEs，赋值空串
		else NAMEs=result.Left(num).Right(num-5);


		num = 0;
		FuzzyList.DeleteAllItems();
		NAME.RemoveAll();
		ID.RemoveAll();
		FuzzyList.InsertColumn(0,"身份证号",LVCFMT_LEFT, 120);
		FuzzyList.InsertColumn(1,"姓名",LVCFMT_LEFT, 100);

		if(NAMEs!="")
		{
			//向listview中插入所有购票的模糊匹配姓名，未购票的暂存变量
			while(int x = NAMEs.Find(",")+1)
			{
				if(NAMEs.Left(2)!="0+")
				{
					int nRow = FuzzyList.InsertItem(num++, "");
					FuzzyList.SetItemText(nRow,0,Code);
					FuzzyList.SetItemText(nRow,1,NAMEs.Left(x-1));
				}
				else 
				{
					NAME.Add(NAMEs.Left(x-1).Right(x-3));
				}
				NAMEs.Delete(0,x);
			}
		}
		if(IDs!="")
		{
			//向listview中插入所有购票的模糊匹配身份证号，未购票的暂存变量
			while(int x = IDs.Find(",")+1)
			{
				if(IDs.Left(2)!="0+")
				{
					int nRow = FuzzyList.InsertItem(num++, "");
					FuzzyList.SetItemText(nRow,0,IDs.Left(x-1));
					FuzzyList.SetItemText(nRow,1,Name);
				}
				else 
				{
					ID.Add(IDs.Left(x-1).Right(x-3));
				}
				IDs.Delete(0,x);
			}

		}
	//存储模糊匹配结果结束
		LVFINDINFO info;
		info.flags = LVFI_STRING|LVFI_PARTIAL;
		info.psz = "";
		int listCount = FuzzyList.GetItemCount();
		int numName = NAME.GetCount();
		int numID = ID.GetCount();
		if(FuzzyList.FindItem(&info)==-1)
		{
			OnBnClickedButton2();//如果模糊匹配后列表中无列出项，直接进入下一步，询问是否购票
		}
		else if(listCount==1&&strcmp(Code,"")&&numName==0&&numID==0&&!strcmp(Name,""))//if（手动输入仅身份证&&模糊匹配购票者仅一项&&模糊匹配未购票者0个）
		{
			//显示出二代证信息页，向服务器请求数据，并显示在该页上
			readcardDlg.ShowWindow(TRUE);
			inputDlg.ShowWindow(FALSE);
			_beginthread(getInfoThread,0,this);

			FuzzyList.SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
			OnBnClickedButton1();
		}
		else
		{
			AfxMessageBox("购票者中未找到身份证姓名的准确匹配\r\n请将模糊匹配结果与您的身份证信息对比");
			GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
		}
	}
}

void CDllValidateDlg::getInfoThread(LPVOID lParam)
{
	CDllValidateDlg *p=(CDllValidateDlg *)lParam;
	CString post_data;
	post_data.Format("code=%s",
		p->Code); //请求的附加参数
	CString result; //返回的结果
	CString post_page = "test_id_validater/getInfoByID.php"; //请求的php
	PostHttpPage(result,post_page,post_data);
	int i = 0, n;
	while(i < 10)
	{
		n = result.Find(",")+1;
		p->readcardDlg.GetDlgItem(IDC_Name+i++)->SetWindowText(result.Left(n-1));
		result.Delete(0,n);
	}
	result = result.Left(result.GetLength()-1);//得到头像图片的BASE64编码值
	//开始BASE64解码
	DWORD length;
	CryptStringToBinary(result,0,CRYPT_STRING_BASE64,NULL,&length,NULL,NULL);
	BYTE *face = new BYTE[length+1];
	CryptStringToBinary(result,0,CRYPT_STRING_BASE64,face,&length,NULL,NULL);
	//写文件
	FILE *pFile;
	fopen_s(&pFile, "photo.bmp","wb");
	fwrite(face,1,length,pFile);
	fclose(pFile);
	delete[] face;
	//读入图片，显示到界面
	HBITMAP hBitmap =(HBITMAP)LoadImage(NULL, _T("photo.bmp"), IMAGE_BITMAP,
		0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE);
	p->readcardDlg.m_picture.ShowWindow(TRUE);
	p->readcardDlg.m_picture.ModifyStyle(0xF,SS_BITMAP|SS_CENTERIMAGE);
	p->readcardDlg.m_picture.SetBitmap(hBitmap);
}


void CDllValidateDlg::OnTcnSelchangeTab3(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
    CRect tabRect;    // 标签控件客户区的Rect   
  
    // 获取标签控件客户区Rect，并对其调整，以适合放置标签页   
    tabs.GetClientRect(&tabRect);   
    tabRect.left += 1;   
    tabRect.right -= 1;   
    tabRect.top += 25;   
    tabRect.bottom -= 1;   
  
    switch (tabs.GetCurSel())  
	{
	case 0:
		readcardDlg.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_SHOWWINDOW);
		inputDlg.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_HIDEWINDOW);
		Thread=TRUE;
		break;
	case 1:
		inputDlg.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_SHOWWINDOW);
		readcardDlg.SetWindowPos(NULL, tabRect.left, tabRect.top, tabRect.Width(), tabRect.Height(), SWP_HIDEWINDOW);
		Thread=FALSE;
		break;
	default:
		break;
	}
}


HBRUSH CDllValidateDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here
//	pDC->SetBkColor(RGB(110,122,230));

	if (pWnd->GetDlgCtrlID()==IDC_TITLE)  /*如果为ID号所指的静态文本框*/
	{
		pDC->SelectObject(&m_font);   /*则将绘图设备加入所需的字体。*/
	}
	// TODO:  Return a different brush if the default is not desired
	 return hbr;                        /*这里特意将系统返回的画刷屏蔽*/
	 //return m_brush;                      /*返回自己制定的蓝色画刷*/
}

LRESULT CDllValidateDlg::OnNcHitTest(CPoint point)
{
	//非标题栏客户区拖动的实现
    CRect rc;
	GetClientRect(&rc);
	ClientToScreen(&rc);
	int x=point.x;
	int y=point.y;
	if (x>rc.left && x<rc.right && y>rc.top && y<rc.top+38) {
		return HTCAPTION;
	}


	return CDialog::OnNcHitTest(point);
}


void CDllValidateDlg::OnBnClickedButton6()
{
	// TODO: Add your control notification handler code here
	GotoEnd();
	ActivitiesTree.SelectItem(ActivitiesTree.GetRootItem());
	AID=0;
	SetDlgItemText(IDC_EDIT1, "未选择");
	button.SetWindowTextA("锁定");
	ActivitiesTree.EnableWindow(TRUE);
}

BOOL CDllValidateDlg::PreTranslateMessage(MSG* pMsg)
{
 // TODO: Add your specialized code here and/or call the base class
 if(pMsg->message==WM_KEYDOWN) 
 { 
  switch(pMsg->wParam)  
  {  
  case VK_RETURN:   //屏蔽Enter 
	if(tabs.GetCurSel()==1)inputDlg.OnBnClickedButton1();
	return true;
  case VK_ESCAPE:   //屏蔽Esc  
	return true;  
  } 
 }
 return CDialog::PreTranslateMessage(pMsg);
}

CString CDllValidateDlg::getCurActPath(HTREEITEM hItem)
{
	CString PathString = ActivitiesTree.GetItemText(hItem);
	//当前为该活动，则返回当前文本
	int tempData = ActivitiesTree.GetItemData(hItem);
	if(tempData == AID)return PathString;
	//否则遍历检查子节点
	HTREEITEM hsubItem = ActivitiesTree.GetChildItem(hItem);
	while (hsubItem != NULL)
	{
		CString subPathString = getCurActPath(hsubItem);
		if(subPathString!="")
		{
			PathString += " > "+subPathString;//找到该活动
			return PathString;
		}
		//没有该活动
		hsubItem = ActivitiesTree.GetNextSiblingItem(hsubItem);
	}
	return "";
}