// DllValidateDlg.cpp : implementation file
//
#include "stdafx.h"
#include "DllValidate.h"
#include "DllValidateDlg.h"
#include "sdtapi.h"
#include "wincrypt.h"
#include <process.h>
#include <set>
#include <windows.h>

#define SERVER_IP  "www.modernsky.com" //�����˷������ĵ�ַ 
#undef SAFE_DELETE
#define SAFE_DELETE(ptr) if (ptr) { delete ptr; ptr = NULL; }

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
/*
1.int InitComm(int Port);       						 	��ʼ������;
2.int CloseComm();			      						 	�رմ���;
3.int Authenticate();									 	����֤;
4.int ReadBaseMsg( unsigned char * pMsg, int * len);		������Ϣ����;
5.int ReadBaseInfos( char * Name, char * Gender, char * Folk,
char *BirthDay, char * Code, char * Address,
char *Agency, char * ExpireStart��char* ExpireEnd);      ������Ϣ����;
6.int ReadBaseMsgW( unsigned char * pMsg, int * len);    ������Ϣ����;
7.int ReadNewAppMsg( unsigned char * pMsg, int * num );  ��׷�ӵ�ַ;
8.int ReadIINSNDN( char * pMsg );                        ����������;
9.int  GetSAMIDToStr( char *pcSAMID );                   ��ģ�����к�;
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
//#define WM_CHANGE_FOCUS WM_USER+1002


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

BOOL CDllValidateDlg::Thread=TRUE;

void CDllValidateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDllValidateDlg)
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_TREE1, ActivitiesTree);
	DDX_Control(pDX, IDC_LIST2, FuzzyList);
	DDX_Control(pDX, IDC_TAB3, tabs);
	DDX_Control(pDX, IDC_BUTTON5, button);
	DDX_Control(pDX, IDC_BUTTON8, bset.b[0]);
	DDX_Control(pDX, IDC_BUTTON7, bset.b[1]);
	DDX_Control(pDX, IDC_BUTTON1, bset.b[2]);
	DDX_Control(pDX, IDC_BUTTON2, bset.b[3]);
	DDX_Control(pDX, IDC_BUTTON3, bset.b[4]);
	DDX_Control(pDX, IDC_BUTTON4, bset.b[5]);
	DDX_Control(pDX, IDC_BUTTON_THIS, bset.b[6]);
	DDX_Control(pDX, IDC_BUTTON_NONE, bset.b[7]);
	DDX_Control(pDX, IDC_BUTTON9, bset.b[8]);
	DDX_Control(pDX, IDC_BUTTON10, bset.b[9]);
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

	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON6, &CDllValidateDlg::OnBnClickedButton6)
	ON_WM_NCDESTROY()
	ON_BN_CLICKED(IDC_BUTTON8, &CDllValidateDlg::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON7, &CDllValidateDlg::OnBnClickedButton7)
	ON_NOTIFY(NM_CLICK, IDC_TREE1, &CDllValidateDlg::OnNMClickTree1)
	ON_BN_CLICKED(IDC_BUTTON_THIS, &CDllValidateDlg::OnBnClickedButtonThis)
	ON_BN_CLICKED(IDC_BUTTON_NONE, &CDllValidateDlg::OnBnClickedButtonNone)
	ON_BN_CLICKED(IDC_BUTTON9, &CDllValidateDlg::OnBnClickedButton9)
	ON_BN_CLICKED(IDC_BUTTON10, &CDllValidateDlg::OnBnClickedButton10)
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
	newCust=0;
//��ʼ�����ݿ⿪ʼ
	string db_host = "localhost";
	string db_user = "root";
	string db_pwd = "root";
	string db_name = "daoru";
	mysql_library_init(0,NULL,NULL);
	mysql_init(&mysql);
	if(! mysql_real_connect(&mysql, db_host.c_str(), db_user.c_str(), db_pwd.c_str(), db_name.c_str(), 3306, NULL, 0) )
	{
		mysql_close(&mysql);
		AfxMessageBox("��ʼ��mysql���ݿ�ʧ�ܣ�");
	}
	
	mysql_query(&mysql,"set names gbk");//���������ַ���
//��ʼ�����ݿ����

	//m_brush.CreateSolidBrush(RGB(110,122,230));
	m_font.CreatePointFont(200,"΢���ź�");

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
	//DWORD dwStyles=GetWindowLong(ActivitiesTree,GWL_STYLE);//��ȡ������ԭ���
	//dwStyles|=TVS_SHOWSELALWAYS;
	//SetWindowLong(ActivitiesTree,GWL_STYLE,dwStyles);//���÷��
	HTREEITEM hRoot;     // ���ĸ��ڵ�ľ��   
	// ������ڵ�   
    hRoot = ActivitiesTree.InsertItem(_T("���л"),1,2, 0, 0); 
	ActivitiesTree.SetItemData(hRoot, 0); 
	getActs(hRoot);

	SetDlgItemText(IDC_EDIT1, "δѡ��");

	FuzzyList.SetExtendedStyle(FuzzyList.GetExtendedStyle()|LVS_EX_FULLROWSELECT);
	FuzzyList.InsertColumn(0,"",LVCFMT_LEFT,1000);

	bset.ModeWait();
	infoNo=IDLE;

	//��ʼ��TAB CONTROL

	readcardDlg.Create(IDD_DIALOG1, &tabs);

	CRect tabRect;
	tabs.GetClientRect(&tabRect);
	tabRect.left += 1;                  
    tabRect.right -= 1;   
    tabRect.top += 1;   
    tabRect.bottom -= 1;   

	readcardDlg.SetWindowPos(NULL,tabRect.left,tabRect.top,tabRect.Width(),tabRect.Height(),SWP_SHOWWINDOW);


	AIDarray.RemoveAll();

//	HANDLE hThread = (HANDLE)
	_beginthread(ReadThreadProc,0,this);

	return TRUE;  // return TRUE  unless you set the focus to a control
}



//WriteAPerson�����������֤��������������������֤��Ϣ�����û���������������Ŀ
//��������ͷ�����ͼƬBASE64����ռ�ڴ�
//ticket=0��ʾû��Ʊ��buy=1����Ҫ��Ʊ��buy=0����Ʊֻ���¡�ticket=1��ʾ����Ʊ����Ҫ��֤����¡�
void CDllValidateDlg::WriteAPerson(CString matchName, CString matchID, BOOL ticket,BOOL buy)
{
	_SYSTEMTIME st;
	GetLocalTime(&st);
	char strtime[11];
	int j = sprintf_s(strtime, "%04d-%02d-%02d", st.wYear, st.wMonth, st.wDay);
	char *str = (char *)calloc(302000,sizeof(char));
	char *query = new char[200];
	sprintf_s(query,200,"SET @update_id := 0");
	mysql_query(&mysql,query);
	int sex = 0;
	CString AID="";
	char numstr[400];
	for(int i=0;i<AIDarray.GetCount();i++)
	{
		if(i!=0)AID+=",";
		_itoa_s(AIDarray.GetAt(i),numstr,10);
		AID += numstr;
	}

	//����Ҫ��ˢ�����ֶ������������
	if(_stricmp("",BirthDay))
	{
		sex = abs(_stricmp(Gender,"Ů"));
		sprintf_s(str,302000,"update index_base set cardtype=1,	sex=%d,	nationality='%s',	birth=%s,	address='%s',	expirebegin=%s, expiration=%s,	issued='%s',face='%s',cardfront='%s',	cardback='%s',	name = '%s',	cardid='%s', id = (SELECT @update_id := id) where	name='%s' and	cardid='%s'",
																sex,	Folk,				BirthDay,	Address,		ExpireStart,	ExpireEnd,		Agency,		ImgString,JpgFront,			JpgBack,		Name,			Code,												matchName,		matchID);
		mysql_query(&mysql,str);//ˢ������ʱ��ȫ��д��
		delete[] ImgString,JpgBack,JpgFront;
	}
	else
	{
		if(_stricmp(Name,"")&&_stricmp(Code,""))
		sprintf_s(query,200,"update index_base set name = '%s',	cardid='%s', id = (SELECT @update_id := id) where	name='%s' and	cardid='%s'",
													Name,			Code,												matchName,		matchID);
		else if(_stricmp(Name,""))
		sprintf_s(query,200,"update index_base set name = '%s', id = (SELECT @update_id := id) where	name='%s' and	cardid='%s'",
													Name,												matchName,		matchID);
		else if(_stricmp(Code,""))
		sprintf_s(query,200,"update index_base set cardid = '%s', id = (SELECT @update_id := id) where	name='%s' and	cardid='%s'",
													Code,												matchName,		matchID);
		else sprintf_s(query,200,"");
		mysql_query(&mysql,query);//�ֶ�����ʱ��ִ����������֮һ
	}
	sprintf_s(query,200,"SELECT @update_id");
	mysql_query(&mysql,query);
	MYSQL_RES *result = mysql_store_result(&mysql);
	MYSQL_ROW id = mysql_fetch_row(result),row;//��ȡ������Ŀ��id����id0��
	strcpy_s(uid,id[0]);

	//���³ɹ������
	if(_stricmp(uid,"0"))
	{
		//����Ʊ�����
		if(ticket)
		{
			FuzzyList.DeleteAllItems();
			FuzzyList.InsertItem(0,"��ѡ��Ҫ��֤�ĸ���µ�Ʊ:");
			sprintf_s(query,200,"select aid from index_b2a where bid=%s and `limit` like '%%%s%%' and FIND_IN_SET(aid,'%s')",uid,strtime,AID);
			mysql_query(&mysql,query);

			result = mysql_store_result(&mysql);
			auto num_ticket = mysql_num_rows(result);
			HTREEITEM root = ActivitiesTree.GetRootItem();
			//AID_no_tckt_array�����洢û��Ʊ�Ļ����Ʊ�Ļ�����ݿ��е�ȡ��˳����뵽listview��
			AID_no_tckt_array.RemoveAll();
			AID_no_tckt_array.Copy(AIDarray);
			for(int i=0;i<num_ticket;i++)
			{
				row = mysql_fetch_row(result);
				int aid=atoi(row[0]);
				for(int j=0;j<AID_no_tckt_array.GetCount();j++)
				{
					if(AID_no_tckt_array.GetAt(j)==aid)
					{
						AID_no_tckt_array.RemoveAt(j);
						break;
					}
				}
				FuzzyList.InsertItem(i+1,getCurActPath(root,aid));
			}
			bset.ModeShow67();

			//���û���������ɹ���Ʊ�ˣ���ڶ�����ťֱ����ʾ����
			if(AID_no_tckt_array.GetCount()==0)
			{
				GetDlgItem(IDC_BUTTON_NONE)->ShowWindow(FALSE);
				GetDlgItem(IDC_BUTTON10)->ShowWindow(TRUE);
			}
		}
		//��Ʊ������Ʊ�����
		else if(!ticket&&buy)
		{
			AID_no_tckt_array.RemoveAll();
			AID_no_tckt_array.Copy(AIDarray);
			OnBnClickedButtonNone();
		}
		else if(!ticket&&!buy)
		{
			FuzzyList.DeleteAllItems();
			FuzzyList.InsertItem(0,"���û�û�й�Ʊ������ͨ�������ṩ����Ϣ��û�����յ�����򣬻����֤��ȫ����Ϣ���Ѿ����£�");
			bset.ModeOK();
			infoNo=INFORM;
		}
	}
	else//���²��ɹ�����Ҫ����������������Ŀ϶�ûƱ��
	{
		if(_stricmp("",BirthDay))
		{
			sex = abs(_stricmp(Gender,"Ů"));
			sprintf_s(str,302000, "insert into index_base (cardtype, name,sex,nationality,birth, address,"
				"cardid, expirebegin, expiration, issued, face, cardfront, cardback)"
				"values ( 1, '%s', %d, '%s',   %s,   '%s',"
				"'%s',   %s,          %s,         '%s',   '%s', '%s',      '%s')",
							 Name,sex,Folk,BirthDay,Address,
				Code,  ExpireStart,  ExpireEnd, Agency, ImgString,JpgFront,JpgBack);
			//const char *error = mysql_error(&mysql);
			delete[] ImgString,JpgBack,JpgFront;
		}
		else if(!ticket&&buy)
		{
			if(_stricmp(Name,"")||_stricmp(Code,""))
			sprintf_s(str,300, "insert into index_base (name,cardid) values ( '%s', '%s')",Name,Code);
			else sprintf_s(str,300,"");
		}
		mysql_query(&mysql, str);
		//const char *error = mysql_error(&mysql);

		if(!ticket&&buy)
		{
			_itoa_s((int)mysql_insert_id(&mysql),uid,200,10);
			AID_no_tckt_array.RemoveAll();
			AID_no_tckt_array.Copy(AIDarray);

			newCust=1;
			OnBnClickedButtonNone();

		}
		else if(!ticket&&!buy)
		{
			if(_stricmp("",BirthDay))
			{
				FuzzyList.DeleteAllItems();
				FuzzyList.InsertItem(0,"���û�������ͨ�����ѵ���֤����Ϣ����δ��Ʊ��");
				bset.ModeOK();
				infoNo=INFORM;
				//AfxMessageBox("���û�������ͨ�����ѵ���֤����Ϣ����δ��Ʊ��");
			}
			else
			{
				FuzzyList.DeleteAllItems();
				FuzzyList.InsertItem(0,"�����ڸ��û�������������ݿ�����κθĶ���������ͨ����");
				bset.ModeOK();
				infoNo=INFORM;
				//AfxMessageBox("�����ڸ��û���������ͨ����");
			}
		}
	}
	free(str);
	delete[] query;
	//int i = mysql_affected_rows(&mysql);

}

void CDllValidateDlg::getActs(HTREEITEM root)
{
	MYSQL_RES *result;
	int top = ActivitiesTree.GetItemData(root);
	time_t rawtime;
	time(&rawtime);
	char *query = new char[200];
	if(top==0)sprintf_s(query,200,"select `id`,`name` from `index_activities` where `top`= %d and `end` > %ld",top,rawtime-2000000);
	else sprintf_s(query,200,"select `id`,`name` from `index_activities` where `top`= %d",top);
	if(mysql_query(&mysql,query))
		AfxMessageBox("�������ݿ����");
	else
	{
		result = mysql_store_result(&mysql);
		MYSQL_ROW row;
		while(row = mysql_fetch_row(result))
		{
		    // �ڸ��ڵ��²����ӽڵ�   
			HTREEITEM hCataItem;
			hCataItem = ActivitiesTree.InsertItem(row[1], 1, 1, root, TVI_LAST);   
		    // Ϊ�ӽڵ���Ӹ��ӵı������   
			ActivitiesTree.SetItemData(hCataItem, atoi(row[0])); 
			getActs(hCataItem);
		}
		mysql_free_result(result);
	}
	delete[] query;
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

int CDllValidateDlg::Readcard() 
{
	int ret=0;
	int iPort = 1001;
	if(!InitComm(iPort))
	{
		AfxMessageBox("�˿ڳ�ʼ��ʧ�ܣ�");
		CloseComm();
		return 0;
	}
	else
	{
		if(!Authenticate())
		{
			AfxMessageBox("û���ҵ�����");
			CloseComm();
			return 0;
		}
		else
		{
			
			ret = ReadBaseInfos(Name, Gender,Folk,BirthDay,Code,Address,Agency,ExpireStart,ExpireEnd);
			if(!ret || !ReadIINSNDN(pIINSNDN) || !GetSAMIDToStr(pcSAMID))
			{
				AfxMessageBox("��ȡ���֤��Ϣʧ�ܣ�");
				CloseComm();
				return 0;
			}
			else
			{
				readcardDlg.GetDlgItem(IDC_Name)->SetWindowText(Name);
				readcardDlg.GetDlgItem(IDC_Gender)->SetWindowText(Gender);
				readcardDlg.GetDlgItem(IDC_BirthDay)->SetWindowText(BirthDay);
				readcardDlg.GetDlgItem(IDC_Code)->SetWindowText(Code);

				//�����Ƶ���ͼƬ�ļ�
				char *buffer;  //BUF_SIZE��С�Լ�����
				DWORD length;

				//��ƬͼƬ
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


				//���֤����
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

				//���֤����
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

				//�����Ƶ���ͼƬ�ļ�����

				HBITMAP hBitmap =(HBITMAP)LoadImage(NULL, _T("photo.bmp"), IMAGE_BITMAP,
					0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE);
				readcardDlg.m_picture.ShowWindow(TRUE);
				readcardDlg.m_picture.ModifyStyle(0xF,SS_BITMAP|SS_CENTERIMAGE);
				readcardDlg.m_picture.SetBitmap(hBitmap);


				CloseComm();
				return 1;
			}




		}
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

int CDllValidateDlg::Validate()
{
	CString AID="";
	char numstr[400];
	for(int i=0;i<AIDarray.GetCount();i++)
	{
		if(i!=0)AID+=",";
		_itoa_s(AIDarray.GetAt(i),numstr,10);
		AID += numstr;
	}
	NAME.RemoveAll();
	ID.RemoveAll();
	NAMEntckt.RemoveAll();
	IDntckt.RemoveAll();

	_SYSTEMTIME st;
	GetLocalTime(&st);
	char strtime[11];
	sprintf_s(strtime, "%04d-%02d-%02d", st.wYear, st.wMonth, st.wDay);

	char *query = new char[200];
	int num;
	MYSQL_RES *result;
	MYSQL_ROW row;

	if(_stricmp(Name,"")&&_stricmp(Code,""))
	{
		sprintf_s(query,200,"select id from index_base where cardid = '%s' and name = '%s'",Code,Name);
		mysql_query(&mysql,query);
		result = mysql_store_result(&mysql);
		num = mysql_num_rows(result);
	}
	else num=0;

	if(num!=0)
	{
		row = mysql_fetch_row(result);
		int uid = atoi(row[0]);
		sprintf_s(query,200,"select id from index_b2a where `limit` like '%%%s%%' and bid = %d and FIND_IN_SET(aid,'%s')",strtime,uid,AID);
		mysql_query(&mysql,query);
		result = mysql_store_result(&mysql);
		if(mysql_num_rows(result))return 1;
		else return -1;
	}
	else
	{
		if(_stricmp(Code,""))
		{
			sprintf_s(query,200,"select id,name from index_base where cardid = '%s'",Code);
			mysql_query(&mysql,query);
			result = mysql_store_result(&mysql);
			num = mysql_num_rows(result);
			MYSQL_RES *rslt;

			for(int i=0;i<num;i++)
			{
				row = mysql_fetch_row(result);
				int uid = atoi(row[0]);
				sprintf_s(query,200,"select * from index_b2a where bid = %d and FIND_IN_SET(aid,'%s') and `limit` like '%%%s%%'",uid,AID,strtime);
				mysql_query(&mysql,query);
				rslt = mysql_store_result(&mysql);
				if(mysql_num_rows(rslt))NAME.Add(row[1]);
				else NAMEntckt.Add(row[1]);
			}
		}
		if(_stricmp(Name,""))
		{
			sprintf_s(query,200,"select id,cardid from index_base where name = '%s'",Name);
			mysql_query(&mysql,query);
			result = mysql_store_result(&mysql);
			num = mysql_num_rows(result);
			MYSQL_RES *rslt;

			for(int i=0;i<num;i++)
			{
				row = mysql_fetch_row(result);
				int uid = atoi(row[0]);
				sprintf_s(query,200,"select * from index_b2a where bid = %d and FIND_IN_SET(aid,'%s') and `limit` like '%%%s%%'",uid,AID,strtime);
				mysql_query(&mysql,query);
				rslt = mysql_store_result(&mysql);
				if(mysql_num_rows(rslt))ID.Add(row[1]);
				else IDntckt.Add(row[1]);
			}
		}
	}

	delete[] query;
	return 0;
}

void CDllValidateDlg::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	HTREEITEM item = ActivitiesTree.GetSelectedItem();
	SetDlgItemText(IDC_EDIT1, ActivitiesTree.GetItemText(item));
}

void CDllValidateDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	//��֤�볡����
	CString matchName,matchID,matchstr; 
	int nId; 
	//���ȵõ������λ��
	POSITION pos=FuzzyList.GetFirstSelectedItemPosition(); 
	if(pos==NULL) 
	{
		CString info = FuzzyList.GetItemText(0,0);
		if(info.Right(14)!="������ѡ��һ��")
		{
			FuzzyList.InsertItem(0,"������ѡ��һ��");
		}
		else
		{
			FuzzyList.DeleteItem(0);
			FuzzyList.InsertItem(0,"��"+info);
		}
		return;
	} 
	//�õ��кţ�ͨ��POSITIONת�� 
	nId=(int)FuzzyList.GetNextSelectedItem(pos);
	//�õ����е����ݣ�0��ʾ��һ�У�ͬ��1,2,3...��ʾ�ڶ���������...�У� 
	matchstr=FuzzyList.GetItemText(nId,0); 
	if(matchstr.Find("		")==-1)
	{
		CString info = FuzzyList.GetItemText(nId,0);
		FuzzyList.DeleteItem(nId);
		FuzzyList.InsertItem(0,"��"+info);
		return; 
	} 
	matchID=matchstr.Left(matchstr.Find("		"));
	matchName=matchstr.Right(matchstr.GetLength()-matchstr.Find("		")-2); 
	char info[100];
	if(_stricmp(pcSAMID,""))
		sprintf_s(info,100,"�Ὣ���֤�е�������Ϣ���ǵ�������%s��֤�ţ�%s��",matchName,matchID);
	else if(!_stricmp(Name,""))
		sprintf_s(info,100,"�Ὣ����������֤�Ÿ��ǵ�������%s��֤�ţ�%s��",matchName,matchID);
	else if(!_stricmp(Code,""))
		sprintf_s(info,100,"�Ὣ��������������ǵ�������%s��֤�ţ�%s��",matchName,matchID);
	else sprintf_s(info,100,"�Ὣ�������������֤�Ÿ��ǵ�������%s��֤�ţ�%s��",matchName,matchID);

	FuzzyList.DeleteAllItems();
	FuzzyList.InsertItem(0,info);
	FuzzyList.InsertItem(1,"���û����й�Ʊ��Ϣ");
	FuzzyList.InsertItem(2,"�Ƿ������");
	Beep(1000,100);
	Beep(1000,100);
	Beep(1000,100);
	infoNo=ASKIFWR;
	bset.ModeOKCancel();
	//WriteAPerson(matchName,matchID,1,0);
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
	if(NAMEntckt.GetSize()==0&&IDntckt.GetSize()==0)
	{
		OnBnClickedButton4();
		return;
	}
	FuzzyList.DeleteAllItems();
	FuzzyList.InsertItem(0,"��Ʊ��Ϣ��׼ȷorģ��ƥ���û���δ�ҵ����Ƿ�������");
	FuzzyList.InsertItem(1,"ע�⣡����ǰ���������ģ��ƥ��ġ�δ��Ʊ�û���������У�ԡ�У�Ը���Ϣ����Ӱ�칺Ʊ��Ϣ����ӡ�");
	infoNo=FUZZYNTCKT;
	bset.ModeOKCancel();
}


void CDllValidateDlg::OnBnClickedButton3()
{
	// TODO: Add your control notification handler code here
	//��֤��Ʊ��Ϣ
	CString matchName,matchID,matchstr; 
	int nId; 
	//���ȵõ������λ�� 
	POSITION pos=FuzzyList.GetFirstSelectedItemPosition(); 
	if(pos==NULL) 
	{
		CString info = FuzzyList.GetItemText(0,0);
		if(info.Right(14)!="������ѡ��һ��")
		{
			FuzzyList.InsertItem(0,"������ѡ��һ��");
		}
		else
		{
			FuzzyList.DeleteItem(0);
			FuzzyList.InsertItem(0,"��"+info);
		}
		return;
	} 
	//�õ��кţ�ͨ��POSITIONת�� 
	nId=(int)FuzzyList.GetNextSelectedItem(pos);
	//�õ����е����ݣ�0��ʾ��һ�У�ͬ��1,2,3...��ʾ�ڶ���������...�У� 
	matchstr=FuzzyList.GetItemText(nId,0); 
	if(matchstr.Find("		")==-1)
	{
		CString info = FuzzyList.GetItemText(nId,0);
		FuzzyList.DeleteItem(nId);
		FuzzyList.InsertItem(0,"��"+info);
		return; 
	} 
	matchID=matchstr.Left(matchstr.Find("		"));
	matchName=matchstr.Right(matchstr.GetLength()-matchstr.Find("		")-2); 
	FuzzyList.DeleteAllItems();
	char info[100];
	if(_stricmp(pcSAMID,""))
		sprintf_s(info,100,"�Ὣ���֤�е�������Ϣ���ǵ�������%s��֤�ţ�%s��",matchName,matchID);
	else if(!_stricmp(Name,""))
		sprintf_s(info,100,"�Ὣ����������֤�Ÿ��ǵ�������%s��֤�ţ�%s��",matchName,matchID);
	else if(!_stricmp(Code,""))
		sprintf_s(info,100,"�Ὣ��������������ǵ�������%s��֤�ţ�%s��",matchName,matchID);
	else sprintf_s(info,100,"�Ὣ�������������֤�Ÿ��ǵ�������%s��֤�ţ�%s��",matchName,matchID);

	FuzzyList.DeleteAllItems();
	FuzzyList.InsertItem(0,info);
	FuzzyList.InsertItem(1,"�������乺Ʊ��Ϣ");
	FuzzyList.InsertItem(2,"�Ƿ������");
	Beep(1000,100);
	Beep(1000,100);
	Beep(1000,100);
	infoNo=ASKIFWR;
	bset.ModeOKCancel();
	//WriteAPerson(matchName,matchID,0,1);
}


void CDllValidateDlg::OnBnClickedButton4()
{
	// TODO: Add your control notification handler code here
	//��Ʊʱ��ƥ����

	FuzzyList.DeleteAllItems();
	FuzzyList.InsertItem(0,"�������������֤�͹�Ʊ��Ϣ��δ�鵽���Ƿ�����������֤����������Ʊ��Ϣ��");
	infoNo=NOCUST;
	bset.ModeOKCancel();
}

void CDllValidateDlg::GotoEnd()
{
	NAME.RemoveAll();
	ID.RemoveAll();

	NAMEntckt.RemoveAll();
	IDntckt.RemoveAll();

	readcardDlg.NameInput.SetWindowText("");
	readcardDlg.IDInput.SetWindowText("");

	FuzzyList.DeleteAllItems();
	bset.ModeWait();
	readcardDlg.GetDlgItem(IDC_Name)->SetWindowText("");
	readcardDlg.GetDlgItem(IDC_Gender)->SetWindowText("");
	readcardDlg.GetDlgItem(IDC_BirthDay)->SetWindowText("");
	readcardDlg.GetDlgItem(IDC_Code)->SetWindowText("");
	readcardDlg.m_picture.ShowWindow(FALSE);
	tabs.EnableWindow(TRUE);
	Thread = TRUE;
}

void CDllValidateDlg::OnBnClickedButton5()
{
	// TODO: Add your control notification handler code here
	CString buttontext;
	button.GetWindowText(buttontext);
	if(buttontext=="����")
	{
		if(infoNo==CANTLOCK)
		{
			FuzzyList.DeleteAllItems();
			bset.ModeWait();
			infoNo=IDLE;
		}
		else if(infoNo==READNLOCK||infoNo==INPUTNLOCK)
		{
			bset.ModeWait();
			FuzzyList.DeleteAllItems();
			Thread=TRUE;
			infoNo=IDLE;
		}
		if(AIDarray.GetCount()==0)
		{
			FuzzyList.DeleteAllItems();
			FuzzyList.InsertItem(0,"����ѡ��/��Ա���");
			bset.ModeOK();
			infoNo=CANTLOCK;
			return;
		}
		else
		{
			int i;
			SetDlgItemText(IDC_EDIT1, "");
			ActivitiesTree.EnableWindow(FALSE);
			for(i=0;i<AIDarray.GetCount();i++)
			{
				HTREEITEM root = ActivitiesTree.GetRootItem();
				CString text;
				GetDlgItemText(IDC_EDIT1,text);
				SetDlgItemText(IDC_EDIT1,text+getCurActPath(root,AIDarray.GetAt(i))+"\r\n");
			}
		}
		bset.ModeWait();
		button.SetWindowTextA("����");
		//Thread=TRUE;
	}
	else if(buttontext=="����")
	{
		ActivitiesTree.EnableWindow(TRUE);
		GotoEnd();
		//Thread=FALSE;
		button.SetWindowTextA("����");
	}
}


void CDllValidateDlg::ReadThreadProc(LPVOID lParam)
{
	CDllValidateDlg *p=(CDllValidateDlg *)lParam;
	int iPort = 1001;
	while(1)
	{
		Sleep(30);
		if(Thread)
		{
		if(InitComm(iPort))
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
	readcardDlg.SetDlgItemText(IDC_EDIT1,"");
	readcardDlg.SetDlgItemText(IDC_EDIT2,"");
	int ret = Readcard();
	if(ret)
	{
		tabs.EnableWindow(FALSE);
		ParseValidation(Validate());
	}
	else if(!ret) Thread = TRUE;
}

LRESULT CDllValidateDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if(message == WM_READ)
	{
		CString lock;
		GetDlgItem(IDC_BUTTON5)->GetWindowText(lock);
		if(lock=="����"&&AIDarray.GetCount()!=0)
		{
			HTREEITEM hItem = ActivitiesTree.GetRootItem();
			CStringArray path;
			path.RemoveAll();
			for(int i=0;i<AIDarray.GetCount();i++)
				path.Add(getCurActPath(hItem,AIDarray.GetAt(i)));
			FuzzyList.DeleteAllItems();
			FuzzyList.InsertItem(0,"���������/��Ա����Ƿ�������ǰѡ�У�");
			FuzzyList.InsertItem(1,"��ǰ���");
			for(int i=0;i<path.GetCount();i++)
				FuzzyList.InsertItem(2+i,path.GetAt(i));
			infoNo=READNLOCK;
			bset.ModeOKCancel();
		}
		else if(lock=="����"&&AIDarray.GetCount()==0)
		{
			FuzzyList.DeleteAllItems();
			FuzzyList.InsertItem(0,"����ѡ��/��Ա��𣬲�������");
			infoNo=READNLOCK;
			bset.ModeOK();
		}
		else READ();
	}
	//if(message == WM_CHANGE_FOCUS)
	//{
	//	bset.b[0].SetFocus();
	//}
	if(message == WM_INPUT_HAND)
	{
		//::SendMessage(GetDlgItem(IDC_TAB3)->m_hWnd,WM_KILLFOCUS,-1,0);
		//SendDlgItemMessage(IDC_TAB3,WM_KILLFOCUS,0,0);
		//readcardDlg.s
		bset.b[0].SetFocus();
		//SendMessage(WM_CHANGE_FOCUS);
		CString lock;
		GetDlgItem(IDC_BUTTON5)->GetWindowText(lock);
		if(lock=="����"&&AIDarray.GetCount()!=0)
		{
			HTREEITEM hItem = ActivitiesTree.GetRootItem();
			CStringArray path;
			path.RemoveAll();
			for(int i=0;i<AIDarray.GetCount();i++)
				path.Add(getCurActPath(hItem,AIDarray.GetAt(i)));
			FuzzyList.DeleteAllItems();
			FuzzyList.InsertItem(0,"���������/��Ա����Ƿ�������ǰѡ�У�");
			FuzzyList.InsertItem(1,"��ǰ���");
			for(int i=0;i<path.GetCount();i++)
				FuzzyList.InsertItem(2+i,path.GetAt(i));
			infoNo=INPUTNLOCK;
			bset.ModeOKCancel();
		}
		else if(lock=="����"&&AIDarray.GetCount()==0)
		{
			FuzzyList.DeleteAllItems();
			FuzzyList.InsertItem(0,"����ѡ��/��Ա��𣬲�������");
			infoNo=INPUTNLOCK;
			bset.ModeOK();
		}
		else inputhand();
	}
	return CDialog::WindowProc(message, wParam, lParam);
}

void CDllValidateDlg::inputhand()
{
		CString N,C;
		readcardDlg.NameInput.GetWindowText(N);
		readcardDlg.IDInput.GetWindowText(C);
		//��ȥ�����ַ�
		std::string Nm=N,Cd=C;
		N="";
		for(auto &c:Nm)
		{
			if(c!=' '&&c!='\t'&&c!='\n'&&c!='\r')N+=c;	
		}
		C="";
		for(auto &c:Cd)
		{
			if((c>='0'&&c<='9')||c=='x'||c=='X')C+=c;
		}
		//���������ַ�����
		if(N.GetLength()>31||(C.GetLength()!=15&&C.GetLength()!=18&&C.GetLength()!=0))
		{
			FuzzyList.DeleteAllItems();
			FuzzyList.InsertItem(0,"��������������֤�ų�������");
			infoNo=INFORM;
			bset.ModeOK();
			return;
		}
		else if(N.GetLength()==0&&C.GetLength()==0)
		{
			FuzzyList.DeleteAllItems();
			FuzzyList.InsertItem(0,"����������һ�");
			infoNo=INFORM;
			bset.ModeOK();
			return;
		}
		if(C.GetLength()==15)
		{
			Cd=C.Left(6)+"19"+C.Right(9)+"0";
			int W[]={7,9,10,5,8,4,2,1,6,3,7,9,10,5,8,4,2,0};
			int Sum=0;
			for(int i=0;i<18;++i)
				Sum+=atoi(""+Cd[i])*W[i];
			Sum=Sum%11;
			char vcode[] = {'1','0','x','9','8','7','6','5','4','3','2'};
			Cd[17]=vcode[Sum];
			C="";
			for(auto &c:Cd)C+=c;
		}
		if(C.GetLength()==18)
		{
			int Area=atoi(C.Left(2)),Year=atoi(C.Right(12).Left(4)),Month=atoi(C.Right(8).Left(2)),Day=atoi(C.Right(6).Left(2));
			Cd=C;
			int W[]={7,9,10,5,8,4,2,1,6,3,7,9,10,5,8,4,2,0};
			int Sum=0;
			for(int i=0;i<18;++i)
				Sum+=(Cd[i]-'0')*W[i];
			Sum=Sum%11;
			char vcode[] = {'1','0','x','9','8','7','6','5','4','3','2'};
			if(vcode[Sum]!=Cd[17]&&toupper(vcode[Sum])!=Cd[17])
			{
				FuzzyList.DeleteAllItems();
				FuzzyList.InsertItem(0,"���֤��У���벻��ȷ��");
				infoNo=INFORM;
				bset.ModeOK();
				return;
			}
			//��ȡʡ��������������ļ���
			int ars[]={11,12,13,14,15,21,22,23,31,32,33,34,35,36,37,41,42,43,44,45,46,50,51,52,53,54,61,62,63,64,65,71,81,91};
			std::set<int> areas(begin(ars),end(ars));
			if(areas.find(Area)==areas.end())
			{
				FuzzyList.DeleteAllItems();
				FuzzyList.InsertItem(0,"���֤���е����������벻���ڣ�");
				infoNo=INFORM;
				bset.ModeOK();
				return;
			}
			if(Year<1900||Year>2014||Month>12||Month<1||Day>31||Day<1)
			{
				FuzzyList.DeleteAllItems();
				FuzzyList.InsertItem(0,"���֤���е����ڲ����ڣ�");
				infoNo=INFORM;
				bset.ModeOK();
				return;
			}

		}
		strcpy_s(Name,N);
		strcpy_s(Code,C);
		strcpy_s(Gender,"");
		strcpy_s(Folk,"");
		strcpy_s(BirthDay,"");
		strcpy_s(Address,"");
		strcpy_s(Agency,"");
		strcpy_s(ExpireStart,"");
		strcpy_s(ExpireEnd,"");
		strcpy_s(pcSAMID,"");
		strcpy_s(pIINSNDN,"");
		ImgString = new char[2];
		JpgBack = new char[2];
		JpgFront = new char[2];
		strcpy_s(ImgString,1,"");
		strcpy_s(JpgBack,1,"");
		strcpy_s(JpgFront,1,"");
		tabs.EnableWindow(FALSE);
		Thread=FALSE;
		ParseValidation(Validate());
		return;

}

void CDllValidateDlg::ParseValidation(int result)
{
	//��ʼ�������ݿ��������û����û����޹�Ʊ����Ʊ�����볡���޵ȷ��ദ��
	if(result==1||result==-1)
	{
		//��ʾ������֤��Ϣҳ����������������ݣ�����ʾ�ڸ�ҳ��
		if(!_stricmp(BirthDay,""))getInfo(Code,Name);
		if(result==1)
		{
			char info[100];
			if(_stricmp(pcSAMID,""))
				sprintf_s(info,100,"�Ὣ���֤�е�������Ϣ���ǵ�������%s��֤�ţ�%s��",Name,Code);
			else if(!_stricmp(Name,""))
				sprintf_s(info,100,"�Ὣ����������֤�Ÿ��ǵ�������%s��֤�ţ�%s��",Name,Code);
			else if(!_stricmp(Code,""))
				sprintf_s(info,100,"�Ὣ��������������ǵ�������%s��֤�ţ�%s��",Name,Code);
			else sprintf_s(info,100,"�Ὣ�������������֤�Ÿ��ǵ�������%s��֤�ţ�%s��",Name,Code);

			FuzzyList.DeleteAllItems();
			FuzzyList.InsertItem(0,info);
			FuzzyList.InsertItem(1,"���û����й�Ʊ��Ϣ");
			FuzzyList.InsertItem(2,"�Ƿ������");
	Beep(1000,100);
	Beep(1000,100);
	Beep(1000,100);
			infoNo=ASKIFWR;
			bset.ModeOKCancel();
			//WriteAPerson(Name,Code,1,0);
		}
		else if(result==-1)
		{
			FuzzyList.DeleteAllItems();
			FuzzyList.InsertItem(0,"�������֤��������û�е�����κι�Ʊ��Ϣ���Ƿ������Ʊ��Ϣ��");
			infoNo=EXACTNTCKT;
			bset.ModeOKCancel();
		}
	}
	else if(result==0)
	{
		int numFuzzy = NAME.GetCount() + ID.GetCount();
		int numNtckt = IDntckt.GetCount() + NAMEntckt.GetCount();
		if(numFuzzy==0)
		{
			OnBnClickedButton2();//���ģ��ƥ���б������ֱ�ӽ�����һ����ѯ���Ƿ�Ʊ
		}
		else//ģ��ƥ�乺Ʊ������Ϊ������
			if(numNtckt==0&&numFuzzy==1&&NAME.GetCount()==1&&strcmp(Code,"")&&!strcmp(Name,""))//if���ֶ���������֤&&ģ��ƥ�乺Ʊ�߽�һ��&&ģ��ƥ��δ��Ʊ��0����
			{
				//��ʾ������֤��Ϣҳ����������������ݣ�����ʾ�ڸ�ҳ��
				CString matchID,matchName;

				if(NAME.GetCount()==1)
				{
					matchID=Code;
					matchName=NAME.GetAt(0); 
				}
				else if(ID.GetCount()==1)
				{
					matchID=ID.GetAt(0);
					matchName=Name; 
				}
				getInfo(matchID,matchName);
				char info[100];
				sprintf_s(info,100,"�Ὣ����������֤�Ÿ��ǵ�������%s��֤�ţ�%s��",matchName,matchID);

				FuzzyList.DeleteAllItems();
				FuzzyList.InsertItem(0,info);
				FuzzyList.InsertItem(1,"���û����й�Ʊ��Ϣ");
				FuzzyList.InsertItem(2,"�Ƿ������");
	Beep(1000,100);
	Beep(1000,100);
	Beep(1000,100);
				infoNo=ASKIFWR;
				bset.ModeOKCancel();
				//WriteAPerson(matchName,matchID,1,0);
			}
			else
			{
				FuzzyList.DeleteAllItems();
				FuzzyList.InsertItem(0,"��Ʊ����δ�ҵ����֤������׼ȷƥ��");
				FuzzyList.InsertItem(1,"�뽫ģ��ƥ�������������֤��Ϣ�Ա�");
				infoNo=FUZZY;
				bset.ModeOK();
			}
	}
}

void CDllValidateDlg::getInfo(CString matchID,CString matchName)
{
	char query[200];
	sprintf_s(query,200,"select name,sex,birth,cardid,face from index_base where cardid=\"%s\" and name=\"%s\"",matchID,matchName);
	mysql_query(&mysql,query);
	MYSQL_RES *result = mysql_store_result(&mysql);
	MYSQL_ROW row = mysql_fetch_row(result);
	int i = 0;
	CString str;
	while(i < 4)
	{
		str = row[i];
		if(i==1)
			if(str!="")readcardDlg.GetDlgItem(IDC_Name+i++)->SetWindowText(str=="0"?"Ů":"��");
			else i++;
		else readcardDlg.GetDlgItem(IDC_Name+i++)->SetWindowText(str);
	}
	str = row[i];//�õ�ͷ��ͼƬ��BASE64����ֵ
	//��ʼBASE64����
	DWORD length;
	CryptStringToBinary(str,0,CRYPT_STRING_BASE64,NULL,&length,NULL,NULL);
	BYTE *face = new BYTE[length+1];
	CryptStringToBinary(str,0,CRYPT_STRING_BASE64,face,&length,NULL,NULL);
	//д�ļ�
	FILE *pFile;
	fopen_s(&pFile, "photo.bmp","wb");
	fwrite(face,1,length,pFile);
	fclose(pFile);
	delete[] face;
	//����ͼƬ����ʾ������
	HBITMAP hBitmap =(HBITMAP)LoadImage(NULL, _T("photo.bmp"), IMAGE_BITMAP,
		0, 0, LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_LOADFROMFILE);
	readcardDlg.m_picture.ShowWindow(TRUE);
	readcardDlg.m_picture.ModifyStyle(0xF,SS_BITMAP|SS_CENTERIMAGE);
	readcardDlg.m_picture.SetBitmap(hBitmap);
}




HBRUSH CDllValidateDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here
//	pDC->SetBkColor(RGB(110,122,230));

	if (pWnd->GetDlgCtrlID()==IDC_TITLE)  /*���ΪID����ָ�ľ�̬�ı���*/
	{
		pDC->SelectObject(&m_font);   /*�򽫻�ͼ�豸������������塣*/
	}
	// TODO:  Return a different brush if the default is not desired
	 return hbr;                        /*�������⽫ϵͳ���صĻ�ˢ����*/
	 //return m_brush;                      /*�����Լ��ƶ�����ɫ��ˢ*/
}

