//@@>>ExifDateTime.cpp
// =========== ExifDateTime.cpp ===========
//
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
#include <time.h>
#include <stdio.h>
#include "ExifDateTime.h"
//#include "cport.h"
#include <cctype>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
//#define new DEBUG_NEW
#endif



/* in cport.h
	int gmtime_p(struct tm& tmTime, const time_t* time) {
		int err = 1;
#ifdef _WIN32
		err = gmtime_s(&tmTime, time);
#else
		struct tm tmp;
		struct tm* tmpp;
		tmpp = gmtime_r(time, &tmp);
		if (tmpp == nullptr) {
			err = -1;
		}
		tmTime = *tmpp;

#endif
		return err;
	}


	
	int localtime_p(struct tm& tmTime, const time_t* time)
	{
		int err = 1;
#ifdef _WIN32
		err = localtime_s(&tmTime, time);
#else
		struct tm tmp;
		struct tm* tmpp;
		tmpp = localtime_r(time, &tmp);
		tmTime = *tmpp;
		if (tmpp == nullptr) {
			err = 0;
		}
#endif
		return err;
	}
	*/

	ExifDateTime::ExifDateTime() {
		now();
	}

	ExifDateTime::ExifDateTime(const char* str) {
		m_isOk = true;
		if (fromString(str) == false) {
			m_isOk = false;
		}
		return;
	}

	bool ExifDateTime::fromString(const char* str) {
		if (str == nullptr || str[0] == '\0') {
			return false;
		}
		else {
			std::string datestr = str;
			size_t s = 0;
			size_t e = datestr.find_first_of(":.");
			std::string numstr = datestr.substr(s, e);
			m_year = strtol(numstr.c_str(), NULL, 10);
			s = e + 1;
			e = datestr.find_first_of(":.", s);
			numstr = datestr.substr(s, e - s);
			m_month = strtol(numstr.c_str(), NULL, 10);
			s = e + 1;
			e = datestr.find_first_of(" .", s);
			numstr = datestr.substr(s, e - s);
			m_day = strtol(numstr.c_str(), NULL, 10);
			s = e + 1;
			e = datestr.find_first_of(":.", s);
			numstr = datestr.substr(s, e - s);
			m_hour = strtol(numstr.c_str(), NULL, 10);
			s = e + 1;
			e = datestr.find_first_of(":.", s);
			numstr = datestr.substr(s, e - s);
			m_min = strtol(numstr.c_str(), NULL, 10);
			s = e + 1;
			e = datestr.length();
			numstr = datestr.substr(s, e - s);
			m_sec = strtol(numstr.c_str(), NULL, 10);
			time(&m_timenum);

			struct tm* timeinfo;

		
			timeinfo = std::gmtime(&m_timenum);
			timeinfo->tm_year = m_year - 1900;
			timeinfo->tm_mon = m_month - 1;
			timeinfo->tm_mday = m_day;
			timeinfo->tm_hour = m_hour;
			timeinfo->tm_min = m_min;
			timeinfo->tm_sec = m_sec;
			m_timenum = mktime(timeinfo);

		}
		return true;
	}

	bool is_number(const std::string& s)
	{
		std::string::const_iterator it = s.begin();
		while (it != s.end() && std::isdigit(*it)) ++it;
		return !s.empty() && it == s.end();
	}

	bool ExifDateTime::isExifDateTime(const char* str) {
		if (str == nullptr || str[0] == '\0') {
			return false;
		}

		std::string datestr = str;
		size_t s = 0;
		size_t e = datestr.find_first_of(":.");
		std::string numstr = datestr.substr(s, e);
		if (is_number(numstr) == false) {
			return false;
		}
		s = e + 1;
		e = datestr.find_first_of(":.", s);
		numstr = datestr.substr(s, e - s);
		if (is_number(numstr) == false) {
			return false;
		}
		s = e + 1;
		e = datestr.find_first_of(" .", s);
		numstr = datestr.substr(s, e - s);
		if (is_number(numstr) == false) {
			return false;
		}
		s = e + 1;
		e = datestr.find_first_of(":.", s);
		numstr = datestr.substr(s, e - s);
		if (is_number(numstr) == false) {
			return false;
		}
		s = e + 1;
		e = datestr.find_first_of(":.", s);
		numstr = datestr.substr(s, e - s);
		if (is_number(numstr) == false) {
			return false;
		}
		s = e + 1;
		e = datestr.length();
		numstr = datestr.substr(s, e - s);
		if (is_number(numstr) == false) {
			return false;
		}

		m_isOk = true;
		return true;
	}



	ExifDateTime::~ExifDateTime() {
		// TODO Auto-generated destructor stub
	}

	ExifDateTime& ExifDateTime::operator =(time_t time) {
		setDateTime(time);
		return *this;
	}

	void ExifDateTime::now() {
		time(&m_timenum);
		setDateTime(m_timenum);
	}

	void ExifDateTime::setDateTime(time_t time) {

		struct tm* timeinfo = std::localtime(&time);

		m_year = timeinfo->tm_year + 1900;
		m_month = timeinfo->tm_mon + 1;
		m_day = timeinfo->tm_mday;
		m_hour = timeinfo->tm_hour;
		m_min = timeinfo->tm_min;
		m_sec = timeinfo->tm_sec;
		m_isOk = true;
	}

	std::string ExifDateTime::toString() const {
		std::stringstream tmp;
		tmp << std::setw(4) << std::setfill('0') << m_year << ':';
		tmp << std::setw(2) << std::setfill('0') << m_month << ':';
		tmp << std::setw(2) << std::setfill('0') << m_day << ' ';
		tmp << std::setw(2) << std::setfill('0') << m_hour << ':';
		tmp << std::setw(2) << std::setfill('0') << m_min << ':';
		tmp << std::setw(2) << std::setfill('0') << m_sec;
		return std::string(tmp.str());

	}

	std::string ExifDateTime::current() {
		std::stringstream s;
		s << std::setw(4) << std::setfill('0') << getYear() << '.';
		s << std::setw(2) << std::setfill('0') << getMonth() << '.';
		s << std::setw(2) << std::setfill('0') << getDay() << '.';
		s << std::setw(2) << std::setfill('0') << getHour() << '.';
		s << std::setw(2) << std::setfill('0') << getMin() << '.';
		s << std::setw(2) << std::setfill('0') << getSec();
		return s.str();
		//return "2014/05/21 16:49:00";
	}

	std::string ExifDateTime::toLogString() {
		std::stringstream s;
		s << std::setw(4) << std::setfill('0') << getYear() << '.';
		s << std::setw(2) << std::setfill('0') << getMonth() << '.';
		s << std::setw(2) << std::setfill('0') << getDay() << ' ';
		s << std::setw(2) << std::setfill('0') << getHour() << '.';
		s << std::setw(2) << std::setfill('0') << getMin() << '.';
		s << std::setw(2) << std::setfill('0') << getSec();
		return std::string(s.str());

	}

	bool operator==(ExifDateTime& d1, ExifDateTime& d2)
	{
		if (d1.m_year != d2.m_year) return false;
		if (d1.m_month != d2.m_month) return false;
		if (d1.m_day != d2.m_day) return false;
		if (d1.m_hour != d2.m_hour) return false;
		if (d1.m_min != d2.m_min) return false;
		if (d1.m_sec != d2.m_sec) return false;
		return true;
	}

