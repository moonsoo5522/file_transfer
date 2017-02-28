#pragma once

#include <string>
#include <process.h>


using namespace std;

class FileControl
{
public:
	int			 size;
	Sock		  soc;
	Directory*    dir;
	string currentDir;
	string	 filePath;
	string	 recvPath;
	//////////////////
	char			str[256];
	CProgressCtrl* mProgress;
	Sock*			 fileSoc;

	FileControl(Sock socket, int dirSize);
	FileControl();
	~FileControl();
	Directory* getFileList(int data_len);
	void send(char * str, CProgressCtrl* mProgress, Sock* fileSoc, void* arg);
	void recv(char * str, CProgressCtrl * mProgress, Sock * fileSoc, void* arg, string recvPath);
	static unsigned int __stdcall sendFile(void * arg);
	static unsigned int __stdcall recvFile(void * arg);
	void setCurrentDir(string curDir);
	char * getLastString(const char * st);
	void clearBuffer(); // 임시 코드... 원인 해결시 삭제
};

