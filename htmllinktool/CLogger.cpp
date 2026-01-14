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

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstdarg>
#include <algorithm>
#include <cctype>
#include <functional>
#include <cctype>
#include <locale>

#include "CLogger.h"
#include "ExifDateTime.h"
#include "LogName.h"
// UDP Not used
// #include "UDPOut.h"

//#include "LogName.h"
//#include "ErrorCode.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
//#define new DEBUG_NEW
#endif

std::string CLogger::m_filename = "Log.txt";
std::unique_ptr<CLogger> CLogger::m_this(nullptr);
std::ofstream CLogger::m_logfile;
CLogger::Level CLogger::m_logLevel = CLogger::Level::SUMMARY;
CLogger::Level CLogger::m_consoleLevel = CLogger::Level::SUMMARY;
CLogger::Level CLogger::m_lastLevel = CLogger::Level::UNKNOWN;

std::string CLogger::m_logpath;
int CLogger::m_size = 10000;
int CLogger::m_cursize = 0;
bool CLogger::m_isQuiet = false;
bool CLogger::m_isSilent = false;
bool CLogger::m_isOpen = false;
int CLogger::m_lastCode;
std::string CLogger::m_lastMessage;
std::string CLogger::m_currentMessage;


std::string CLogger::m_appName;
std::unique_ptr<LogBuffer> CLogger::m_startUpBuffer{nullptr};

class LoggBuffer : public std::vector<std::string> {

};

CLogger::CLogger() {
	m_startUpBuffer = std::make_unique<LogBuffer>();
}

CLogger &CLogger::getLogger() {
	static CLogger thisLogger;
	return thisLogger;
}

CLogger::~CLogger() {
	m_logfile.close();
	m_isOpen = false;
	
}



// trim from start (in place)
static inline void ltrim(std::string& s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
		return !std::isspace(ch);
		}));
}

// trim from end (in place)
static inline void rtrim(std::string& s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
		return !std::isspace(ch);
		}).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string& s) {
	ltrim(s);
	rtrim(s);
}

// trim from start (copying)
static inline std::string ltrim_copy(std::string s) {
	ltrim(s);
	return s;
}

// trim from end (copying)
static inline std::string rtrim_copy(std::string s) {
	rtrim(s);
	return s;
}

// trim from both ends (copying)
static inline std::string trim_copy(std::string s) {
	trim(s);
	return s;
}

/*
static inline void ltrim(std::string &s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(),
		std::not1(std::ptr_fun<int, int>(std::isspace))));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
	s.erase(std::find_if(s.rbegin(), s.rend(),
		std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
	ltrim(s);
	rtrim(s);
}
*/

CLogger::Level CLogger::messageLevel(std::string message) {
	size_t spos = message.find("[");
	size_t epos = message.find("]", spos + 1);
	std::string levelStr = message.substr(spos+1, (epos-spos)-1);
	trim(levelStr);
	return toLevel(levelStr);
	
}

void CLogger::setLogPath(const char *logpath) {
	m_logpath = logpath;
}

void CLogger::startLogging() {
	m_isOpen = true;
	makeFile(m_appName.c_str());
	processBuffer();
}

void CLogger::processBuffer() {

	if (m_startUpBuffer == nullptr)
	{
		return;
	}

	for (auto i = m_startUpBuffer->begin(); i != m_startUpBuffer->end(); i++) {
		
		m_logfile << *i;
	
		if (m_isSilent == false) {
			std::string message = *i;
			CLogger::Level level = messageLevel(*i);
			if (IsConsoleOut(level)) {
				if (m_isQuiet) {
					std::cout << message << '\n';
				}
				
			}
			// UDP Not used
			// UDPOut::out(message.c_str());
		}
		
	}
}

void CLogger::makeFile(const std::string& appName) {
	LogName logName;
	if (m_isOpen == false) {
		return;
	}
	m_filename = logName.makeName(m_logpath.c_str(), appName.c_str(), "log", 256);
	std::string fullpath = m_logpath;
	fullpath += '/';
	fullpath += m_filename;
	m_logfile.open(fullpath.c_str(), std::ios::out | std::ios::app);
	if (m_logfile.is_open() == false) {  // changed to true for testing
		//printf("cannot open log file %s", fullpath.c_str());
		throw std::exception("Cannot open log file");
	}
	//printf("Opened log file %s", fullpath.c_str());
}

ExifDateTime last;
int count = 0;

/**
This is for status leval messages only. They will be output as status level messaes with an
extra status code defining the status of the last opertion. For example if an operation to import an
image failed as the image already is in the achive then this will e recorded in the status message but also
recorded with the ReporterEvent
*/

