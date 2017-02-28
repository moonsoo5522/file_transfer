/*
 객체지향적으로 짜도록 하자
 헤더파일에 클래스 및 프로시저 정의
 이 파일에서 구체화
*/

#include "stdafx.h"
#include "Sock.h"
#include <WinSock2.h>

#define PORT 50000

Sock::Sock()
{
	int		   iRet;
	WSADATA wsaData;

	iRet = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iRet != 0)
		exit(1);
}

int Sock::mkSock()
{
	int			    i, sock;
	struct sockaddr_in serv;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1)
		return -1;

	memset(&serv, 0, sizeof(struct sockaddr_in));
	serv.sin_family = AF_INET;
	serv.sin_port = htons(PORT);
	serv.sin_addr.s_addr = inet_addr("192.168.0.8");

	// 커넥트 추가
	if(connect(sock, (sockaddr*)&serv, sizeof(sockaddr)) == -1)
		exit(1);
	printf("소켓 %d\n", sock);
	return sock;
}

int Sock::make_packet(void * pkt, const char * data, int type, int data_size)
{
	unsigned int packet_type = type;
	unsigned int	data_len = data_size;

	Header cmn_pkt_hdr;

	cmn_pkt_hdr.type = htonl(packet_type);
	cmn_pkt_hdr.data_len = htonl(data_size);

	int hdr_len = sizeof(struct Header);

	memcpy(pkt, (const void*)&cmn_pkt_hdr, hdr_len);

	memcpy((void *)((char *)pkt + hdr_len), data, data_size);

	return hdr_len + data_size;
}

int Sock::receive_nbytes(int fd, void* vptr, int n)
{
	int   nleft;
	int   nread;
	char*   ptr;

	ptr = (char *)vptr;
	nleft = n;
	while (nleft > 0)
	{           /* keep reading upto n bytes     */
		if ((nread = recv(fd, ptr, nleft, 0)) < 0)
		{
			return -1;
		}

		else if (nread == 0)
		{
			break;                    /* EOF */
		}
		nleft -= nread;
		ptr += nread;
	}

	return(n - nleft);            /* return >= 0 */
}

int Sock::send_nbytes(int fd, const void* vptr, size_t n)
{
	int        nleft;
	int     nwritten;
	const char*  ptr;

	ptr = (const char*)vptr;
	nleft = n;

	while (nleft > 0) {
		if ((nwritten = send(fd, ptr, nleft, 0)) <= 0)
		{
			return(-1);
		}
		nleft -= nwritten;
		ptr += nwritten;
	}
	return(n);
}