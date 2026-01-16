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

#ifndef LOGNAME_H_
#define LOGNAME_H_
#include <string>



/**
 * @brief This class returns a logging file name based on the date and the size of the
 * current log file.
 * If new names are needed then a number is added to the end of the log name.
 * A prefix and extension can be passed to complete the required file name.
 */
class LogName {
	std::string fileTemplate(const char *preName);
	int checkLogSize(const char *logPath, uint64_t maxSize);
	std::string m_filename;
public:
	/// Constructor
	LogName();
	/// Destructor
	virtual ~LogName() = default;
	/// @brief Makes the log file name base on size and date of previous log files in the directory
	/// passed.
	/// @param logPath - path to look for previous logs.
	/// @param preName - Prefixed name given to the log file.
	/// @param ext - Extension given to the log file.
	/// @param maxSize - max size of current log file before new log name is created.
	std::string makeName(const char *logPath, const char *preName, const char *ext, int maxSize);
	/// Date string from filename
	/// Make date string
	static std::string dateString();
	/// Make todays date string
	static std::string dateStringToday();
	std::string &getFilename() {
		return m_filename;
	}
	static const int ALWAYS_CREATE = -1;
};

#endif /* LOGNAME_H_ */