void CLogger::status(int code, ReporterEvent::Status level, const char *format, ...) {
	if (m_size < m_cursize) {
		m_logfile.close();
		makeFile(m_appName);
		m_cursize = 0;
	}
	

	ExifDateTime date;
	date.now();
	if (last.getTime() == date.getTime()) {
		count++;
	}
	else {
		count = 1;
	}
	last.now();
	char message[1024];
	try {

		va_list args;
		va_start(args, format);
#ifdef _WIN32
		vsprintf_s(message, format, args);
#else
		vsprintf(message, format, args);
#endif
		va_end(args);
	}
	catch (std::exception e) {
		printf("logger crashed parsing message");
		exit(-1);
	}
	try {
		m_lastMessage = message;
		m_lastCode = code;
		std::stringstream logstr;
		logstr << "\n" << std::setfill('0') << std::setw(6) << code << ": " << date.toLogString() << '.' << count << "\t";
		logstr << '[' << levelStr(CLogger::Level::STATUS) << "]\t";
		logstr << message;
		// Return if the message is to low leval to be include in the log, UDP or terminal.
		if (!IsLogOut(CLogger::Level::STATUS) && m_isOpen) {
			return;
		}
		if (m_isOpen) {
			if (IsLogOut(CLogger::Level::STATUS)) {
				m_logfile << logstr.str();
			}
		}
		else {
			m_startUpBuffer->push_back(logstr.str());
		}
		m_cursize++;

		if (m_isSilent == false) {
			if (IsConsoleOut(CLogger::Level::STATUS)) {
				if (m_isQuiet) {
					std::cout << message << '\n';
				}
				else {
					// No codes in console output
					//std::cout << "\n"; // << setfill('0') << setw(6) << code << '\t';
					std::cout << '[' << levelStr(CLogger::Level::STATUS) << "]\t" << message << " - " << ReporterEvent::statusString(level) << " " << std::endl;
				}
			}
		}

		std::stringstream strudp;
		strudp << std::setfill('0') << std::setw(6) << code << ":" << message;
		std::string udpMessage = strudp.str();

		// UDP Not used
		// UDPOut::out(udpMessage.c_str());

		StatusReporter& statusReporter = StatusReporter::get();
		std::string finalStr = message;
		statusReporter.add(level, finalStr);

	}
	catch (std::exception e) {
		printf("logger crashed");
		exit(-1);
	}
}

bool CLogger::setHighestLevel(Level level) {
	
	if (level > m_lastLevel)
	{
		m_lastLevel = level;
		return true;
	}
	return false;
}

int CLogger::returnCode()
{
	if (m_lastLevel == Level::UNKNOWN) {
		return -1;
	} else if (m_lastLevel <= Level::SUMMARY) {
		return 0;
	} else if (m_lastLevel == Level::WARNING) {
		return 1;
	} else if (m_lastLevel <= Level::ERR) {
		return 2;
	}
	return 3;
}

void CLogger::log(int code, Level level, const char* format, ...) {
	std::string message;
	try {
		size_t final_n, n = (strlen(format) * 2); // Reserve two times as much as the length of the fmt_str //
		std::string str;
		std::unique_ptr<char[]> formatted;
		va_list ap;
		while (1) {
			//formatted.reset(new char[n]); // Wrap the plain char array into the unique_ptr
			auto formatted = std::make_unique<char[]>(n);
			//strcpy(formatted.get(), format);
			va_start(ap, format);
			final_n = vsnprintf(formatted.get(), n, format, ap);
			va_end(ap);
			if (final_n < 0 || final_n >= n) {
				n += abs((long)(final_n - n + 1));
			}
			else {
				message = formatted.get();
				break;
			}
		}

	}
	catch (const std::exception e) {
		printf("logger crashed parsing message");
		exit(-1);
	}

	log(code, level, message);
}

