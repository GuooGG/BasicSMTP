#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include<vector>
#include <iostream>
#include <WinSock2.h>
#include"socket.h"
#include"MailBuilder.h"
#pragma comment(lib, "Ws2_32.lib")

const std::vector<std::string> DEFAULT_ATTACHMENT_PATHS = {};

class EmailSender
{
public:
	EmailSender(const std::string& senderEmailAdress,
		const std::string& senderEmailPassword,
		const std::string& recipientEmailAdress);
	~EmailSender();
	bool sendEmail(const std::string& subject,
		const std::string& body,
		const std::vector<std::string>& attachmentPath = {});
private:
	bool initialize();
	bool connectsock();
	bool login();
	bool EHLO();
	bool MAIL_FROM();
	bool RCPT_TO();
	//可以右值引用优化吗
	bool sendData(const std::string data);
	bool receiveData();
	std::string base64Encode(const std::string& data);

	std::string m_senderEmailAddress;
	std::string m_senderEmailPassword;
	std::string m_recipientEmailAddress;

	SOCKET m_socket;
	int m_port;
	std::string m_boundary;
	const char* base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	static const int BUFFER_SIZE = 1024;
	char buffer[BUFFER_SIZE];
	std::stringstream CmdBuilder;
	std::string response;
	std::vector<MailBuilder*> Mails;
};

