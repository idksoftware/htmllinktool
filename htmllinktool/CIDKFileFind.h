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

#ifndef _CIDKFileFind
#define _CIDKFileFind

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include <string>
#ifndef _WIN32
#include <dirent.h>
#include <string.h>
#endif

class CIDKFileFind  
{
public:
	CIDKFileFind(std::string inFilePattern)
	{
		m_szFilePattern = inFilePattern;

		const char* ptr = m_szFilePattern.c_str();
		const char *l_ResPtr = nullptr;
		//i=strrchr(ptr, '/')-ptr;
		l_ResPtr = strrchr(ptr, '/');
		if (l_ResPtr != nullptr)
		{
			// The File Pattern also contains the path

			m_szPath = m_szFilePattern.substr(m_szFilePattern.find_last_of("/"));
			m_szFile = m_szFilePattern.substr(m_szFilePattern.find_last_of("/"), m_szFilePattern.length());
		}
		else
		{
			// The File Pattern do'es not contain the path
			// so is relative to the 
			m_szFile = m_szFilePattern;
		}
		m_szCurrentFile = "";
		m_Filelocator = 0;
	};
	virtual ~CIDKFileFind()
	{
		Close();
	};

	bool GotFile(void)
	{
		return (m_Filelocator != 0);
	};
	bool GetNext();
	bool Open();
	bool Close();
	std::string GetFileName()
	{
		return m_szCurrentFile;
	};

private:
#ifdef _WIN32
	intptr_t  m_Filelocator;
#else
	DIR* m_Filelocator;
	struct dirent *m_pEntry;
	bool PatternMatch(char* pattern, char* string);
#endif

	std::string m_szFilePattern;
	std::string m_szPath;
	std::string m_szFile;
	std::string m_szCurrentFile;
};

#endif



