#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<iostream>
#include<WinSock2.h>
#include <string>
#include<sstream>
#include<istream>
#include <fstream>

#pragma comment(lib,"ws2_32.lib")

#define SEVER_IP "183.47.101.192"
#define SEVER_PORT 25
#define USERNAME "2311619185@qq.com"
#define SENDER_EMAIL_ADDRESS "2311619185@qq.com"
#define RECIPIENT_EMAIL_ADDRESS "2311619185@qq.com"
#define PASSWORD "sqegtmfvtbtpdjae"

#define BUFFER_SIZE 1024

//手动进行base64编码，不安全，后续可以换成第三方库实现
const char* base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
std::string base64_encode(const std::string& s) {
	std::string result;
	int i = 0;
	int j = 0;
	unsigned char char_array_3[3];
	unsigned char char_array_4[4];

	for (std::string::const_iterator iter = s.begin(); iter != s.end(); ++iter) {
		char_array_3[i++] = *iter;
		if (i == 3) {
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for (i = 0; i < 4; i++) {
				result += base64_chars[char_array_4[i]];
			}
			i = 0;
		}
	}

	if (i) {
		for (j = i; j < 3; j++) {
			char_array_3[j] = '\0';
		}
		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

		for (j = 0; j < i + 1; j++) {
			result += base64_chars[char_array_4[j]];
		}

		while (i++ < 3) {
			result += '=';
		}
	}

	return result;
}

