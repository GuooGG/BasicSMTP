#include "MailBuilder.h"

MailBuilder::MailBuilder(const std::string& ifrom
	, const std::string& ito
	, const std::string& isubject
	, const std::vector<std::string>& iattachments)
	:from(ifrom)
	,to(ito)
	,subject(isubject)
	,attachemnts(iattachments)
{
}