LRESULT CDllValidateDlg::OnNcHitTest(CPoint point)
{
	//�Ǳ������ͻ����϶���ʵ��
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
	HTREEITEM hItem = ActivitiesTree.GetRootItem();
	ActivitiesTree.SelectItem(hItem);
	AIDarray.RemoveAll();
	SetDlgItemText(IDC_EDIT1, "δѡ��");
	ActivitiesTree.SetCheck(hItem,FALSE);
	SetAllChildrenSame(hItem,FALSE);
	button.SetWindowTextA("����");
	ActivitiesTree.EnableWindow(TRUE);
}

BOOL CDllValidateDlg::PreTranslateMessage(MSG* pMsg)
{
 // TODO: Add your specialized code here and/or call the base class
 if(pMsg->message==WM_KEYDOWN) 
 { 
  switch(pMsg->wParam)  
  {  
  case VK_RETURN:   //����Enter 
	  if(GetDlgItem(IDC_BUTTON8)->IsWindowEnabled() && GetDlgItem(IDC_BUTTON8)->IsWindowVisible())OnBnClickedButton8();
	return true;
  case VK_ESCAPE:   //����Esc  
	return true;  
  } 
 }
 return CDialog::PreTranslateMessage(pMsg);
}

CString CDllValidateDlg::getCurActPath(HTREEITEM hItem,int AID)
{
	CString PathString = ActivitiesTree.GetItemText(hItem);
	//��ǰΪ�û���򷵻ص�ǰ�ı�
	int tempData = ActivitiesTree.GetItemData(hItem);
	if(tempData == AID)return PathString;
	//�����������ӽڵ�
	HTREEITEM hsubItem = ActivitiesTree.GetChildItem(hItem);
	while (hsubItem != NULL)
	{
		CString subPathString = getCurActPath(hsubItem,AID);
		if(subPathString!="")
		{
			PathString += " > "+subPathString;//�ҵ��û
			return PathString;
		}
		//û�иû
		hsubItem = ActivitiesTree.GetNextSiblingItem(hsubItem);
	}
	return "";
}

