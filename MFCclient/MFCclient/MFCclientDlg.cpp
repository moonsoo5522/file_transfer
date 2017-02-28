#define _CRT_SECURE_NO_WARNINGS
// MFCclientDlg.cpp : 구현 파일
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

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
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


// CMFCclientDlg 대화 상자



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
			mCtrlList.SetItemText(i, 0, _T("일반 파일"));
		}
		else
			mCtrlList.SetItemText(i, 0, _T("폴더"));
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
			AfxMessageBox(_T("폴더를 생성하였습니다."));
			break;
		case MKDIR_FAIL:
			AfxMessageBox(_T("중복된 이름입니다."));
			break;
		case RM:
			AfxMessageBox(_T("성공적으로 삭제되었습니다."));
			break;
		case RM_FAIL:
			AfxMessageBox(_T("삭제에 실패하였습니다."));
			break;
		}
	}
	return 0;
}

// CMFCclientDlg 메시지 처리기

BOOL CMFCclientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	char *title[3] = { "형식", "파일명", "크기" };
	int size[3] = { 100, 400, 150 };
	LV_COLUMN lCol = { 0, };

	lCol.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lCol.fmt = LVCFMT_LEFT;

	mCtrlList.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	
	mProgress.SetRange(0, 100);  //진행폭을 결정
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

	// 서버에서 소켓 종류 확인차 보낸 메시지를 받는다. (서버에서는 소켓 번호를 같이 보낸다.)
	// 그리고 자신이 첫번째 소켓이라는 것을 알리는 패킷을 보낸다.
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
	
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMFCclientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CMFCclientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMFCclientDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnOK();
}

void CMFCclientDlg::OnBnClickedButton1()
{
	// 리스트 컨트롤에서 선택된 엘리먼트를 가져온다.
	// 엘리먼트의 파일 타입을 검사해서 디렉토리일 경우 프로시저가 실행되도록 한다.
	// 이후 경로정보 파싱해서 서버로 질의한다.

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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString str;
	GetDlgItemText(IDC_EDIT1, str);

	fileSoc = Sock();
	
	// 파일 전송 소켓 connect
	fileSoc.fd = fileSoc.mkSock();
	fileSoc.receive_nbytes(fileSoc.fd, &fileSoc.serverSock, sizeof(int));

	Header hd;
	int pktLen = fileSoc.make_packet(&hd, NULL, FILE_SOC, 0);
	fileSoc.send_nbytes(fileSoc.fd, &hd, pktLen);

	// 파일 전송
	files.send(LPSTR(LPCTSTR(str)), &mProgress, &fileSoc, (void*)this);
}

// 선택된 파일의 이름과 길이, 타입을 갱신한다.
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

		// 경로 정보 이어붙이기
		files.filePath = files.currentDir +'/' + dName;
	}
	*pResult = 0;
}


void CMFCclientDlg::OnBnClickedButton2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	static TCHAR BASED_CODE szFilter[] = _T("모든파일(*.*)|*.*||");
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// 레귤러 파일을 선택했을 시에만 동작. 파일 수신 프로시저
	// 귀찮으니까 그냥 현재 디렉토리에다가 파일 저장 합시다!
	
	if (fType == REGFILE)
	{
		BROWSEINFO   bi;
		LPITEMIDLIST  idl;

		ZeroMemory(&bi, sizeof(BROWSEINFO));
		bi.hwndOwner = this->m_hWnd;  // 선택된 디렉토리명 저장 
		bi.lpszTitle = "폴더를 선택해 주세요";
		bi.ulFlags = BIF_EDITBOX    // 선택된 디렉토리명을 표시하고 편집할 수 있는 창
			| 0x0040             // 새로운 디렉토리를 만들수 있는 버튼 추가 
			;

		char m_dirFolderPath[256] = { 0, };
		idl = SHBrowseForFolder(&bi);
		if (idl) {
			SHGetPathFromIDList(idl, m_dirFolderPath); // 전체 PATH을 포함한 디렉토리명

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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// 클릭시 팝업창 하나 띄워서 디렉토리 이름 전달받음.
	// 서버로 전체 경로명 전송한다.
	// 중복된 이름일경우와 성공한 경우에 대해서 처리해주면 끝.
	mkd.Create(IDD_DIALOG1, this);
	mkd.CenterWindow();
	mkd.ShowWindow(SW_SHOW);
}

//rm
void CMFCclientDlg::OnBnClickedButton6()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	printf("%s\n", files.filePath.c_str());
	char pkt[256] = { 0, };

	int pktLen = soc.make_packet(pkt, files.filePath.c_str(), RM, files.filePath.length());
	soc.send_nbytes(soc.fd, pkt, pktLen);
}
