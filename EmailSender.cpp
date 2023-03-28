#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "EmailSender.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#include"socket.h"
const std::string EMAIL_SEVER_ADRESS = "smtp.qq.com";
const int EMAIL_SEVER_PORT = 25;

const std::string SENDER_EMAIL_ADDRESS = "2311619185@qq.com";
const std::string SENDER_EMAIL_PASSWORD = "sqegtmfvtbtpdjae";


EmailSender::EmailSender(const std::string& senderEmailAddress,
    const std::string& senderEmailPassword,
    const std::string& recipientEmailAddress)
    : m_senderEmailAddress(senderEmailAddress)
    , m_senderEmailPassword(senderEmailPassword)
    , m_recipientEmailAddress(recipientEmailAddress)
    , m_boundary("1234567890abcdef")
    , m_port(25)
    , m_socket(0)
    , buffer("")
{
    
}
EmailSender::~EmailSender() {
    closesocket(m_socket);
    WSACleanup();
}

bool EmailSender::sendEmail(const std::string& subject,
    const std::string& body,
    const std::vector<std::string>& attachmentPath)
{
    if (!initialize()) {
        return false;
    }
    if (!connectsock()) {
        return false;
    }
    if (!EHLO()) {
        return false;
    }
    if (!login()) {
        return false;
    }
    if (!MAIL_FROM()) {
        return false;
    }
    if (!RCPT_TO()) {
        return false;
    }
    //发送DATA命令
    if (!sendData("DATA\r\n")) {
        return false;
    }
    if (!receiveData()) {
        return false;
    }

    //构造邮件头部
    std::string from = m_senderEmailAddress;
    std::string to = m_recipientEmailAddress;
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
        ssBody << base64Encode((ssAttachment.str().c_str()));
        ssBody << "\r\n";
    }
    ssBody << "--boundary_1234--\r\n\r\n";
    // 发送邮件头部
    int result = send(m_socket, ssHeader.str().c_str(), ssHeader.str().length(), 0);
    if (result == SOCKET_ERROR) {
        std::cerr << "send failed: " << WSAGetLastError() << std::endl;
        closesocket(m_socket);
        WSACleanup();
        return 1;
    }
    // 发送邮件内容
    result = send(m_socket, ssBody.str().c_str(), ssBody.str().length(), 0);
    if (result == SOCKET_ERROR) {
        std::cerr << "send failed: " << WSAGetLastError() << std::endl;
        closesocket(m_socket);
        WSACleanup();
        return 1;
    }
    //发送结束标识符
    result = send(m_socket, "\r\n.\r\n", 5, 0);
    if (result == SOCKET_ERROR) {
        std::cerr << "send failed: " << WSAGetLastError() << std::endl;
        closesocket(m_socket);
        WSACleanup();
        return 1;
    }
    //接收发送邮件的响应
    std::string response;
    char buffer[1025];
    result = recv(m_socket,buffer,1024, 0);
    if (result == SOCKET_ERROR) {
        std::cerr << "recv failed: " << WSAGetLastError() << std::endl;
        closesocket(m_socket);
        WSACleanup();
        return 1;
    }
    buffer[result] = '\0';
    std::cout << buffer << std::endl;
    //Close socket
    closesocket(m_socket);
    return true;
}

bool EmailSender::initialize()
{
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        return false;
    }
    return true;
}

bool EmailSender::connectsock()
{
    //连接到SMTP服务器
    m_socket = connecttcp(EMAIL_SEVER_ADRESS.c_str(), "smtp");
    //接收服务器欢迎消息
    if (!receiveData()) {
        return false;
    }
    return true;
}

bool EmailSender::login()
{
    std::string command = "AUTH LOGIN\r\n";
    if (!sendData(command)) {
        return false;
    }
    if (!receiveData()) {
        return false;
    }
    std::string username_b64 = base64Encode(m_senderEmailAddress);
    std::string password_b64 = base64Encode(m_senderEmailPassword);
    command = username_b64 + "\r\n";
    if (!sendData(command)) {
        return false;
    }
    if (!receiveData()) {
        return false;
    }
    command = password_b64 + "\r\n";
    if (!sendData(command)) {
        return false;
    }
    if (!receiveData()) {
        return false;
    }
    return true;
}
bool EmailSender::EHLO()
{
    if (!sendData("EHLO localhost\r\n")) {
        return false;
    }
    if (!receiveData()) {
        return false;
    }
    return true;
}
bool EmailSender::MAIL_FROM()
{
    //发送MAIL FROM命令并接收服务器响应
    CmdBuilder.clear();
    CmdBuilder << "MAIL FROM: <" << m_senderEmailAddress << ">\r\n";
    if (!sendData(CmdBuilder.str())) {
        return false;
    }
    if (!receiveData()) {
        return false;
    }
    return true;
}
bool EmailSender::RCPT_TO()
{
    //发送RCPT TO命令
    CmdBuilder.clear();
    CmdBuilder << "RCPT TO: <" << m_recipientEmailAddress << ">\r\n";
    if (!sendData(CmdBuilder.str())) {
        return false;
    }
    if (!receiveData()) {
        return false;
    }
    return true;
}
bool EmailSender::sendData(const std::string data)
{
    if (data.empty()) {
        std::cerr << "Error sending empty data" << std::endl;
        return false;
    }

    size_t totalSent = 0;
    const char* pData = data.c_str();
    const size_t dataSize = data.size();

    while (totalSent < dataSize) {
        const size_t sent = send(m_socket, pData + totalSent, dataSize - totalSent, 0);
        if (sent == SOCKET_ERROR) {
            std::cerr << "Error sending data: " << WSAGetLastError() << std::endl;
            return false;
        }
        totalSent += sent;
    }

    return true;
}

bool EmailSender::receiveData()
{
    int bytesReceived = 0;
    while ((bytesReceived = recv(m_socket, buffer, BUFFER_SIZE-1, 0)) > 0)
    {
        response.append(buffer, bytesReceived);
        if (bytesReceived < sizeof(buffer))
        {
            break;
        }
    }

    if (bytesReceived == -1)
    {
        std::cerr << "Error receiving data from server" << std::endl;
        return false;
    }
    std::cout << response << std::endl;
    response.clear();
    return true;
}

std::string EmailSender::base64Encode(const std::string& s)
{

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
