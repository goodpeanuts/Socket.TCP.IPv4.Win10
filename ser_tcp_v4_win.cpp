#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "WinSock2.h"
#include "iostream"
#include <fstream>
#pragma comment(lib,"ws2_32.lib")  
#define PORT 65035
#define BUFFER_SIZE 1024
using namespace std;

//发给客户端的信息
char msg[] = "连接成功，请选择需要的文件\n\ttest1.txt\n\ttest2.txt\n\ttest3.txt\n";  

int main() {
	//加载套接字库,约定版本2.2
	WSADATA wsaData;                 
	WSAStartup(MAKEWORD(2,2), &wsaData);
	
	//建立流式套接字,分别为服务端和新连接
	SOCKET sock_server, newsock;    
	struct sockaddr_in addr;         
	struct sockaddr_in client_addr;  
	char buffer[BUFFER_SIZE];
	
	//设置服务端套接字,使用IPv4地址族,TCP协议
	//接受所有网络地址的连接申请
	sock_server = socket(AF_INET, SOCK_STREAM, 0);
	int addr_len = sizeof(struct sockaddr_in);
	memset((void*)&addr, 0, addr_len);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = inet_addr("0.0.0.0");   
	
	//监听套接字绑定地址
	bind(sock_server, (struct sockaddr*)&addr, sizeof(addr));
	
	//监听客户的连接请求
	listen(sock_server, 0);
	
	//接收连接请求并收发数据
	//程序会一直循环执行,持续监听客户端连接和进行数据交互，
	//直到手动停止程序
	int size;
	while (true) {
		cout << "    服务器正在运行,监听连接请求中\n";
		newsock = accept(sock_server, (struct sockaddr*)&client_addr, &addr_len);  
		cout << "    成功与客户端建立连接\n";
		
		// 与客户端建立连接
		size = send(newsock, msg, sizeof(msg), 0);
		if (size == SOCKET_ERROR) {
			cout << "    发送连接成功消息失败！错误代码：" << WSAGetLastError() << endl;
			closesocket(newsock);
			continue;
		}
		
		// 接收客户端请求文件名;若请求文件名不存在,断开连接
		size = recv(newsock, buffer, BUFFER_SIZE, 0);
		if (size == 0) {
			cout << "    客户端断开连接\n";
			closesocket(newsock);
			continue;
		}
		else {
			buffer[size] = '\0';
			string filename = buffer;
			cout << "    客户端请求文件: " << filename << endl;
			
			// 打开文件
			ifstream file(filename, ios::binary | ios::ate);
			if (!file) {
				cout << "    无法打开文件" << endl;
				closesocket(newsock);
				continue;
			}
			
			// 获取文件大小
			streampos fileSize = file.tellg();
			file.seekg(0, ios::beg);
			
			// 发送文件大小;若大小为0,发送失败
			size = send(newsock, (char*)&fileSize, sizeof(fileSize), 0);
			if (size == SOCKET_ERROR) {
				cout << "    发送文件,失败错误代码：" << WSAGetLastError() << endl;
				closesocket(newsock);
				file.close();
				continue;
			}
			
			// 发送文件内容
			while (fileSize > 0) {
				size = file.readsome(buffer, BUFFER_SIZE);
				if (size == 0) {
					cout << "    读取文件内容失败" << endl;
					break;
				}
				else {
					size = send(newsock, buffer, size, 0);
					if (size == SOCKET_ERROR) {
						cout << "    发送文件内容失败,错误代码：" << WSAGetLastError() << endl;
						break;
					}
					fileSize -= size;
				}
			}
			
			file.close();
			cout << "    文件发送完成" << endl;
			cout << "---------------- \n"<<endl;
		}
		
		//发送完成,结束本次TCP连接
		closesocket(newsock);
	}
	
	//关闭套接字和清理Winsock库的资源
	closesocket(sock_server);
	WSACleanup();
	return 0;
}