void CDllValidateDlg::OnNcDestroy()
{
	CDialog::OnNcDestroy();

	// TODO: �ڴ˴������Ϣ����������
 	mysql_close(&mysql);
	mysql_library_end();
}


void CDllValidateDlg::OnBnClickedButton8()
{
	_SYSTEMTIME st;
	GetLocalTime(&st);
	char strtime[11];
	int j = sprintf_s(strtime, "%04d-%02d-%02d", st.wYear, st.wMonth, st.wDay);
	char *str = (char *)calloc(302000,sizeof(char));
	char *query = new char[200];

	// TODO: �ڴ���ӿؼ�֪ͨ����������
	switch(infoNo)
	{
	case INFORM:
		{
		infoNo=IDLE;
		GotoEnd();
		}
		break;
	case MOREIN:
		{
		int begin = limitcopy.Find(strtime)+11;
		CString a = limitcopy.Right(limitcopy.GetLength() - begin);
		char* cstr = (LPSTR)(LPCTSTR)a;
		char* suffixstr = new char[limitcopy.GetLength()];
		char* finalstr = new char[limitcopy.GetLength()];
		suffixstr[0] = 0;
		int num;
		sscanf_s(cstr, "%d%s", &num, suffixstr);
		sprintf_s(finalstr,limitcopy.GetLength(), "%d%s", num+1, suffixstr);
		CString newlimit = limitcopy.Left(begin)+finalstr;
		delete[] suffixstr;
		delete[] finalstr;
		sprintf_s(query,200,"update index_b2a set now=now+1, `limit`='%s' where id=%s",newlimit,tempid);
		mysql_query(&mysql,query);
		FuzzyList.InsertItem(1,"�볡�������볡���޾������ӣ���������ͨ����");
		infoNo=INFORM;
		bset.ModeOK();
		}
		break;
	case FUZZYNTCKT:
		{
		int num = 0;
		FuzzyList.DeleteAllItems();

		for(int i=0;i<NAMEntckt.GetSize();i++)
		{
			CString str = NAMEntckt.GetAt(i);
			//��listview�в�������ģ��ƥ������
			int nRow = FuzzyList.InsertItem(num++, "");
			char temp[80];
			sprintf_s(temp,80,"%s		%s",Code,str);
			FuzzyList.SetItemText(nRow,0,temp);
		}
		for(int i=0;i<IDntckt.GetSize();i++)
		{
			CString str = IDntckt.GetAt(i);
			//��listview�в�������ģ��ƥ�����֤��
			int nRow = FuzzyList.InsertItem(num++, "");
			char temp[80];
			sprintf_s(temp,80,"%s		%s",str,Name);
			FuzzyList.SetItemText(nRow,0,temp);
		}

		bset.ModeShow45();
			
		if(!strcmp(Name,"")&&strcmp(Code,"")&&NAMEntckt.GetCount()==1&&IDntckt.GetCount()==0)
		{
			//��ʾ������֤��Ϣҳ����������������ݣ�����ʾ�ڸ�ҳ��
			CString matchName,matchID,matchstring; 

			FuzzyList.SetItemState(0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
			POSITION pos=FuzzyList.GetFirstSelectedItemPosition(); 
			//�õ��кţ�ͨ��POSITIONת�� 
			int nId=(int)FuzzyList.GetNextSelectedItem(pos);
			//�õ����е����ݣ�0��ʾ��һ�У�ͬ��1,2,3...��ʾ�ڶ���������...�У� 
			matchstring=FuzzyList.GetItemText(nId,0); 
			matchID=matchstring.Left(matchstring.Find("		"));
			matchName=matchstring.Right(matchstring.GetLength()-matchstring.Find("		")-2); 
			getInfo(matchID,matchName);
			OnBnClickedButton3();//Ĭ��ѡ�н���һ���������������û�/���¹�Ʊ��
		}
		}
		break;
	case EXACTNTCKT:
		char info[100];
		if(_stricmp(pcSAMID,""))
			sprintf_s(info,100,"�Ὣ���֤�е�������Ϣ���ǵ�������%s��֤�ţ�%s��",Name,Code);
		else sprintf_s(info,100,"�Ὣ�������������֤�Ÿ��ǵ�������%s��֤�ţ�%s��",Name,Code);

		FuzzyList.DeleteAllItems();
		FuzzyList.InsertItem(0,info);
		FuzzyList.InsertItem(1,"�������乺Ʊ��Ϣ");
		FuzzyList.InsertItem(2,"�Ƿ������");
	Beep(1000,100);
	Beep(1000,100);
	Beep(1000,100);
		infoNo=ASKIFWR;
		bset.ModeOKCancel();
		//WriteAPerson(Name,Code,0,1);
		break;
	case FUZZY:
		FuzzyList.DeleteItem(1);
		FuzzyList.DeleteItem(0);
		bset.ModeShow23();
		{
			int num_c = ID.GetCount();
			for(int i=0;i<num_c;i++)
			{
				FuzzyList.InsertItem(i,ID.GetAt(i)+"		"+(CString)Name);
			}
			int num_n = NAME.GetCount();
			for(int i=0;i<num_n;i++)
			{
				FuzzyList.InsertItem(num_c+i,(CString)Code+"		"+NAME.GetAt(i));
			}
		}
		break;
	case NOCUST:
		FuzzyList.DeleteAllItems();
		if(_stricmp(pcSAMID,""))
			sprintf_s(info,100,"�Ὣ���֤�е�������Ϣ����Ϊ����Ŀ",Name,Code);
		else sprintf_s(info,100,"�Ὣ�������������֤������Ϊ����Ŀ",Name,Code);

		FuzzyList.DeleteAllItems();
		FuzzyList.InsertItem(0,info);
		FuzzyList.InsertItem(1,"�������乺Ʊ��Ϣ");
		FuzzyList.InsertItem(2,"�Ƿ������");
	Beep(1000,100);
	Beep(1000,100);
	Beep(1000,100);
		infoNo=ASKIFWR;
		bset.ModeOKCancel();
		//WriteAPerson(Name,Code,0,1);
		break;
	case CANTLOCK:
		FuzzyList.DeleteAllItems();
		infoNo=IDLE;
		OnBnClickedButton5();
		break;
	case READNLOCK:
		infoNo=IDLE;
		FuzzyList.DeleteAllItems();
		OnBnClickedButton5();
		READ();
		break;
	case INPUTNLOCK:
		infoNo=IDLE;
		FuzzyList.DeleteAllItems();
		OnBnClickedButton5();
		inputhand();
		break;
	case ASKIFWR:
		{
			int ticket,buy;
			CString info=FuzzyList.GetItemText(1,0);
			if(info=="���û����й�Ʊ��Ϣ")
			{
				ticket=1;
				buy=0;
			}
			else if(info=="�������乺Ʊ��Ϣ")
			{
				ticket=0;
				buy=1;
			}
			else if(info=="�������¹�Ʊ��Ϣ")
			{
				ticket=0;
				buy=0;
			}
			info=FuzzyList.GetItemText(0,0);
			if(info.Find("����Ŀ")!=-1)WriteAPerson(Name,Code,ticket,buy);
			else
			{
				int begin = info.Find("������")+6,end = info.Find("��");
				CString matchName = info.Right(info.GetLength()-begin).Left(end-begin);
				begin = info.Find("֤�ţ�")+6;
				end = info.GetLength()-2;
				CString matchID = info.Right(info.GetLength()-begin).Left(end-begin);
				WriteAPerson(matchName,matchID,ticket,buy);
			}
		}
		break;
	}
}


void CDllValidateDlg::OnBnClickedButton7()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	switch(infoNo)
	{
	case ASKIFWR:
		GotoEnd();
		break;
	case MOREIN:
		FuzzyList.InsertItem(1,"����ͨ����");
		infoNo=INFORM;
		bset.ModeOK();
		break;
	case FUZZYNTCKT:
		FuzzyList.DeleteAllItems();
		FuzzyList.InsertItem(0,"����ͨ����");
		infoNo=INFORM;
		bset.ModeOK();
		break;
	case EXACTNTCKT:
		char info[100];
		if(_stricmp(pcSAMID,""))
			sprintf_s(info,100,"�Ὣ���֤�е�������Ϣ���ǵ�������%s��֤�ţ�%s��",Name,Code);
		else sprintf_s(info,100,"�Ὣ�������������֤�Ÿ��ǵ�������%s��֤�ţ�%s��",Name,Code);

		FuzzyList.DeleteAllItems();
		FuzzyList.InsertItem(0,info);
		FuzzyList.InsertItem(1,"�������¹�Ʊ��Ϣ");
		FuzzyList.InsertItem(2,"�Ƿ������");
	Beep(1000,100);
	Beep(1000,100);
	Beep(1000,100);
		infoNo=ASKIFWR;
		bset.ModeOKCancel();
		//WriteAPerson(Name,Code,0,0);
		break;
	case NOCUST:
		FuzzyList.DeleteAllItems();
		if(_stricmp(pcSAMID,""))
		{
			sprintf_s(info,100,"�Ὣ���֤�е�������Ϣ����Ϊ����Ŀ",Name,Code);
			FuzzyList.DeleteAllItems();
			FuzzyList.InsertItem(0,info);
			FuzzyList.InsertItem(1,"�������¹�Ʊ��Ϣ");
			FuzzyList.InsertItem(2,"�Ƿ������");
	Beep(1000,100);
	Beep(1000,100);
	Beep(1000,100);
			infoNo=ASKIFWR;
			bset.ModeOKCancel();
		}
		else WriteAPerson(Name,Code,0,0);
		break;
	case READNLOCK:
	case INPUTNLOCK:
		FuzzyList.DeleteAllItems();
		FuzzyList.InsertItem(0,"������״̬�޷�������֤��");
		infoNo=INFORM;
		bset.ModeOK();
		break;
	}
}

