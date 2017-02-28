#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"
#include "Sock.h"
#include "FileControl.h"
#include "MFCclientDlg.h"

struct package
{
	CMFCclientDlg* mainDlg;
	FileControl*		me;
}pac;

FileControl::FileControl(Sock socket, int dirSize)
{
	soc = socket;
	size = dirSize;
	dir = new Directory[size];
	memset(dir, 0, sizeof(Directory) * dirSize);
}

FileControl::FileControl()
{
}

FileControl::~FileControl()
{
}


void FileControl::setCurrentDir(string curDir)
{
	currentDir = curDir;
}

Directory* FileControl::getFileList(int data_len)
{
	memset(dir, 0, sizeof(Directory) * size);
	soc.receive_nbytes(soc.fd, dir, data_len);

	return dir;
}

void FileControl::send(char *str, CProgressCtrl * mProgress, Sock * fileSoc, void * arg)
{
	memset(this->str, 0, 256);
	strncpy_s(this->str, str, strlen(str));
	this->mProgress = mProgress;
	this->fileSoc = fileSoc;

	pac = { (CMFCclientDlg*)arg, this };

	_beginthreadex(NULL, 0, sendFile, &pac, 0, NULL);
}
void FileControl::recv(char *str, CProgressCtrl * mProgress, Sock * fileSoc, void * arg, string recvPath)
{
	memset(this->str, 0, 256);
	strncpy_s(this->str, str, strlen(str));
	this->mProgress = mProgress;
	this->fileSoc = fileSoc;
	this->recvPath = recvPath;

	pac = { (CMFCclientDlg*)arg, this };

	_beginthreadex(NULL, 0, recvFile, &pac, 0, NULL);
}

unsigned int __stdcall FileControl::sendFile(void * arg)
{
	struct package *ptr = (struct package*)arg;

	CMFCclientDlg* mainDlg = ptr->mainDlg;
	FileControl* me = ptr->me;

	mainDlg->recvBtn.EnableWindow(FALSE);
	mainDlg->sendBtn.EnableWindow(FALSE);

	FILE* file = fopen(me->str, "rb");
	if (file == NULL)
		exit(1);

	long long fsize = 0, csize = 0;
	char data[4096] = { 0, };

	_fseeki64(file, 0, SEEK_END);
	fsize = _ftelli64(file);
	_fseeki64(file, 0, SEEK_SET);

	//파일이름, 파일길이 전송
	FileStream stream;

	memset(&stream, 0, sizeof(FileStream));
	stream.length = fsize;
	printf("%lld", stream.length);

	char* p = me->getLastString(LPSTR(LPCTSTR(me->str)));
	string entireName = me->currentDir + '/' + p;
	strncpy(stream.name, entireName.c_str(), entireName.length());
	printf("%s\n", entireName.c_str());

	//free(p);
	printf("file send\n");
	char tmp[300] = { 0, };
	int pktLen = me->fileSoc->make_packet(tmp, (char*)&stream, SEND_FILE, sizeof(FileStream));
	me->fileSoc->send_nbytes(me->fileSoc->fd, tmp, pktLen);

	//메인파일 전송
	me->mProgress->SetPos(0);

	while (fsize > csize)
	{
		int tmp_size;
		csize += tmp_size = fread(data, 1, PIECE, file);

		char pkt[PIECE + sizeof(Header)] = { 0, };
		pktLen = me->fileSoc->make_packet(pkt, data, SEND_FILE, tmp_size);
		me->fileSoc->send_nbytes(me->fileSoc->fd, pkt, pktLen);

		me->mProgress->SetPos((int)((csize * 100) / fsize));
	}

	free(p);
	fclose(file);

	mainDlg->recvBtn.EnableWindow(TRUE);
	mainDlg->sendBtn.EnableWindow(TRUE);
}

unsigned int __stdcall FileControl::recvFile(void * arg)
{
	struct package *ptr = (struct package*)arg;

	CMFCclientDlg* mainDlg = ptr->mainDlg;
	FileControl* me = ptr->me;

	mainDlg->recvBtn.EnableWindow(FALSE);
	mainDlg->sendBtn.EnableWindow(FALSE);

	char pkt[300] = { 0, };

	printf("파일 보낸당\n");
	me->clearBuffer();

	int pktLen = me->fileSoc->make_packet(pkt, me->str, RECV_FILE, me->filePath.length());
	me->fileSoc->send_nbytes(me->fileSoc->fd, pkt, pktLen);

	// 파일 받기 프로시저
	
	long long fsize = 0, csize = 0;
	me->fileSoc->receive_nbytes(me->fileSoc->fd, &fsize, sizeof(long long));

	printf("%lld\n", fsize);
	me->mProgress->SetPos(0);

	// 실제 파일 전송/수신 부분만 스레드로 구현 ?? ( 앞부분 파일요청, 길이 수신부분은 메인스레드 처리 )
	string path = me->recvPath;
	path.append("/");
	path.append(me->getLastString(me->filePath.c_str()));

	printf("%s\n", path.c_str());

	FILE* file = fopen(path.c_str(), "wb+");
	while (fsize > csize)
	{
		// 패킷 헤더와 몸체 받는 부분을 뮤텍스로 감싸기 or 파일 스트림 소켓 포트를 새로 하나 열기
		Header header;
		me->fileSoc->receive_nbytes(me->fileSoc->fd, &header, sizeof(Header));

		char data[4096];
		csize += me->fileSoc->receive_nbytes(me->fileSoc->fd, data, ntohl(header.data_len));
		fwrite(data, 1, ntohl(header.data_len), file);

		me->mProgress->SetPos((int)((csize * 100) / fsize));
	}
	fclose(file);

	mainDlg->recvBtn.EnableWindow(TRUE);
	mainDlg->sendBtn.EnableWindow(TRUE);
	return 0;
}

char * FileControl::getLastString(const char * st)
{
	string tmp = st;

	int lastIdx = tmp.rfind("/");
	tmp = tmp.substr(lastIdx + 1, tmp.length() - (lastIdx + 1));
	printf("%s\n", tmp.c_str());

	char * p = new char[50];
	memset(p, 0, 50);
	strncpy(p, tmp.c_str(), tmp.length());

	return p;
}

void FileControl::clearBuffer()
{
	char tmp_char;
	unsigned long tmp_long = 0;
	long i;

	if (ioctlsocket(soc.fd, FIONREAD, &tmp_long) != SOCKET_ERROR) {
		for (i = 0; i < tmp_long; i++) {
			soc.receive_nbytes(soc.fd, &tmp_char, sizeof(char));
		}
	}
}
