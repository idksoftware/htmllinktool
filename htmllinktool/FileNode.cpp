#include "FileNode.h"
#include "LinkLine.h"

std::string normalizePath(const std::string& messyPath) {
    std::filesystem::path path(messyPath);
    std::filesystem::path canonicalPath = std::filesystem::weakly_canonical(path);
    std::string npath = canonicalPath.make_preferred().string();
    return npath;
}

FileNode::FileNode(const char* path) : m_path(path), logger(CLogger::getLogger())
{
    std::filesystem::path p = path;
    
    std::filesystem::path current_working_path = std::filesystem::current_path().string();
    
    std::filesystem::path new_current_working_path = current_working_path;
    new_current_working_path /= p.parent_path();
    //m_current_path = new_current_path.string();
    m_filename = p.filename().string();
    m_new_current_working_path = normalizePath(new_current_working_path.string());
    logger.log(0, CLogger::Level::STATUS, "Current File: \"%s\" path: \"%s\"", m_filename.c_str(), new_current_working_path.string().c_str());
    logger.log(0, CLogger::Level::INFO, "setting current working path: %s", m_new_current_working_path.c_str());
    std::filesystem::current_path(m_new_current_working_path);
};

bool FileNode::read()
{
    
    std::filesystem::path fullCurrentPath = m_new_current_working_path;
    fullCurrentPath /= m_filename;
    logger.log(0, CLogger::Level::INFO, "Reading file: %s", fullCurrentPath.string().c_str());
    std::ifstream inputFile(fullCurrentPath.string().c_str());
    if (!inputFile.is_open()) {
        logger.log(0, CLogger::Level::ERR, "Error: Unable to open file: %s", fullCurrentPath.string().c_str());
        return false;
    }
    std::string line;
    int lineNumber = 1;
    while (std::getline(inputFile, line, '\n')) {
        std::shared_ptr<LinkLine> linkLine = readLink(lineNumber, line);
        lineNumber++;
        if (linkLine != nullptr)
        {
            if (linkLine->process() == false)
            {
                return false;
            }
            if (linkLine->islinkTargetUpdated())
            {
                logger.log(0, CLogger::Level::INFO, "Replace link in file Contents: %s", m_path);
                m_fileContents.push_back(linkLine->getNewLinkLine());
                m_linkList.push_back(linkLine);
                m_updated = true;
            }
            else
            {
                m_fileContents.push_back(line);
                m_linkList.push_back(linkLine);
            }
        }
        else
        {
            m_fileContents.push_back(line);
        }
    }
    // Close the file
    inputFile.close();
    return true;
}



bool FileNode::write(const char* path)
{
    logger.log(0, CLogger::Level::INFO, "Writing file Contents: %s", path);
    std::ofstream outputFile(path);
    if (!outputFile.is_open()) {
        std::cerr << "Error: Unable to open file." << std::endl;
        logger.log(0, CLogger::Level::ERR, "Error: Unable to open file: %s", path);
        return false;
    }
    for (auto line : m_fileContents)
    {
        outputFile << line << std::endl;
        logger.log(0, CLogger::Level::TRACE, "Writing line: %s", line.c_str());
        //std::cout << line << std::endl;
    }
    outputFile.close();
    return true;
}

std::shared_ptr<LinkLine> FileNode::readLink(int lineNumber, std::string& line)
{
    // <a href="URL">
    // <a href="ImgArchive-Home_797376627.html">ImgArchive Home</a>
    std::shared_ptr<LinkLine> linkLine;

    int pos;
    if ((pos = line.find("href")) != std::string::npos) {
        logger.log(0, CLogger::Level::INFO, "Found href: %s", line.c_str());
        linkLine = std::make_shared<LinkLine>(lineNumber, line.c_str());
    }
    return linkLine;
}

bool FileNode::processFile()
{
    if (read() == false)
    {
        return false;
    }
    if (isUpdated() == true)
    {
        /*
        std::filesystem::path p = m_path;
        std::filesystem::path rp = p.parent_path();
        std::string f = p.filename().string();
        std::string nf = "~";
        nf += f;
        std::filesystem::path np = rp;
        np /= nf;
		*/
        
        std::string f = m_filename;
        std::string nf = "~";
        nf += m_filename;

        std::error_code ec;
        std::filesystem::rename(f, nf, ec);
        if (ec)
        {
            //logger.log(0, CLogger::Level::ERR, "File rename failed? old name: \"%s\" new name: \"%s\"", p.string().c_str(), np.string().c_str());
            logger.log(0, CLogger::Level::ERR, "File rename failed? old name: \"%s\" new name: \"%s\"", f.c_str(), nf.c_str());
            return false;
        }

        if (write(f.c_str()) == false)
        {
            logger.log(0, CLogger::Level::INFO, "Writing file Contents: %s", f.c_str());
            return false;
        }
    }
    return true;
}


bool FileNode::processLinks()
{
    logger.log(0, CLogger::Level::INFO, "processing links in : %s", m_filename.c_str());
    for (auto link : m_linkList)
    {
        logger.log(0, CLogger::Level::INFO, "processing link: %s", link->getLinkTarget().c_str());
        std::filesystem::path p = link->getLinkTarget();
        std::string path = p.string();

        std::string current_path = std::filesystem::current_path().string();
        std::cout << "Current path: " << current_path << std::endl;

        logger.log(0, CLogger::Level::INFO, "Finding if link path \"%s\" exists", link->getLinkTarget().c_str());
        if (std::filesystem::exists(path.c_str()) == true) {
            logger.log(0, CLogger::Level::INFO, "Link path \"%s\" does exists", link->getLinkTarget().c_str());
            if (std::filesystem::is_regular_file(p.string()) == true)
            {
                logger.log(0, CLogger::Level::INFO, "Link path \"%s\" is regular file", link->getLinkTarget().c_str());
                std::string pathStr = p.string().c_str();
                FileNode fileNode(pathStr.c_str());
                if (fileNode.process() == false)
                {
                    logger.log(0, CLogger::Level::ERR, "failed to process %s", m_filename.c_str());
                    return false;
                }
            }
            else
            {
                logger.log(0, CLogger::Level::INFO, "Link path \"%s\" is not a regular file", link->getLinkTarget().c_str());
                return false;
            }
        }
        else
        {
            logger.log(0, CLogger::Level::INFO, "Link path \"%s\" does not exists", link->getLinkTarget().c_str());
            return false;
        }
    }
    return true;
}

bool FileNode::process()
{
    if (processFile() == false)
    {
        return false;
    }
    if (processLinks() == false)
    {
        return false;
    }
    return false;
}


