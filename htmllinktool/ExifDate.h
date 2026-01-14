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

#ifndef EXIFDATE_H_
#define EXIFDATE_H_
#include <time.h>
#include <string>
#include "ExifDateTime.h"
//#include "cport.h"



class ExifDate {
	int m_day;
	int m_month;
	int m_year;
	bool m_isOk;
	time_t m_timenum;
public:
	ExifDate();
	ExifDate(time_t time);
	ExifDate(const char *datestr);
	ExifDate(int y, int m, int d);
	ExifDate(const ExifDate &exifDate) {
		m_day = exifDate.m_day;
		m_month = exifDate.m_month;
		m_year = exifDate.m_year;
		m_isOk = exifDate.m_isOk;
		time(&m_timenum);
		struct tm* timeinfo = gmtime(&m_timenum);
		timeinfo->tm_year = m_year - 1900;
		timeinfo->tm_mon = m_month - 1;
		timeinfo->tm_mday = m_day;
		m_timenum = mktime(timeinfo);
		m_isOk = true;
	}
	ExifDate(const ExifDateTime &exifDate) {
		m_day = exifDate.getDay();
		m_month = exifDate.getMonth();
		m_year = exifDate.getYear();
		m_isOk = exifDate.isOk();
		
		time(&m_timenum);
		
		struct tm* timeinfo = gmtime(&m_timenum);
		timeinfo->tm_year = m_year - 1900;
		timeinfo->tm_mon = m_month - 1;
		timeinfo->tm_mday = m_day;
		m_timenum = mktime(timeinfo);
		
	}

	//ExifDate& operator=(ExifDate& d);
	//ExifDate& operator=(ExifDateTime& d);

	ExifDate &operator=(const ExifDate &exifDate) {
		m_day = exifDate.m_day;
		m_month = exifDate.m_month;
		m_year = exifDate.m_year;
		m_isOk = exifDate.m_isOk;
		time(&m_timenum);
		struct tm* timeinfo = gmtime(&m_timenum);
		timeinfo->tm_year = m_year - 1900;
		timeinfo->tm_mon = m_month - 1;
		timeinfo->tm_mday = m_day;
		m_timenum = mktime(timeinfo);
		m_isOk = true;
		return *this;
	}

	ExifDate &operator=(const ExifDateTime &exifDate) {
		m_day = exifDate.m_day;
		m_month = exifDate.m_month;
		m_year = exifDate.m_year;
		m_isOk = exifDate.m_isOk;
		time(&m_timenum);
		struct tm* timeinfo = gmtime(&m_timenum);
		timeinfo->tm_year = m_year - 1900;
		timeinfo->tm_mon = m_month - 1;
		timeinfo->tm_mday = m_day;
		m_timenum = mktime(timeinfo);
		m_isOk = true;
		return *this;
	}

	bool equals(const ExifDate& exifDate) const {
		if (m_day != exifDate.m_day) return false;
		if (m_month != exifDate.m_month) return false;
		if (m_year != exifDate.m_year) return false;
		return true;
	}

	bool operator==(const ExifDate &exifDate) {
		return equals(exifDate);
	}

	
	virtual ~ExifDate();

	std::string toString();
	std::string toFileString();
	std::string toShortRelativePath();
	void now();

	int getDay() const {
		return m_day;
	}

	int getMonth() const {
		return m_month;
	}

	int getYear() const {
		return m_year;
	}

	bool isOk() const {
		return m_isOk;
	}

	time_t getTime() const {
		return m_timenum;
	}

};

#endif /* EXIFDATE_H_ */
