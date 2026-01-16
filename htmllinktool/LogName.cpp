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

#include "LogName.h"

#include "ExifDate.h"
#include <stdio.h>
#include <cstdlib>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <chrono>



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
//#define new DEBUG_NEW
#endif



LogName::LogName() {
	// TODO Auto-generated constructor stub

}


int LogName::checkLogSize(const char *logPath, uint64_t maxSize) {
	uintmax_t size = 0;

	std::filesystem::path p = logPath;
	size = std::filesystem::file_size(p);
	
	if (size > maxSize) {
		return 0;
	}
	return 1;
}
std::string LogName::fileTemplate(const char *preName) {
	//CDate& date = CDate::timeNow();
	ExifDate date;
	date.now();
	int year = date.getYear();
	int month = date.getMonth();
	int day = date.getDay();
	//printf("Time: %d %d %d", year, month, day);
	std::string fileMatch;
	if (preName != 0) {
		fileMatch += preName;
	}
	std::stringstream s;
	s << year << std::setw(2) << std::setfill('0') << month
			<< std::setw(2) << std::setfill('0') << day;
	fileMatch += s.str();
	return fileMatch;
}

using FileList = std::vector<std::string>;
using FileList_Ptr = std::unique_ptr<FileList>;

FileList_Ptr getFiles(const char* dirpath)
{
	FileList_Ptr fileList = std::make_unique<FileList>();
	for (auto const& dir_entry : std::filesystem::directory_iterator{ dirpath })
	{
		std::filesystem::path p = dir_entry.path();
		std::filesystem::path f = p.filename();
		std::string fileItem = f.string();
		std::string pathStr = p.string();
		fileList->emplace_back(fileItem);
	}
	return fileList;
}


std::string LogName::makeName(const char *logPath, const char *preName, const char *ext, int maxSize) {

	std::string fileMatch = fileTemplate(preName);
	bool found = false;
	int version = 0;
	std::string filenameItem;
	FileList_Ptr filelist = getFiles(logPath);
	if (filelist == nullptr || filelist->empty() == false) {

		for (std::vector<std::string>::iterator i = filelist->begin(); i != filelist->end(); i++) {
			filenameItem = *i;
			//printf("File \"%s\"\n", filenameItem->c_str());
			std::string tmpExt = filenameItem.substr(filenameItem.find_last_of(".") + 1);
			//printf("Ext \"%s\"\n", tmpExt.c_str());
			if (tmpExt.compare(ext) != 0) {
				continue;
			}

			std::filesystem::path p = filenameItem;
			std::string tmpFileName = p.replace_extension().string();

			std::string nameDate = tmpFileName.substr(0, fileMatch.length());
			//printf("nameDate \"%s\"\n", nameDate.c_str());
			if (nameDate.compare(fileMatch) != 0) {
				continue;
			}
			std::string fileNumber = tmpFileName.substr(fileMatch.length(), tmpFileName.length());
			fileNumber = fileNumber.substr(1, fileNumber.length());
			int thisVer = atoi(fileNumber.c_str());
			if (thisVer > version) {
				version = thisVer;
			}

			//printf("fileNumber \"%s\"\n", fileNumber.c_str());

			found = true;
		}
	}
	
	std::string result;
	std::stringstream s;
	if (found == false) {

		s << fileMatch << "_0001." << ext;
		
	} else {

		s << fileMatch << '_' << std::setw(4) << std::setfill('0') << version << '.' << ext;
		std::string fullpath = logPath;
		fullpath += '/';
		fullpath += s.str().c_str();

		if (maxSize != ALWAYS_CREATE) { // always create new filename.
			int res = checkLogSize(fullpath.c_str(), maxSize * 1024);
			if (res == 1) {
				// less than max size
				result = s.str();
				return result;
			}
		}
		s.str(std::string());
		version++;
		s << fileMatch << '_' << std::setw(4) << std::setfill('0') << version << '.' << ext;
	}
	result = s.str();
	return result;
}

// Date string from filename
std::string LogName::dateString() {

	auto now = std::chrono::system_clock::now();
	auto current_time_t = std::chrono::system_clock::to_time_t(now);
	auto current_time = std::localtime(&current_time_t);

	//printf("Time: %d %d %d", year, month, day);
	std::string dateString;

	std::stringstream s;
	//s << year  << std::setfill('0') << month
	//	<< std::setw(2) << std::setfill('0') << day;

	s << "Current date and time: "
		<< 1900 + current_time->tm_year << std::setw(2)
		<< 1 + current_time->tm_mon << std::setw(2) << std::setfill('0') << current_time->tm_mday;

	dateString += s.str();
	return dateString;
}

// todays date string
std::string LogName::dateStringToday() {
	
	return dateString();
}