void CDllValidateDlg::SetAllChildrenSame(HTREEITEM hItem,BOOL bChk)
{
	if(ActivitiesTree.ItemHasChildren(hItem))
	{
		HTREEITEM hChildItem=ActivitiesTree.GetChildItem(hItem);
		while(hChildItem!=NULL)
		{
			if(ActivitiesTree.GetCheck(hChildItem)!=bChk)
			{
				if(!ActivitiesTree.ItemHasChildren(hChildItem))
				{
					int data=ActivitiesTree.GetItemData(hChildItem);
					if(bChk)AIDarray.Add(data);
					else
						for(int i=0;i<AIDarray.GetCount();)
						{
							if(AIDarray.GetAt(i)==data)AIDarray.RemoveAt(i);
							else i++;
						}
				}
			}
			ActivitiesTree.SetCheck(hChildItem,bChk);
			SetAllChildrenSame(hChildItem,bChk);
			hChildItem=ActivitiesTree.GetNextItem(hChildItem,TVGN_NEXT);
		}
	}
}


void CDllValidateDlg::UpdateParentItem(HTREEITEM hItem,BOOL bChk)
{
	BOOL bFalse=0;
	HTREEITEM hParentItem=ActivitiesTree.GetParentItem(hItem);
	if(hParentItem==NULL)return;
	if(!bChk)
	{
		ActivitiesTree.SetCheck(hParentItem,0);
		UpdateParentItem(hParentItem,bChk);
		return;
	}
	else
	{
		HTREEITEM hSiblingItem = ActivitiesTree.GetChildItem(hParentItem);
		while(hSiblingItem!=NULL)
		{
			if(hSiblingItem!=hItem&&ActivitiesTree.GetCheck(hSiblingItem)!=bChk)
			{
				bFalse=1;
				break;
			}
			hSiblingItem = ActivitiesTree.GetNextItem(hSiblingItem,TVGN_NEXT);
		}
		if(!bFalse)
		{
			ActivitiesTree.SetCheck(hParentItem);
			UpdateParentItem(hParentItem,bChk);
		}
	}
}


