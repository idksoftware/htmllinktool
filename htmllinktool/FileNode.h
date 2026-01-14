#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib> // for strtol
#include <vector> // for errno
#include <filesystem>
#include "CLogger.h"
#include "LinkLine.h"

class FileNode
{
    const char* m_path;
    std::string m_filename;
    std::string m_new_current_working_path;
    std::string m_current_working_path;
    std::vector<std::shared_ptr<LinkLine>> m_linkList;
    std::vector<std::string> m_fileContents;
    bool m_updated{ false };
    CLogger& logger;
public:
    FileNode() = default;
    FileNode(const char* path);
	
    virtual ~FileNode() = default;

    bool read();
    bool write(const char* path);
    std::shared_ptr<LinkLine> readLink(int lineNumber, std::string& linkLine);

    bool isUpdated()
    {
        return m_updated;
    }

    bool processFile();
    bool processLinks();
    bool process();
};


