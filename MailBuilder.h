#pragma once
#include<string>
#include<vector>
#include<iostream>
#include<fstream>
#include"simple_Base64.h"
#include <sstream>
class MailBuilder
{
public:
	MailBuilder(const std::string& ifrom
		, const std::string& ito
		, const std::string& isubject
		, const std::string& ibody = ""
		, const std::vector<std::string>& iattachments = {});
	~MailBuilder() = default;
	std::string GenerateCmdString();
	void AddAttachment(const std::string& path);
	void AddText(const std::string& input);
private:
	std::string from;
	std::string to;
	std::string subject;
	std::string body;
	std::vector<std::string> attachemnts;
};

