#pragma once

#ifndef SOCK_H_
#define SOCK_H_

#include <WinSock2.h>

#define RQ_FILELIST 0
#define SEND_FILE   1
#define RECV_FILE   2
#define WHAT_SOCK	3
#define MSG_SOC		4
#define FILE_SOC	5
#define MKDIR       6
#define MKDIR_FAIL  7
#define RM          8
#define RM_FAIL     9

#define PIECE 4096

typedef struct Header
{
	unsigned int     type;
	unsigned int data_len;
};

typedef struct Directory
{
	char	  fileName[244];
	int		  fileType;
	long long fileLen;
};

typedef struct FileStream
{
	long long  length;
	char	   name[252];
};

typedef struct _Long
{
	long long pkt;
};

class Sock
{
public:
	int			fd;
	int serverSock;

	Sock();
	int mkSock();
	int make_packet(void * pkt, const char * data, int type, int data_size);
	int receive_nbytes(int fd, void* vptr, int n);
	int send_nbytes(int fd, const void* vptr, size_t n);
};
#endif