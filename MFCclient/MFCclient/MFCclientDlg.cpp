#define _CRT_SECURE_NO_WARNINGS
// MFCclientDlg.cpp : ���� ����
//

#include <Winsock2.h>
#include "stdafx.h"
#include "MFCclient.h"
#include "MFCclientDlg.h"
#include "afxdialogex.h"

#include <Windows.h>

extern "C" {
#include <libavformat/avformat.h>
}

///> Library Link On Windows System
#pragma comment( lib, "avformat.lib" )
#pragma comment( lib, "avutil.lib" )

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define DIRECTORY 4
#define REGFILE	  8

using namespace std;

const char *bytes[4] = { "B", "KB", "MB", "GB" };

char dName[244] = { 0, };
int fLen=0, fType=0;
Sock	  fileSoc;
Directory	  *dp;

// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCclientDlg ��ȭ ����



CMFCclientDlg::CMFCclientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MFCCLIENT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCclientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST3, mCtrlList);
	DDX_Control(pDX, IDC_EDIT1, fPath);
	DDX_Control(pDX, IDC_PROGRESS1, mProgress);
	DDX_Control(pDX, IDC_BUTTON4, recvBtn);
	DDX_Control(pDX, IDC_BUTTON3, sendBtn);
}

BEGIN_MESSAGE_MAP(CMFCclientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CMFCclientDlg::OnBnClickedOk)
	ON_NOTIFY(NM_CLICK, IDC_LIST3, &OnNMClickList3)
	ON_BN_CLICKED(IDC_BUTTON3, &CMFCclientDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON1, &CMFCclientDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CMFCclientDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON4, &CMFCclientDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CMFCclientDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CMFCclientDlg::OnBnClickedButton6)
END_MESSAGE_MAP()

void CMFCclientDlg::renewList()
{
	for (int i = 0; i < files.size; i++)
	{
		mCtrlList.DeleteItem(i);
		mCtrlList.InsertItem(i, (""));
	}

	for (int i = 0; i < files.size; i++)
	{
		mCtrlList.SetItemText(i, 1, _T(dp[i].fileName));
		printf("%s\n", dp[i].fileName);

		string tmp;
		char st[30];

		if (dp[i].fileType != DIRECTORY)
		{
			for (long long k = 1, j = 0; k < dp[i].fileLen; k *= 1024, j++)
			{
				char a[50] = { 0, }, b[50] = { 0, }, c[50] = { 0, };
				sprintf(a, "%lld", dp[i].fileLen / k);
				snprintf(b, 2, "%d", dp[i].fileLen % k);

				tmp.clear();

				tmp.append(a);
				tmp.append(".");
				tmp.append(b);
				tmp.append(bytes[j]);

				memset(st, 0, 30);
				strncpy(st, tmp.c_str(), tmp.length());
				mCtrlList.SetItemText(i, 2, _T(st));
			}
			mCtrlList.SetItemText(i, 0, _T("�Ϲ� ����"));
		}
		else
			mCtrlList.SetItemText(i, 0, _T("����"));
	}
}

unsigned int __stdcall CMFCclientDlg::recvPacket(void * arg)
{
	CMFCclientDlg* ctx = (CMFCclientDlg*)arg;
	Sock msgSoc = ctx->soc;
	FileControl files = ctx->files;

	Header header;
	
	int a = 1;
	while (1)
	{
		int retValue = msgSoc.receive_nbytes(msgSoc.fd, &header, sizeof(Header));
		if (retValue == 0)
			exit(0);

		int type = ntohl(header.type);
		int len = ntohl(header.data_len);

		switch (type)
		{
		case RQ_FILELIST:
			dp = files.getFileList(len);
			ctx->renewList();
			break;
		case MKDIR:
			AfxMessageBox(_T("������ �����Ͽ����ϴ�."));
			break;
		case MKDIR_FAIL:
			AfxMessageBox(_T("�ߺ��� �̸��Դϴ�."));
			break;
		case RM:
			AfxMessageBox(_T("���������� �����Ǿ����ϴ�."));
			break;
		case RM_FAIL:
			AfxMessageBox(_T("������ �����Ͽ����ϴ�."));
			break;
		}
	}
	return 0;
}

// CMFCclientDlg �޽��� ó����

