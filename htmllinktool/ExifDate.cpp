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

#include <string>
#include <cstdlib>
#include <sstream>
#include <iomanip>
#include <stdio.h>
#include "ExifDate.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
//#define new DEBUG_NEW
#endif



ExifDate::ExifDate() {
	now();
}

ExifDate::ExifDate(const char *str) {
	m_isOk = true;
	std::string datestr = str;
	size_t s = 0;
	size_t e = datestr.find_first_of(':');
	if (e != 4) {
		now();
		m_isOk = false;
		return;
	}
	std::string numstr = datestr.substr(s, e);
	m_year = strtol(numstr.c_str(), NULL, 10);
	s = e + 1;
	e = datestr.find_first_of(':', s);
	numstr = datestr.substr(s, e-s);
	m_month = strtol(numstr.c_str(), NULL, 10);
	s = e + 1;
	e = datestr.length();
	numstr = datestr.substr(s, e-s);
	m_day = strtol(numstr.c_str(), NULL, 10);
	time(&m_timenum);
	struct tm* timeinfo = std::gmtime(&m_timenum);
	timeinfo->tm_year = m_year - 1900;
	timeinfo->tm_mon = m_month - 1;
	timeinfo->tm_mday = m_day;
	m_timenum = mktime(timeinfo);
	m_isOk = true;
}

ExifDate::ExifDate(int y, int m, int d) {

	struct tm timeinfo;

	m_year = y;
	timeinfo.tm_year = y - 1900;
	m_month = m;
	timeinfo.tm_mon = m - 1;
	m_day = d;
	timeinfo.tm_mday = d;
	m_timenum = mktime(&timeinfo);
	m_isOk = true;
}

ExifDate::ExifDate(time_t time) {
	m_timenum = time;
	struct tm* timeinfo = std::localtime(&m_timenum);

	m_year = timeinfo->tm_year + 1900;
	m_month = timeinfo->tm_mon + 1;
	m_day = timeinfo->tm_mday;
	
	m_isOk = true;
}

ExifDate::~ExifDate() {
	// TODO Auto-generated destructor stub
}

void ExifDate::now() {
	time(&m_timenum);
	struct tm* timeinfo = std::localtime(&m_timenum);
	m_year = timeinfo->tm_year + 1900;
	m_month = timeinfo->tm_mon + 1;
	m_day = timeinfo->tm_mday;

	m_isOk = true;
}

std::string ExifDate::toString() {
	std::stringstream tmp;
	tmp << m_year << ':' << m_month << ':' << m_day;

	return std::string(tmp.str());

}

std::string ExifDate::toFileString() {
	std::stringstream s;
	s << std::setw(4) << std::setfill('0') << getYear() << '.';
	s << std::setw(2) << std::setfill('0') << getMonth() << '.';
	s << std::setw(2) << std::setfill('0') << getDay();
	return std::string(s.str());

}

std::string ExifDate::toShortRelativePath() {
	std::stringstream s;
	s << std::setw(4) << std::setfill('0') << getYear() << '-';
	s << std::setw(2) << std::setfill('0') << getMonth() << '-';
	s << std::setw(2) << std::setfill('0') << getDay();
	return std::string(s.str());

}


