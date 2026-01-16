// mdhtmllinktool.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <string>

#include <filesystem>
#include "CLogger.h"
#include "ExifDateTime.h"
#include "LinkLine.h"
#include "FileNode.h"
#include "options.h"

// https://www.codeproject.com/articles/Cplusplus-Replacement-for-getopt#comments-section

/*
 Defining Options
Each valid option is described by a descriptor string with the following syntax:

CODE

[<short>] <flag> [<long>] [<spaces><parameter>] [\t<description>]
where:

<short> - a single character that is the short form of the option.
<flag> - one character that specifies the number of arguments that can follow the option:
'|' - no arguments
':' - one required argument
'?' - one optional argument
'+' - one or more arguments
'*' - zero or more arguments
<long> - a string that specifies the long form of the option
<parameter> - a string that is used as parameter name in synopsis
<description> - a string used for option description. Parameter name and description are separated by a tab \t character. Either one of the long or short forms of an option can be missing.
*/

/*
	--root-path C:\Development\ImgArchive\docs\UserDocs\imgarchive-home.html
 */



int main(int argc, char* argv[]) {

    //See what we've got
    /* debug
    std::cout << "argc=" << argc << std::endl;
    for (int i = 0; i < argc; i++) {
        std::cout << "argv[" << i << "]=" << argv[i] << std::endl;
    }
    std::cout << std::endl << std::endl;
    */

    getopt::OptParser parser{
        "h|help \t show help message",
        //"y| \t boolean flag",
        //"n| \t another boolean flag",
          "r:root-path filepath \t Root html file path",
          "e:rep-ext extention \t Replacement extention (default html)",
          "l:log-path log-filepath \t Path to logging files (default html)",
          "L:logging-level log-level \t logging level for log files (default \"summary\")",
          "C:console-level console-level \t logging level for console (default \"summary\")",
          //"o?option value \t optional value",
          //"*stuff things \t option with zero or more arguments"
    };

    int nonopt, ret;
    if ((ret = parser.parse(argc, argv, &nonopt)) != 0)
    {
        if (ret == 1)
            std::cout << "Unknown option: " << argv[nonopt] << " Valid options are : " << std::endl
            << parser.synopsis() << std::endl;
        else if (ret == 2)
            std::cout << "Option " << argv[nonopt] << " requires an argument." << std::endl;
        else if (ret == 3)
            std::cout << "Invalid options combination: " << argv[nonopt] << std::endl;
        exit(1);
    }
    if (argc == 1)
    {
        //No arguments on command line. Show help and exit.
        std::cout << "Usage:" << std::endl;
        std::cout << parser.synopsis() << std::endl;
        std::cout << "Where:" << std::endl
            << parser.description() << std::endl;
        exit(0);
    }

    std::string rootFilepath;
    if (parser.getopt("root-path", rootFilepath))
    {

        bool rootOk = false;
        if (std::filesystem::exists(rootFilepath.c_str()) == true) {

            if (std::filesystem::is_regular_file(rootFilepath) == true)
            {
                rootOk = true;
            }
        }
        if (rootOk == false) {

            std::cout << "Root html Filepath: \"" << rootFilepath << "\" not valid" << std::endl << std::endl;

            std::cout << "Synopsis:" << std::endl << parser.synopsis() << std::endl;
            std::cout << "Description:" << std::endl << parser.description() << std::endl;
            exit(-1);
        }

    }

    std::string extention;
    if (parser.getopt("rep-ext", extention))
    {
        std::cout << "Using replacement extention: \"" << extention << std::endl << std::endl;
    }
    else
    {
        extention = "html";
    }
    
    std::string logPath;
    if (parser.getopt("log-path", logPath))
    {
        bool logOk = false;
        if (std::filesystem::exists(logPath.c_str()) == true) {

            if (std::filesystem::is_regular_file(logPath) == true)
            {
                logOk = true;
            }
        }
        if (logOk == false) {

            std::cout << "Root html Filepath: \"" << logPath << "\" not valid" << std::endl << std::endl;

            std::cout << "Synopsis:" << std::endl << parser.synopsis() << std::endl;
            std::cout << "Description:" << std::endl << parser.description() << std::endl;
            exit(-1);
        }
    } else
    {
        logPath = "C:\\temp";
    }

    std::string loglevel;
    if (parser.getopt("logging-level", loglevel))
    {
        std::cout << "Log file level reporting: \"" << loglevel << std::endl << std::endl;
    }
    else
    {
        loglevel = "summary";
    }

    std::string consolelevel;
    if (parser.getopt("console-level", consolelevel))
    {
        std::cout << "Console level reporting: \"" << consolelevel << std::endl << std::endl;
    }
    else
    {
        consolelevel = "summary";
    }


    CLogger::setAppName("HtmlLinkTool");
    CLogger::setLogLevel(loglevel);
    CLogger::setConsoleLevel(consolelevel);
    CLogger::setSilent(false);
    CLogger::setLogPath(logPath.c_str());
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


    
    logger.log(0, CLogger::Level::SUMMARY, "Root filepath: %s", rootFilepath.c_str());

    std::filesystem::path p = rootFilepath;
    std::filesystem::current_path(p.parent_path());

    logger.log(0, CLogger::Level::STATUS, "Setting extension: %s", extention.c_str());
    LinkLine::setExtension(extention.c_str());

    FileNode fileNode(rootFilepath.c_str());
    if (fileNode.process() == false)
    {
        return -1;
    }
    
    ExifDateTime date;
    logger.log(0, CLogger::Level::SUMMARY, "Application ended at %s", date.toString().c_str());
    return 0;
}