void CLogger::log(int code, Level level, const std::string& message) {
	
	if (m_size < m_cursize) {
		m_logfile.close();
		makeFile(m_appName);
		m_cursize = 0;
	}
	// This is used to set the return level 
	if (setHighestLevel(level) == true) {

		//std::stringstream rm;
		//rm << '[' << levelStr(level) << "]\t";
		//rm << message << std::endl;

	}
	
	ExifDateTime date;
	date.now();
	if (last.getTime() == date.getTime()) {
		count++;
	}
	else {
		count = 1;
	}
	last.now();
	//std::string message;
	
	try {

		if (IsLogOut(level)) {
			CLogger::m_lastMessage = message;
			m_lastCode = code;
			std::stringstream logstr;
			logstr << "\n" << std::setfill('0') << std::setw(6) << code << ": " << date.toLogString() << '.' << count << "\t";
			logstr << '[' << levelStr(level) << "]\t";
			logstr << message;
			if (m_isOpen) {
				m_logfile << logstr.str();
			}
			else {
				m_startUpBuffer->push_back(logstr.str());
			}
			m_cursize++;
		}
		if (m_isSilent == false) {
			if (IsConsoleOut(level)) {
				if (!m_isQuiet) { // if isQuiet is false then send out the log message
					std::cout << '[' << levelStr(level) << "]\t";
					std::cout << message << std::endl;

				}
				else {
					//std::cout << std::endl; // << setfill('0') << setw(6) << code << ": " << date.toLogString() << '.' << count << "\t";
					//std::cout << '[' << levelStr(level) << "]\t";
					//std::cout << message;
				}
			}
		}


		/*
		if (UDPOut::isEnabled()) {
			if (IsConsoleOut(level)) {
				std::stringstream strudp;
				strudp << std::setfill('0') << std::setw(6) << code << "@" << date.toLogTimeString() << "@" << toString(level) << "@" << message;
				const std::string udpMessage = strudp.str();
				UDPOut::out(strudp.str().c_str());
			}
		}
		*/
		if (level == Level::STATUS) {

		}
	}
	catch (const std::exception e) {
		printf("logger crashed");
		exit(-1);
	}
	
}

/*
void CLogger::Log(Level level, const char *format, ...) {
	CLogger &Logger = getLogger();

	char message[256];
	va_list args;
	va_start(args, format);
	vsprintf(message, format, args);
	va_end(args);
	Logger.Log(level, message);
}
*/



// UDP Not used
/*
CLogger& CLogger::operator << (const std::string& message) {
	ExifDateTime date;
	date.now();
	m_logfile << "\n" << date.toLogString() << ":\t";
	m_logfile << message;
	UDPOut::out(message.c_str());
	return *this;
}
*/

inline bool CLogger::IsLogOut(Level level) {
	if (level >= m_logLevel) {
		return true;
	}
	return false;
}

inline bool CLogger::IsConsoleOut(Level level) {
	if (level >= m_consoleLevel) {
		return true;
	}
	return false;
}

const char *CLogger::toString(CLogger::Level level) {
	switch (level) {
	case CLogger::Level::TRACE: return    "TRACE";
	case CLogger::Level::FINE: return     "FINE";
	case CLogger::Level::INFO: return     "INFO";
	case CLogger::Level::STATUS: return   "STATUS";
	case CLogger::Level::SUMMARY: return  "SUMMARY";
	case CLogger::Level::WARNING: return  "WARNING";
	case CLogger::Level::ERR: return      "ERROR";
	case CLogger::Level::FATAL: return    "FATAL";
	case CLogger::Level::UNKNOWN: return  "FATAL";
	}
	return "FATAL";
}

const char *CLogger::levelStr(CLogger::Level level) {
	switch (level) {
	case CLogger::Level::TRACE: return    "      TRACE";
	case CLogger::Level::FINE: return     "      FINE ";
	case CLogger::Level::INFO: return     "     INFO  ";
	case CLogger::Level::STATUS: return   "    STATUS ";
	case CLogger::Level::SUMMARY: return  "   SUMMARY ";
	case CLogger::Level::WARNING: return  "  WARNING  ";
	case CLogger::Level::ERR: return      " ERROR     ";
	case CLogger::Level::FATAL: return    "FATAL      ";
	case CLogger::Level::UNKNOWN: return  "FATAL      ";
	}
	return "FATAL";
}

CLogger::Level CLogger::toLevel(const std::string &s) {
	CLogger::Level level;
	if (s.compare("TRACE") == 0) {
		level = Level::TRACE;
	}
	else if (s.compare("FINE") == 0) {
		level = Level::FINE;
	}
	else if (s.compare("INFO") == 0) {
		level = Level::INFO;
	}
	else if (s.compare("STATUS") == 0) {
		level = Level::STATUS;
	}
	else if (s.compare("SUMMARY") == 0) {
		level = Level::SUMMARY;
	}
	else if (s.compare("WARNING") == 0) {
		level = Level::WARNING;
	}
	else if (s.compare("ERROR") == 0) {
		level = Level::ERR;
	}
	else if (s.compare("FATAL") == 0) {
		level = Level::FATAL;
	} else {
		level = Level::UNKNOWN;
	}
	return level;
}


bool CLogger::setLevel(CLogger::Level &level, const std::string &s) {

	level = toLevel(s);
	if (level == Level::UNKNOWN) {
		return false;
	}
	
	return true;
}

ReporterEvent::ReporterEvent(ReporterEvent::Status status, std::string & message) : m_message(message), m_status(status) {}