void CDllValidateDlg::OnNMClickTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CPoint cP;
	UINT uF = 0;
	GetCursorPos(&cP);
	ActivitiesTree.ScreenToClient(&cP);
	HTREEITEM hItem = ActivitiesTree.HitTest(cP,&uF);
	HTREEITEM hItemSel = ActivitiesTree.GetSelectedItem();

	if(TVHT_NOWHERE & uF) return;
	if(TVHT_ONITEMSTATEICON & uF)
	{
        BOOL bChk=!ActivitiesTree.GetCheck(hItem);//���������checkbox״̬
		//�����
		SetAllChildrenSame(hItem,bChk);
		if(!ActivitiesTree.ItemHasChildren(hItem))
		{
			int data=ActivitiesTree.GetItemData(hItem);
			if(bChk)AIDarray.Add(data);
			else
				for(int i=0;i<AIDarray.GetCount();)
				{
					if(AIDarray.GetAt(i)==data)AIDarray.RemoveAt(i);
					else i++;
				}
		}
        //�����
		UpdateParentItem(hItem,bChk);
    }
    //������:������TVHT_ONITEMSTATEICON
    //��Ӵ������
	*pResult = 0;
}


void CDllValidateDlg::OnBnClickedButtonThis()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	_SYSTEMTIME st;
	GetLocalTime(&st);
	char strtime[11];
	int j = sprintf_s(strtime, "%04d-%02d-%02d", st.wYear, st.wMonth, st.wDay);
	char *query = new char[200];

	CString AID="";
	char numstr[400];
	for(int i=0;i<AIDarray.GetCount();i++)
	{
		if(i!=0)AID+=",";
		_itoa_s(AIDarray.GetAt(i),numstr,10);
		AID += numstr;
	}


	int nId; 
	//���ȵõ������λ�� 
	POSITION pos=FuzzyList.GetFirstSelectedItemPosition(); 
	if(pos==NULL) 
	{
		CString info = FuzzyList.GetItemText(0,0);
		if(info.Right(14)!="������ѡ��һ��")
		{
			FuzzyList.DeleteItem(0);
			FuzzyList.InsertItem(0,"������ѡ��һ��");
		}
		else
		{
			FuzzyList.DeleteItem(0);
			FuzzyList.InsertItem(0,"��"+info);
		}
		return;
	} 
	//�õ��кţ�ͨ��POSITIONת�� 
	nId=(int)FuzzyList.GetNextSelectedItem(pos);
	if(nId==0) 
	{
		CString info = FuzzyList.GetItemText(0,0);
		if(info.Right(14)!="������ѡ��һ��")
		{
			FuzzyList.DeleteItem(0);
			FuzzyList.InsertItem(0,"������ѡ��һ��");
		}
		else
		{
			FuzzyList.DeleteItem(0);
			FuzzyList.InsertItem(0,"��"+info);
		}
		return;
	} 

	
	sprintf_s(query,200,"select `limit`,`now`,`id` from index_b2a where bid=%s and `limit` like '%%%s%%' and FIND_IN_SET(aid,'%s')",uid,strtime,AID);
	mysql_query(&mysql,query);
	MYSQL_RES *result = mysql_store_result(&mysql);
	mysql_data_seek(result,nId-1);//num_line�ǵ�ǰѡ���е��к�
	MYSQL_ROW row = mysql_fetch_row(result);
	CString limitstr = row[0];
	limitcopy = row[0];
	int now,limit;
	if(row[1]==NULL)now=0;
	else now = atoi(row[1]);//row[1]��nullʱ��ô�죬���Ѿ��Զ���0ֵ�ˣ�������Ҫ�����жϺʹ���
	strcpy_s(tempid,row[2]);

	//�������ŷָ���ȡ����limit
	while(int pos = limitstr.Find(',')+1)
	{
		CString temp = limitstr.Left(pos-1);
		int posc = temp.Find(':');
		if(temp.Left(posc) == strtime)
		{
			limit = atoi(temp.Right(temp.GetLength()-posc-1));
			break;
		}
		limitstr.Delete(0,pos);
	}
	int posc = limitstr.Find(':');
	if(limitstr.Left(posc) == strtime)limit = atoi(limitstr.Right(limitstr.GetLength()-posc-1));
	//��������

	bset.ModeOKCancel();

	if(limit==0||now<limit)
	{
		//now=now+1
		sprintf_s(query,200,"update index_b2a set now=%d where id=%s",now+1,tempid);
		mysql_query(&mysql,query);
		FuzzyList.DeleteAllItems();
		FuzzyList.InsertItem(0,"�ҵ����û���Ʊ��Ϣ���Ѹ����볡������������룡");
		bset.ModeOK();
		infoNo=INFORM;
	}
	else
	{
		FuzzyList.DeleteAllItems();
		FuzzyList.InsertItem(0,"��������������ޣ��Ƿ�����׼�룿");
		bset.ModeOKCancel();
		infoNo=MOREIN;
	}
	delete[] query;
}


