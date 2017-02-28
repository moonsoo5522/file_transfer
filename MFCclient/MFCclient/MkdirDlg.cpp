// MkdirDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "MFCclient.h"
#include "MkdirDlg.h"
#include "MFCclientDlg.h"
#include "afxdialogex.h"
#include "Sock.h"


// CMkdirDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CMkdirDlg, CDialogEx)

CMkdirDlg::CMkdirDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}

CMkdirDlg::~CMkdirDlg()
{
}

void CMkdirDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, dirName);
}


BEGIN_MESSAGE_MAP(CMkdirDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CMkdirDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CMkdirDlg �޽��� ó�����Դϴ�.


void CMkdirDlg::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CMFCclientDlg * parent = (CMFCclientDlg*)AfxGetMainWnd();
	Sock soc = parent->soc;
	FileControl ctl = parent->files;

	CString tmp;
	GetDlgItemText(IDC_EDIT1, tmp);
	
	string fullPath = parent->files.currentDir;
	fullPath.append("/");
	fullPath.append(tmp);
	char pkt[256] = { 0, };
	char buf[248] = { 0, };
	
	strncpy(buf, fullPath.c_str(), fullPath.length());
	
	int pktLen = soc.make_packet(pkt, buf, MKDIR, strlen(buf));
	soc.send_nbytes(soc.fd, pkt, pktLen);
	
	CDialogEx::OnOK();
}
