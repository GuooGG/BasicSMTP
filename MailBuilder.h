#pragma once
#include<string>
#include<vector>
class MailBuilder
{
public:
	MailBuilder(const std::string& ifrom
		        ,const std::string& ito
				,const std::string& isubject
				,const std::vector<std::string>& iattachments);
	~MailBuilder() = default;
	std::string GenerateCmdString();
private:
	const std::string from;
	const std::string to;
	const std::string subject;
	const std::vector<std::string> attachemnts;
};