int main() 
{

	//初始化winsock
	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0) {
		std::cerr << "WSAStartup failed: " << result << std::endl;
		return 1;
	}
	//创建套接字
	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) {
		std::cerr << "socket failed : " << WSAGetLastError() << std::endl;
		WSACleanup();
		return 1;
	}
	//连接到SMTP服务器
	SOCKADDR_IN severAddr;
	severAddr.sin_family = AF_INET;
	severAddr.sin_port = htons(SEVER_PORT);
	severAddr.sin_addr.s_addr = inet_addr(SEVER_IP);
	result = connect(sock, (SOCKADDR*)&severAddr, sizeof(severAddr));
	if (result == SOCKET_ERROR) {
		std::cerr << "connect failed : " << WSAGetLastError() << std::endl;
		closesocket(sock);
		WSACleanup();
		return 1;
	}
	//接收服务器欢迎消息
	char buffer[BUFFER_SIZE+1];
	result = recv(sock, buffer, BUFFER_SIZE, 0);
	if (result == SOCKET_ERROR) {
		std::cerr << "recv failed: " << WSAGetLastError() << std::endl;
		closesocket(sock);
		WSACleanup();
		return 1;
	}
	buffer[result] = '\0';
	std::cout << buffer << std::endl;
	//发送EHLO命令
	std::stringstream ss;
	ss << "EHLO " << "localhost" << "\r\n";
	std::string ehloCmd = ss.str();
	result = send(sock, ehloCmd.c_str(), ehloCmd.length(), 0);
	if (result == SOCKET_ERROR) {
		std::cerr << "send failded: " << WSAGetLastError() << std::endl;
		closesocket(sock);
		WSACleanup();
		return 1;
	}
	//接收EHLO响应
	result = recv(sock, buffer, BUFFER_SIZE, 0);
	if (result == SOCKET_ERROR) {
		std::cerr << "recv failed: " << WSAGetLastError() << std::endl;
		closesocket(sock);
		WSACleanup();
		return 1;
	}
	buffer[result] = '\0';
	std::cout << buffer << std::endl;
	//发送AUTH LOGIN命令
	result = send(sock, "AUTH LOGIN\r\n", 12, 0);
	if (result == SOCKET_ERROR) {
		std::cerr << "send failed: " << WSAGetLastError() << std::endl;
		closesocket(sock);
		WSACleanup();
		return 1;
	}
	//接收AUTH LOGIN响应
	result = recv(sock, buffer, BUFFER_SIZE, 0);
	if (result == SOCKET_ERROR) {
		std::cerr << "recv failed: " << WSAGetLastError() << std::endl;
		closesocket(sock);
		WSACleanup();
		return 1;
	}
	buffer[result] = '\0';
	std::cout << buffer << std::endl;
	//发送用户名
	std::string usernameBase64 = base64_encode(USERNAME);
	result = send(sock, usernameBase64.c_str(), usernameBase64.length(), 0);
	if (result == SOCKET_ERROR) {
		std::cerr << "send failed: " << WSAGetLastError() << std::endl;
		closesocket(sock);
		WSACleanup();
		return 1;
	}
	result = send(sock, "\r\n", 2, 0);
	if (result == SOCKET_ERROR) {
		std::cerr << "send failed: " << WSAGetLastError() << std::endl;
		closesocket(sock);
		WSACleanup();
		return 1;
	}
	//接收用户名响应
	result = recv(sock, buffer, BUFFER_SIZE, 0);
	if (result == SOCKET_ERROR) {
		std::cerr << "recv failed: " << WSAGetLastError() << std::endl;
		closesocket(sock);
		WSACleanup();
		return 1;
	}
	buffer[result] = '\0';
	std::cout << buffer << std::endl;
	//发送授权码/密码
	std::string passwordBase64 = base64_encode(PASSWORD);
	result = send(sock, passwordBase64.c_str(), passwordBase64.length(),0);
	if (result == SOCKET_ERROR) {
		std::cerr << "send failed: " << WSAGetLastError() << std::endl;
		closesocket(sock);
		WSACleanup();
		return 1;
	}
	result = send(sock, "\r\n", 2, 0);
	if (result == SOCKET_ERROR) {
		std::cerr << "send failed: " << WSAGetLastError() << std::endl;
		closesocket(sock);
		WSACleanup();
		return 1;
	}
	//接收密码响应
	result = recv(sock, buffer, BUFFER_SIZE, 0);
	if (result == SOCKET_ERROR) {
		std::cerr << "recv failed: " << WSAGetLastError() << std::endl;
		closesocket(sock);
		WSACleanup();
		return 1;
	}
	buffer[result] = '\0';
	std::cout << buffer << std::endl;
	//发送MAIL FROM命令
	ss.str("");
	ss << "MAIL FROM:<" << SENDER_EMAIL_ADDRESS << ">\r\n";
	std::string mailFromCmd = ss.str();
	result = send(sock, mailFromCmd.c_str(), mailFromCmd.length(), 0);
	if (result == SOCKET_ERROR) {
		std::cerr << "send failed: " << WSAGetLastError() << std::endl;
		closesocket(sock);
		WSACleanup();
		return 1;
	}
	//接收MAIL FROM响应
	result = recv(sock, buffer, BUFFER_SIZE, 0);
	if (result == SOCKET_ERROR) {
		std::cerr << "recv failed: " << WSAGetLastError() << std::endl;
		closesocket(sock);
		WSACleanup();
		return 1;
	}
	buffer[result] = '\0';
	std::cout << buffer << std::endl;
	//发送RCPT TO命令
	ss.str("");
	ss << "RCPT TO:<" << RECIPIENT_EMAIL_ADDRESS << ">\r\n";
	std::string rcptToCmd = ss.str();
	result = send(sock, rcptToCmd.c_str(), rcptToCmd.length(), 0);
	if (result == SOCKET_ERROR) {
		std::cerr << "send failed: " << WSAGetLastError() << std::endl;
		closesocket(sock);
		WSACleanup();
		return 1;
	}
	//接收RCPT TO响应
	result = recv(sock, buffer, BUFFER_SIZE, 0);
	if (result == SOCKET_ERROR) {
		std::cerr << "recv failed: " << WSAGetLastError() << std::endl;
		closesocket(sock);
		WSACleanup();
		return 1;
	}
	buffer[result] = '\0';
	std::cout << buffer << std::endl;
	// 发送 DATA 命令
	result = send(sock, "DATA\r\n", 6, 0);
	if (result == SOCKET_ERROR) {
		std::cerr << "send failed: " << WSAGetLastError() << std::endl;
		closesocket(sock);
		WSACleanup();
		return 1;
	}

	// 接收 DATA 命令响应
	result = recv(sock, buffer, BUFFER_SIZE, 0);
	if (result == SOCKET_ERROR) {
		std::cerr << "recv failed: " << WSAGetLastError() << std::endl;
		closesocket(sock);
		WSACleanup();
		return 1;
	}
	buffer[result] = '\0';
	std::cout << buffer << std::endl;
	//发送邮件内容
	std::string from = SENDER_EMAIL_ADDRESS;
	std::string to = RECIPIENT_EMAIL_ADDRESS;
	std::string subject = "Test Email";
	std::string body = "Hello, this is a test email.";
	//构造邮件头部
	std::stringstream ssHeader;
	ssHeader << "From: " << from << "\r\n";
	ssHeader << "To: " << to << "\r\n";
	ssHeader << "Subject: " << subject << "\r\n";
	ssHeader << "MIME-Version: 1.0\r\n";
	ssHeader << "Content-Type: multipart/mixed; boundary=boundary_1234\r\n\r\n";
	//构造邮件内容
	std::stringstream ssBody;
	ssBody << "--boundary_1234\r\n";
	ssBody << "Content-Type: text/plain; charset=UTF-8\r\n\r\n";
	ssBody << body << "\r\n\r\n";
	// 读取附件文件内容
	std::ifstream attachmentFile("attachment.txt");
	if (attachmentFile.is_open()) {
		std::stringstream ssAttachment;
		ssAttachment << attachmentFile.rdbuf();
		attachmentFile.close();

		// 构造附件内容
		ssBody << "--boundary_1234\r\n";
		ssBody << "Content-Type: text/plain; charset=UTF-8\r\n";
		ssBody << "Content-Transfer-Encoding: base64\r\n";
		ssBody << "Content-Disposition: attachment; filename=\"attachment.txt\"\r\n\r\n";
		ssBody << base64_encode((ssAttachment.str().c_str()));
		ssBody << "\r\n";
	}
	ssBody << "--boundary_1234--\r\n\r\n";
	// 发送邮件头部
	result = send(sock, ssHeader.str().c_str(), ssHeader.str().length(), 0);
	if (result == SOCKET_ERROR) {
		std::cerr << "send failed: " << WSAGetLastError() << std::endl;
		closesocket(sock);
		WSACleanup();
		return 1;
	}

	// 发送邮件内容
	result = send(sock, ssBody.str().c_str(), ssBody.str().length(), 0);
	if (result == SOCKET_ERROR) {
		std::cerr << "send failed: " << WSAGetLastError() << std::endl;
		closesocket(sock);
		WSACleanup();
		return 1;
	}
	//发送结束标识符
	result = send(sock, "\r\n.\r\n", 5, 0);
	if (result == SOCKET_ERROR) {
		std::cerr << "send failed: " << WSAGetLastError() << std::endl;
		closesocket(sock);
		WSACleanup();
		return 1;
	}
	//接收发送邮件的响应
	result = recv(sock, buffer, BUFFER_SIZE, 0);
	if (result == SOCKET_ERROR) {
		std::cerr << "recv failed: " << WSAGetLastError() << std::endl;
		closesocket(sock);
		WSACleanup();
		return 1;
	}
	buffer[result] = '\0';
	std::cout << buffer << std::endl;
	// 发送 QUIT 命令
	result = send(sock, "QUIT\r\n", 6, 0);
	if (result == SOCKET_ERROR) {
		std::cerr << "send failed: " << WSAGetLastError() << std::endl;
		closesocket(sock);
		WSACleanup();
		return 1;
	}
	// 接收 QUIT 命令响应
	result = recv(sock, buffer, BUFFER_SIZE, 0);
	if (result == SOCKET_ERROR) {
		std::cerr << "recv failed: " << WSAGetLastError() << std::endl;
		closesocket(sock);
		WSACleanup();
		return 1;
	}
	buffer[result] = '\0';
	std::cout << buffer << std::endl;
	closesocket(sock);
	WSACleanup();
	return 0;
 }