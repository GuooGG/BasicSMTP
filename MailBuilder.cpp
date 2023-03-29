#include "MailBuilder.h"

MailBuilder::MailBuilder(const std::string& ifrom
	, const std::string& ito
	, const std::string& isubject
	, const std::string& ibody
	, const std::vector<std::string>& iattachments)
	:from(ifrom)
	,to(ito)
	,subject(isubject)
	,attachemnts(iattachments)
	,body(ibody)
{
}
std::string MailBuilder::GenerateCmdString()
{
	std::stringstream ss;
	//首先构造头部
	ss << "From: " << from << "\r\n";
	ss << "To: " << to << "\r\n";
	ss << "Subject: " << subject << "\r\n";
	ss << "MIME-Version: 1.0\r\n";
	ss << "Content-Type: multipart/mixed; boundary=boundary_1234\r\n\r\n";
	//构造邮件内容
	ss << "--boundary_1234\r\n";
	ss << "Content-Type: text/plain; charset=UTF-8\r\n\r\n";
	ss << body << "\r\n\r\n";
	//构造附件
	for (auto path : attachemnts) {
		std::ifstream attachmentFile(path);
		if (!attachmentFile.is_open()) {
			std::cerr << "Can't open file: " << path << std::endl;
			continue;
		}
		std::stringstream ssAttachment;
		ssAttachment << attachmentFile.rdbuf();
		attachmentFile.close();
		ss << "--boundary_1234\r\n";
		ss << "Content-Type: text/plain; charset=UTF-8\r\n";
		ss << "Content-Transfer-Encoding: base64\r\n";
		ss << "Content-Disposition: attachment; filename=";
		ss << "\"" << path << "\"" << "\r\n\r\n";
		ss << base64_encode(ssAttachment.str().c_str());
		ss << "\r\n";
	}
	ss << "--boundary_1234--\r\n\r\n";
	ss << "\r\n.\r\n";
	return ss.str();
}
void MailBuilder::AddAttachment(const std::string& path)
{
	attachemnts.emplace_back(path);
}

void MailBuilder::AddText(const std::string& input)
{
	body += input;
}
