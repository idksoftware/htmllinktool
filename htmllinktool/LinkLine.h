#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib> // for strtol
#include <vector> // for errno
#include <filesystem>
#include "CLogger.h"
#include "ExifDateTime.h"
/*
 <a href="ImgArchive-Home_797376627.html">ImgArchive Home</a>
 */

class LinkLine
{
    std::string m_linkLine;
    //std::string m_newLinkLine;
    std::string m_linkTarget;
    std::string m_newLinkTarget;
    static std::string m_extension;
    std::string m_prevext;
    bool m_linkTargetUpdated{ false };
    CLogger& logger;
    int m_lineNumber{ -1 };
public:

    LinkLine() : logger(CLogger::getLogger()) {};
    LinkLine(int lineNumber, const char* linkLine) : m_lineNumber(lineNumber), m_linkLine(linkLine), logger(CLogger::getLogger())
    {
        logger.log(0, CLogger::Level::INFO, "Creating linkline - line number: %d path: %s", m_lineNumber, m_linkLine.c_str());
    }

    virtual ~LinkLine() = default;

    bool process();
    bool hasExt();
    void replaceExt(const char* ext);
    std::string getFilePathNoExt(const char* path);

    static void setExtension(const char* ext)
    {
        m_extension = ext;
    }

    bool islinkTargetUpdated()
    {
        return m_linkTargetUpdated;
    }

    const char* getNewLinkLine()
    {
        return m_linkLine.c_str();
    }

    std::string getLinkTarget() { return m_linkTarget; }
    //std::string getNewLinkTarget() { return m_newLinkTarget; }
    int getLineNumber() const { return m_lineNumber; }
};
