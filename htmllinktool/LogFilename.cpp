/* **************************************************
**
**    III                DDD  KKK
**    III                DDD  KKK
**                       DDD  KKK
**    III   DDDDDDDDDDD  DDD  KKK            KKK
**    III  DDD           DDD  KKK            KKK
**    III  DDD           DDD  KKK           KKK
**    III  DDD           DDD  KKK        KKKKKK
**    III  DDD           DDD  KKK   KKKKKKKKK
**    III  DDD           DDD  KKK        KKKKKK
**    III  DDD           DDD  KKK           KKK
**    III  DDD           DDD  KKK            KKK
**    III   DDDDDDDDDDDDDDDD  KKK            KKK
**
**
**     SSS         FF
**    S           F   T
**     SSS   OO   FF  TTT W   W  AAA  R RR   EEE
**        S O  O  F   T   W W W  AAAA RR  R EEEEE
**    S   S O  O  F   T   W W W A   A R     E
**     SSS   OO  FFF   TT  W W   AAAA R      EEE
**
**    Copyright: (c) 2015 IDK Software Ltd
**
****************************************************
**
**	Filename	: CRegString.cpp
**	Author		: I.Ferguson
**	Version		: 1.000
**	Date		: 26-05-2015
**
** #$$@@$$# */

#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <cstdlib>
#include <stdlib.h>
#include <sstream>
#include <chrono>
#include <filesystem>
//#include "CIDKFileFind.h"
#include "LogFilename.h"
//#include "SAUtils.h"
#include "CIDKFileFind.h"
#include "ExifDate.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
//#define new DEBUG_NEW
#endif



LogFilename::LogFilename(const char *logfilePath) {

	m_logfilePath = logfilePath;
	m_maxSize = 256;
}

LogFilename::~LogFilename() {

}

time_t toDate(const char *str) {
	std::string datestr = str;
	size_t s = 0;
	size_t e = datestr.find_first_of('-');
	std::string numstr = datestr.substr(s, e);
	int m_year = strtol(numstr.c_str(), NULL, 10);
	s = e + 1;
	e = datestr.find_first_of('-', s);
	numstr = datestr.substr(s, e - s);
	int m_month = strtol(numstr.c_str(), NULL, 10);
	s = e + 1;
	e = datestr.length();
	numstr = datestr.substr(s, e - s);
	int m_day = strtol(numstr.c_str(), NULL, 10);
	time_t m_timenum;
	time(&m_timenum);
	//struct tm timeinfo;

	auto timeinfo = std::localtime(&m_timenum);
	
	timeinfo->tm_year = m_year - 1900;
	timeinfo->tm_mon = m_month - 1;
	timeinfo->tm_mday = m_day;
	m_timenum = mktime(timeinfo);
	return m_timenum;
}

using FileList = std::vector<std::string>;
using FileList_Ptr = std::unique_ptr<FileList>;

std::string LogFilename::lastfile() {

	int m_ver = 0;
	//DIR *dir;
	//struct dirent *ent;
	std::string seachpath = m_logfilePath;
	seachpath += "/*.log";

	FileList_Ptr fileList = std::make_unique<FileList>();
	for (auto const& dir_entry : std::filesystem::directory_iterator{ m_logfilePath })
	{
		std::filesystem::path p = dir_entry.path();
		std::filesystem::path f = p.filename();
		std::string fileItem = f.string();
		std::string pathStr = p.string();
		fileList->emplace_back(fileItem);
	}

	std::string nFilename;
	for (std::string entry : *fileList)
	{
		
		ExifDate dateToday;
		std::string ext = entry.substr(entry.find_last_of(".") + 1);
		if (ext.compare(".log") == 0)
		{
			std::filesystem::path p = entry;
			std::string tmp = p.replace_extension().string();
			size_t pos = tmp.find_first_of("+");
			std::string dateOnly = tmp.substr(0, pos);
			std::string verOnly = tmp.substr(pos + 1, tmp.length() - pos);
			ExifDate exifDate(toDate(dateOnly.c_str()));
			//printf("%s %d %s\n", exifDate.toString().c_str(), exifDate.getTime(), verOnly.c_str());
			if (dateToday.equals(exifDate)) {
				int nver = strtol(verOnly.c_str(), NULL, 10);
				if (nver > m_ver) {
					m_ver = nver;
				}
				nFilename = entry;
				printf("Newest = %s\n", nFilename.c_str());
			}
		}
	}
	return nFilename;
}
const std::string LogFilename::filepath() {
	ExifDate exifDate;
	std::stringstream tmp;
	tmp << exifDate.getYear() << '-' << exifDate.getMonth() << '-' << exifDate.getDay();
	std::string filename = tmp.str();
	std::string lastFilename = lastfile();
	std::string buf;
	if (m_ver > 0) {
		buf = std::to_string(m_ver);
		std::string path = m_logfilePath + '/' + filename + buf;
		path = path + ".log";
		return path;
	}
	std::string path = m_logfilePath + '/' + filename + "+1.log";
	return path;
}