BOOL CMFCclientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// �� ��ȭ ������ �������� �����մϴ�.  ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	// TODO: ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	char *title[3] = { "����", "���ϸ�", "ũ��" };
	int size[3] = { 100, 400, 150 };
	LV_COLUMN lCol = { 0, };

	lCol.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lCol.fmt = LVCFMT_LEFT;

	mCtrlList.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	
	mProgress.SetRange(0, 100);  //�������� ����
	mProgress.SetStep(1);


	for (int i = 0; i < 3; i++)
	{
		lCol.pszText = title[i];
		lCol.iSubItem = i;
		lCol.cx = size[i];
		mCtrlList.InsertColumn(i, &lCol);
	}

	const char *szFilePath = "test.mp4";

	///> Initialize libavformat and register all the muxers, demuxers and protocols.
	av_register_all();

	///> Do global initialization of network components.
	avformat_network_init();

	int ret;
	AVFormatContext *pFmtCtx = avformat_alloc_context();

	///> Open an input stream and read the header.
	ret = avformat_open_input(&pFmtCtx, szFilePath, NULL, NULL);
	if (ret != 0) {
		char buf[1024] = { 0, };
		av_strerror(ret, buf, sizeof(buf));
		printf("%s\n", buf);
		Sleep(100000);
	}
	av_log(NULL, AV_LOG_INFO, "File [%s] Open Success\n", szFilePath);

	///> Read  of a media file to get stream information
	ret = avformat_find_stream_info(pFmtCtx, NULL);
	if (ret < 0) {
		av_log(NULL, AV_LOG_ERROR, "Fail to get Stream Information\n");
		exit(-1);
	}
	av_log(NULL, AV_LOG_INFO, "Get Stream Information Success\n");

	///> Close an opened input AVFormatContext. 
	avformat_close_input(&pFmtCtx);

	///> Undo the initialization done by avformat_network_init.
	avformat_network_deinit();
	
	soc = Sock();
	if ((soc.fd = soc.mkSock()) == -1) exit(1);
	printf("%d\n", soc.fd);

	// �������� ���� ���� Ȯ���� ���� �޽����� �޴´�. (���������� ���� ��ȣ�� ���� ������.)
	// �׸��� �ڽ��� ù��° �����̶�� ���� �˸��� ��Ŷ�� ������.
	soc.receive_nbytes(soc.fd, &soc.serverSock, sizeof(int));

	Header hd;
	int pktLen = soc.make_packet(&hd, NULL, MSG_SOC, 0);
	printf("%d\n", ntohl(hd.type));
	soc.send_nbytes(soc.fd, &hd, pktLen);
	/////////////////////////////////////////////////////////////////
	for (int i = 0; i < 50; i++)
		mCtrlList.InsertItem(i, _T(""));

	files  = FileControl(soc, 50);
	files.setCurrentDir("/Users/user/Desktop");

	_beginthreadex(NULL, 0, CMFCclientDlg::recvPacket, this, 0, NULL);
	
	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}



void CMFCclientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�.  ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CMFCclientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CMFCclientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMFCclientDlg::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CDialogEx::OnOK();
}

void CMFCclientDlg::OnBnClickedButton1()
{
	// ����Ʈ ��Ʈ�ѿ��� ���õ� ������Ʈ�� �����´�.
	// ������Ʈ�� ���� Ÿ���� �˻��ؼ� ���丮�� ��� ���ν����� ����ǵ��� �Ѵ�.
	// ���� ������� �Ľ��ؼ� ������ �����Ѵ�.

	if (fType == DIRECTORY && strcmp(dName, "."))
	{
		if (strcmp(dName, "..") == 0)
		{
			if (!files.currentDir.compare("/Users/user/Desktop")) return;

			int lastIdx = files.currentDir.rfind("/");
			files.currentDir = files.currentDir.substr(0, lastIdx);
		}
		else
		{
			files.currentDir = files.filePath;
		}
		char header[256] = { 0, };
		char path[244] = { 0, };
		memcpy(path, files.currentDir.c_str(), files.currentDir.length());
		printf("%s\n", path);

		files.clearBuffer();
		int pktLen = soc.make_packet(header, path, RQ_FILELIST, files.currentDir.length());
		soc.send_nbytes(soc.fd, header, pktLen);
	}
}

void CMFCclientDlg::OnBnClickedButton3()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CString str;
	GetDlgItemText(IDC_EDIT1, str);

	fileSoc = Sock();
	
	// ���� ���� ���� connect
	fileSoc.fd = fileSoc.mkSock();
	fileSoc.receive_nbytes(fileSoc.fd, &fileSoc.serverSock, sizeof(int));

	Header hd;
	int pktLen = fileSoc.make_packet(&hd, NULL, FILE_SOC, 0);
	fileSoc.send_nbytes(fileSoc.fd, &hd, pktLen);

	// ���� ����
	files.send(LPSTR(LPCTSTR(str)), &mProgress, &fileSoc, (void*)this);
}