StatusReporter& StatusReporter::get()
{
	static StatusReporter statusReporter;
	return statusReporter;
}

void StatusReporter::add(ReporterEvent::Status status, const char * fmt, ...)
{
	char message[1024];
	try {

		va_list args;
		va_start(args, fmt);
#ifdef _WIN32
		vsprintf_s(message, fmt, args);
#else
		vsprintf(message, fmt, args);
#endif
		va_end(args);
	}
	catch (std::exception e) {
		printf("logger crashed parsing message");
		exit(-1);
	}
	std::string finalStr = message;
	add(status, finalStr);

}

void StatusReporter::add(ReporterEvent::Status status, std::string &msg) {

	m_list->push_back(ReporterEvent(status, msg));
}

const char * ReporterEvent::statusString(ReporterEvent::Status status)
{
	switch (status) {
	case ReporterEvent::Status::Completed: return	"Completed ";
	case ReporterEvent::Status::Warning: return		"Warning   ";
	case ReporterEvent::Status::Error: return		"Error     ";
	case ReporterEvent::Status::Fatal: return		"Fatal     ";
	}
	return											"Unknown   ";
}

const char * ReporterEvent::statusString()
{
	return statusString(m_status);
}

ReporterEvent SummaryReporter::doProcess()
{

	
	for (auto i = m_list->begin(); i != m_list->end(); i++) {
		ReporterEvent& item = *i;
		//str << item.statusString() << item.m_message << '\n';
		switch (item.m_status) {
		case ReporterEvent::Status::Completed: m_completed++; break;
		case ReporterEvent::Status::Warning: m_warning++; break;
		case ReporterEvent::Status::Error: m_error++; break;
		case ReporterEvent::Status::Fatal: m_fatal++; break;
		case ReporterEvent::Status::Unkown: m_unknown++; break;
		}
	}
	std::stringstream strSummary;
	std::stringstream strResult;
	ReporterEvent::Status status = ReporterEvent::Status::Unkown;
	strResult << "Result: ";

	if (m_fatal != 0) {
		strResult << "Not completed, Fatal error encountered";
		status = ReporterEvent::Status::Fatal;
	}
	else if (m_error != 0) {
		strResult << "Completed with errors, some operations may not have been completed";
		status = ReporterEvent::Status::Error;
		strSummary << "Summary:";
		if (m_error != 0) strSummary << "  Errors:     " << m_error;
		if (m_warning != 0) strSummary << "  Warnings:   " << m_warning;
	}
	else if (m_warning != 0) {
		strResult << "Completed with warnings, some operations may not have been completed as exepected";
		status = ReporterEvent::Status::Warning;
		strSummary << "Summary:";
		if (m_warning != 0) strSummary << "  Warnings:   " << m_warning;
	}
	else if (m_unknown != 0) {
		strResult << "Unknown error has be encountered, some operations may not have been completed";
		status = ReporterEvent::Status::Unkown;
		strSummary << "Summary:";
		if (m_unknown != 0) strSummary << "  Unknowns:   " << m_unknown;
	}
	else {
		strResult << "Completed successfully" << '\n';
		status = ReporterEvent::Status::Completed;
	}
	setSummary(strSummary.str().c_str());
	std::string message = strResult.str().c_str();
	ReporterEvent res(status, message);
	
	setResult(strResult.str().c_str());
	return res;
}

/*
strSummary << "  Completed: " << m_completed << '\n';
		if (m_warning != 0) strSummary << "  Warnings:   " << m_warning << '\n';
		if (m_error != 0) strSummary << "  Errors:     " << m_error << '\n';
		if (m_fatal != 0) strSummary << "  Fatal:     " << m_fatal << '\n';
		if (m_unknown != 0) strSummary << "  Unknowns:   " << m_unknown << '\n';
*/


void SummaryReporter::toConsole()
{
	std::cout << m_summary;
	std::cout << std::endl;
	std::cout << m_result;
}

CLog::CLog(int code) : m_code(code), m_level(CLogger::Level::ERR) {};
CLog::CLog() : m_code(0), m_level(CLogger::Level::ERR) {};


CLog& CLog::operator<< (const int i) {

	if (m_currentMessage.length() == 0) {
		m_currentMessage = std::to_string(i);
	}
	else {
		m_currentMessage += std::to_string(i);
	}
	return *this;
}

CLog& CLog::operator<< (const std::string& s) {
	if (m_currentMessage.length() == 0) {
		m_currentMessage = s;
	}
	else {
		m_currentMessage += s;
	}
	return *this;
}

CLog& CLog::operator<< (CLogger::Level level) {
	m_level = level;
	CLogger& logger = CLogger::getLogger();
	logger.log(m_code, m_level, m_currentMessage);
	return *this;
}


