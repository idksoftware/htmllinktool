// mdhtmllinktool.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib> // for strtol
#include <vector> // for errno
#include <filesystem>
#include "CLogger.h"
#include "ExifDateTime.h"
#include "LinkLine.h"
#include "FileNode.h"



int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << "\n";
        return 1;
    }


    CLogger::setAppName("HtmlLinkTool");
    CLogger::setLogLevel(CLogger::Level::TRACE);
    CLogger::setConsoleLevel(CLogger::Level::TRACE);
    CLogger::setSilent(false);
    CLogger::setLogPath("C:\\temp");
    try {
        CLogger::startLogging();
    }
    catch (std::exception e) {
        printf("Failed to start logging");
        return -1;
    }
    CLogger& logger = CLogger::getLogger();
    try {
        ExifDateTime date;
        logger.log(0, CLogger::Level::SUMMARY, "Application starting at %s", date.toString().c_str());
        CLogger::startLogging();
        logger.log(0, CLogger::Level::INFO, "Starting logging");
    }
    catch (std::exception e) {
        logger.log(0, CLogger::Level::FATAL, "Failed to start logging");
        return false;
    }


    std::string rootFilepath = argv[1];
    logger.log(0, CLogger::Level::INFO, "Root filepath: %s", rootFilepath.c_str());

    std::filesystem::path p = rootFilepath;
    std::filesystem::current_path(p.parent_path());

    logger.log(0, CLogger::Level::STATUS, "Setting extension: %s", ".html");
    LinkLine::setExtension(".html");

    FileNode fileNode(rootFilepath.c_str());
    if (fileNode.processFile() == false)
    {
        return -1;
    }
    if (fileNode.processLinks() == false)
    {
        return -1;
    }
    ExifDateTime date;
    logger.log(0, CLogger::Level::SUMMARY, "Application ended at %s", date.toString().c_str());
    return 0;
}