// ���õ� ������ �̸��� ����, Ÿ���� �����Ѵ�.
void CMFCclientDlg::OnNMClickList3(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<NMITEMACTIVATE*>(pNMHDR);

	int index = 0;
	index = pNMItemActivate->iItem;
	if (index >= 0 && index < mCtrlList.GetItemCount())
	{
		CString data = mCtrlList.GetItemText(index, 1);
		CString len  = mCtrlList.GetItemText(index, 2);

		memset(dName, 0, 244);
		strncpy(dName, LPSTR(LPCTSTR(data)), strlen(LPSTR(LPCTSTR(data))));
		fLen  = atoi(LPSTR(LPCTSTR(len)));
		fType = dp[index].fileType;

	//	printf("%s %s, %d\n", dName, fLen, files[index].fileType);

		// ��� ���� �̾���̱�
		files.filePath = files.currentDir +'/' + dName;
	}
	*pResult = 0;
}


void CMFCclientDlg::OnBnClickedButton2()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	static TCHAR BASED_CODE szFilter[] = _T("�������(*.*)|*.*||");
	CFileDialog dlg(TRUE, _T("*.jpg"), _T(""), OFN_HIDEREADONLY, szFilter);
	if (IDOK == dlg.DoModal())
	{
		CString pathName = dlg.GetPathName();
		char* st = LPSTR(LPCTSTR(pathName));

		int stLen = strlen(st);
		for (int i = 0; i < stLen; i++)
			st[i] = st[i] == '\\' ? '/' : st[i];

	//	fPath.SetWindowTextA(pathName);
		fPath.SetWindowTextA(pathName);
	}
}


void CMFCclientDlg::OnBnClickedButton4()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	// ���ַ� ������ �������� �ÿ��� ����. ���� ���� ���ν���
	// �������ϱ� �׳� ���� ���丮���ٰ� ���� ���� �սô�!
	
	if (fType == REGFILE)
	{
		BROWSEINFO   bi;
		LPITEMIDLIST  idl;

		ZeroMemory(&bi, sizeof(BROWSEINFO));
		bi.hwndOwner = this->m_hWnd;  // ���õ� ���丮�� ���� 
		bi.lpszTitle = "������ ������ �ּ���";
		bi.ulFlags = BIF_EDITBOX    // ���õ� ���丮���� ǥ���ϰ� ������ �� �ִ� â
			| 0x0040             // ���ο� ���丮�� ����� �ִ� ��ư �߰� 
			;

		char m_dirFolderPath[256] = { 0, };
		idl = SHBrowseForFolder(&bi);
		if (idl) {
			SHGetPathFromIDList(idl, m_dirFolderPath); // ��ü PATH�� ������ ���丮��

			char fName[252] = { 0, };
			memcpy(fName, files.filePath.c_str(), files.filePath.length());

			fileSoc.fd = fileSoc.mkSock();
			files.fileSoc = &fileSoc;

			fileSoc.receive_nbytes(fileSoc.fd, &fileSoc.serverSock, sizeof(int));

			Header hd;
			int pktLen = fileSoc.make_packet(&hd, NULL, FILE_SOC, 0);
			fileSoc.send_nbytes(fileSoc.fd, &hd, pktLen);

			string dirPath = m_dirFolderPath;
			files.recv(fName, &mProgress, &fileSoc, (void*)this, dirPath);
		}
	}
}


//mkdir
void CMFCclientDlg::OnBnClickedButton5()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	// Ŭ���� �˾�â �ϳ� ����� ���丮 �̸� ���޹���.
	// ������ ��ü ��θ� �����Ѵ�.
	// �ߺ��� �̸��ϰ��� ������ ��쿡 ���ؼ� ó�����ָ� ��.
	mkd.Create(IDD_DIALOG1, this);
	mkd.CenterWindow();
	mkd.ShowWindow(SW_SHOW);
}

//rm
void CMFCclientDlg::OnBnClickedButton6()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	printf("%s\n", files.filePath.c_str());
	char pkt[256] = { 0, };

	int pktLen = soc.make_packet(pkt, files.filePath.c_str(), RM, files.filePath.length());
	soc.send_nbytes(soc.fd, pkt, pktLen);
}
