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

//////////////////////////////////////////////////////////////////////
//
// CIDKFileFind.cpp: implementation of the CIDKFileFind class.
//
//////////////////////////////////////////////////////////////////////
#include "CIDKFileFind.h"
#ifdef _WIN32
#include <io.h>
#else
#include <dirent.h>
#include <limits.h>
#include <string.h>
#endif

// #include "sys/syslimits.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

bool CIDKFileFind::Open(void)
{
#ifdef _WIN32
	struct _finddata_t filedata;
	m_Filelocator = _findfirst(m_szFilePattern.c_str(), &filedata);
	if (m_Filelocator < 0)
	{
		m_Filelocator = 0;
		return false;
	}
	m_szCurrentFile = filedata.name;
#else
	m_Filelocator = opendir(m_szPath.c_str());
	m_pEntry = (struct dirent *) new char [sizeof (struct dirent) + PATH_MAX];

	if (m_Filelocator == NULL)
	{
		return false;
	}
	if (GetNext()==false)
	{
		m_Filelocator = 0;
		return false;
	}
#endif
	return true;
}

bool CIDKFileFind::Close()
{
#ifdef _WIN32
	if (m_Filelocator != 0)
	{
		_findclose(m_Filelocator);
		m_Filelocator = 0;
		return true;
	}
#else
	delete [] (char*) m_pEntry;
	m_pEntry = 0;

	if (m_Filelocator != 0)
	{
		closedir(m_Filelocator);
		m_Filelocator = 0;
		return true;
	}
#endif
	return false;
}

/*
 * Unix File-system dependent directory entry.
 *
typedef struct dirent {
    ino_t       d_ino;      * "inode number" of entry
    off_t       d_off;      * offset of disk directory entry
    unsigned short  d_reclen;
							* length of this record
    char        d_name[1];  * name of file
} dirent_t;

!!!! enough space must be allocated for d_name - PSB

*/

#ifdef _WIN32
bool CIDKFileFind::GetNext(void)
{
	struct _finddata_t filedata;
	if (m_Filelocator == 0 )
	{
		return false;
	}
	if (_findnext(m_Filelocator, &filedata) != 0)
	{
		Close();
		return false;
	}
	m_szCurrentFile = filedata.name;
	return true;
}
#else

// UNIX version - requires dirent

bool CIDKFileFind::GetNext(void)
{
	struct dirent *entry = nullptr;

	if (m_Filelocator == 0)
	{
		return false;
	}

/* linux
	while (entry = readdir_r(m_Filelocator, m_pEntry))
	{
		if (PatternMatch(m_szFile, m_pEntry->d_name))
		{
			m_szCurrentFile = m_pEntry->d_name;
			break;
		}
	}
*/
	if (entry)	return true;
	else
	{
		Close();
		return false;
	}
}

bool CIDKFileFind::PatternMatch(char* pattern, char* string)
{
	long i=0;
	long j=0;

	while (string[i] != 0)
	{
		if (pattern[j]=='*')
		{
			j++;
			char* idx = strrchr(&string[i], pattern[j]);
			if (idx == NULL)
			{
				break;
			}
			i=idx-string;
		}
		else if (pattern[j]=='?')
		{
			j++;
			i++;
		}
		else
		{
			if (string[i] != pattern[j])
			{
				break;
			}
			i++;
			j++;
		}
	}
	if (pattern[j] != 0)
	{
		return false;
	}
	return true;
}

#endif
