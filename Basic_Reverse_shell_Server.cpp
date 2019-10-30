#include<winsock2.h>
#include<windows.h>
#include<iostream>
#pragma comment(lib, "ws2_32.lib")

using std::cin;
using std::endl;
using std::cout;

void Initialization();

void Display_Error(const char* sentence);

void Initialization()
{
	LPWSADATA lpWSAData;
	lpWSAData = new WSADATA;
	if (WSAStartup(2.2, lpWSAData))
	{
		cout << "Failed to Initialization" << endl;
		exit(1);
	}
}


void Display_Error(const char* sentence)
{
	cout << sentence << endl;
	WSACleanup();
	exit(1);
}


int main()
{
	//初始化
	Initialization();

	SOCKET S = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, NULL);
	if (S == INVALID_SOCKET)
	{
		const char* Error = "Failed to create socket";
		Display_Error(Error);
	}

	sockaddr_in SocketAddr;
	ZeroMemory(&SocketAddr, sizeof(SocketAddr));
	SocketAddr.sin_family = AF_INET;
	SocketAddr.sin_port = htons(8888);
	SocketAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (bind(S, (const sockaddr*)& SocketAddr, sizeof(SocketAddr)) == SOCKET_ERROR)
	{ 
		cout << WSAGetLastError() << endl;
		closesocket(S);
		const char* Error = "Failed to bind socket";
		Display_Error(Error);
	}

	if (listen(S, 5))
	{
		closesocket(S);
		const char* Error = "Failed to listen socket";
		Display_Error(Error);
	}
	while (true)
	{
		sockaddr_in* AddrClient = new sockaddr_in;
		int size = sizeof(sockaddr_in);
		SOCKET S_new = WSAAccept(S, (sockaddr*)AddrClient, &size,NULL,NULL);
		if (S_new == INVALID_SOCKET)
		{
			closesocket(S);
			const char* Error = "Failed to Accept request";
			Display_Error(Error);
		}
		//设置为非阻塞模式
		int Mode = 1;
		ioctlsocket(S_new, FIONBIO, (u_long FAR*) & Mode);

		//c初始化接受与发送缓冲区
		char* recv_buffer = new char[1000];
		char* send_buffer = new char[1000];
		ZeroMemory(send_buffer, 1000);
		ZeroMemory(recv_buffer, 1000);

		cout << "Connection from:" << inet_ntoa(AddrClient->sin_addr) << endl;

		while (true)
		{
			if (recv(S_new, recv_buffer,1000,0)== SOCKET_ERROR)
			{ 
				
				if (WSAGetLastError()!= WSAEWOULDBLOCK)
				{
					closesocket(S_new);
					break;
				}
			}

			if (strlen(recv_buffer) != 0)
			{
				cout << recv_buffer << endl;
				ZeroMemory(recv_buffer, 1000);
				continue;
			}
			else
			{
				cout << "Enter Command:";
				cin >> send_buffer;
				send_buffer[strlen(send_buffer)] = '\n';
				send_buffer[strlen(send_buffer)] = '\0';
				send(S_new, send_buffer, strlen(send_buffer), 0);
				if(!strcmp(send_buffer,"exit\n"))
				{
					cout << "Disconnect" << endl;
					closesocket(S_new);
					break;
				}
				ZeroMemory(send_buffer, 1000);
				Sleep(1000);
			}
		}
	}
}