void CDllValidateDlg::OnBnClickedButtonNone()
{
	bset.ModeShow89();
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	FuzzyList.DeleteAllItems();
	HTREEITEM hItem = ActivitiesTree.GetRootItem();
	FuzzyList.InsertItem(0,"��Ҫ���������ĸ���Ĺ�Ʊ��Ϣ��");
	for(int i=0;i<AID_no_tckt_array.GetCount();i++)
	{
		FuzzyList.InsertItem(i+1,getCurActPath(hItem,AID_no_tckt_array.GetAt(i)));
	}
}


void CDllValidateDlg::OnBnClickedButton9()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	_SYSTEMTIME st;
	GetLocalTime(&st);
	char strtime[11];
	int j = sprintf_s(strtime, "%04d-%02d-%02d", st.wYear, st.wMonth, st.wDay);
	char *query = new char[200];

	int nId; 
	//���ȵõ������λ�� 
	POSITION pos=FuzzyList.GetFirstSelectedItemPosition(); 
	if(pos==NULL) 
	{
		CString info = FuzzyList.GetItemText(0,0);
		if(info.Right(14)!="������ѡ��һ��")
		{
			FuzzyList.DeleteItem(0);
			FuzzyList.InsertItem(0,"������ѡ��һ��");
		}
		else
		{
			FuzzyList.DeleteItem(0);
			FuzzyList.InsertItem(0,"��"+info);
		}
		return;
	} 
	//�õ��кţ�ͨ��POSITIONת�� 
	nId=(int)FuzzyList.GetNextSelectedItem(pos);
	if(nId==0) 
	{
		CString info = FuzzyList.GetItemText(0,0);
		if(info.Right(14)!="������ѡ��һ��")
		{
			FuzzyList.DeleteItem(0);
			FuzzyList.InsertItem(0,"������ѡ��һ��");
		}
		else
		{
			FuzzyList.DeleteItem(0);
			FuzzyList.InsertItem(0,"��"+info);
		}
		return;
	} 
	bset.ModeOKCancel();

	int AID = AID_no_tckt_array.GetAt(nId-1);

	sprintf_s(query,200,"select `limit` from index_activities where id = %d",AID);
	mysql_query(&mysql,query);
	MYSQL_RES *result = mysql_store_result(&mysql);
	MYSQL_ROW row = mysql_fetch_row(result);
	if(newCust)
	{
			sprintf_s(query,200,"insert into index_b2a (bid, aid, `limit`, now) values (%d, %d, '%s:%s', 1)",uid,AID,strtime,row[0]);
			mysql_query(&mysql,query);
			int num = mysql_affected_rows(&mysql);
			if(num>0)
			{
				FuzzyList.DeleteAllItems();
				FuzzyList.InsertItem(0,"����Ϊ���û���ӣ������¹�Ʊ��Ϣ������ͨ����");
				bset.ModeOK();
				infoNo=INFORM;
				//AfxMessageBox("����Ϊ���û���ӣ������¹�Ʊ��Ϣ\r\n����ͨ����");
			}
			else
			{
				FuzzyList.DeleteAllItems();
				FuzzyList.InsertItem(0,"��Ʊ��Ϣ��Ӳ��ɹ�������������");
				bset.ModeOK();
				infoNo=INFORM;
				//AfxMessageBox("��Ʊ��Ϣ��Ӳ��ɹ�������������");
			}
			newCust=0;
			return;
	}


	sprintf_s(query,200,"update index_b2a set now=1,`limit`=concat(`limit`,',','%s:%s') where bid=%s and aid=%d",strtime,row[0],uid,AID);
	mysql_query(&mysql,query);
	int num = mysql_affected_rows(&mysql);
	if(num==0)
	{
		sprintf_s(query,200,"insert into index_b2a (bid,aid,`limit`,now) values (%s,%d,'%s:%s',1)",uid,AID,strtime,row[0]);
		mysql_query(&mysql,query);
		if(mysql_affected_rows(&mysql))
		{
			FuzzyList.DeleteAllItems();
			FuzzyList.InsertItem(0,"���ڸ��û����¸��¹�Ʊ��Ϣ������ͨ����");
			bset.ModeOK();
			infoNo=INFORM;
		}
		else
		{
			FuzzyList.DeleteAllItems();
			FuzzyList.InsertItem(0,"�����û��Ĺ�Ʊ��Ϣ���²��ɹ�������ͨ��������������");
			bset.ModeOK();
			infoNo=INFORM;
		}
	}
	else if(num>0)
	{
		FuzzyList.DeleteAllItems();
		FuzzyList.InsertItem(0,"���ڸ��û����¸��¹�Ʊ��Ϣ������ͨ����");
		bset.ModeOK();
		infoNo=INFORM;
	}
	delete[] query;
}


void CDllValidateDlg::OnBnClickedButton10()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	bset.ModeOKCancel();
	if(newCust)
	{
			if(_stricmp("",BirthDay))
			{
				FuzzyList.DeleteAllItems();
				FuzzyList.InsertItem(0,"���û�������ͨ�����ѵ���֤����Ϣ����δ��Ʊ��");
				bset.ModeOK();
				infoNo=INFORM;
			}
			else
			{
				FuzzyList.DeleteAllItems();
				FuzzyList.InsertItem(0,"�����ڸ��û�������������ݿ�����κθĶ���������ͨ����");
				bset.ModeOK();
				infoNo=INFORM;
			}
			newCust=0;
			return;
	}

	FuzzyList.DeleteAllItems();
	FuzzyList.InsertItem(0,"���û�û�й�Ʊ������ͨ�������ṩ����Ϣ��û�����յ�����򣬻����֤��ȫ����Ϣ���Ѿ����£�");
	bset.ModeOK();
	infoNo=INFORM;
}
