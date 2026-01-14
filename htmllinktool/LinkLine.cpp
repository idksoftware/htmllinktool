#include "LinkLine.h"

std::string LinkLine::m_extension;

bool LinkLine::process()
{
    logger.log(0, CLogger::Level::INFO, "Processing link: %s", m_linkLine.c_str());
    int pos;
    if ((pos = m_linkLine.find("href")) == std::string::npos) {
        logger.log(0, CLogger::Level::INFO, "Cannot find \"href\" of link target %s", m_linkLine.c_str());
        return false;
    }
    if ((pos = m_linkLine.find_first_of('\"', pos + 1)) == std::string::npos) {
        logger.log(0, CLogger::Level::INFO, "Cannot find the start of link target %s", m_linkLine.c_str());
        return false;
    }
    int s = pos;
    if ((pos = m_linkLine.find_first_of('\"', pos + 1)) == std::string::npos) {
        logger.log(0, CLogger::Level::INFO, "Cannot find end of link target %s", m_linkLine.c_str());
        return false;
    }
    int e = pos;
    m_linkTarget = m_linkLine.substr(s + 1, e - (s + 1));

    logger.log(0, CLogger::Level::INFO, "linkTarget: %s", m_linkTarget.c_str());
    std::filesystem::path p = m_linkTarget;
    if (hasExt())
    {
        m_prevext = p.extension().string();
        logger.log(0, CLogger::Level::INFO, "Has extension: %s", m_prevext.c_str());

        if (m_prevext.compare(m_extension) != 0) {
            logger.log(0, CLogger::Level::INFO, "Replaceing extension: %s with %s", m_prevext.c_str(), m_extension.c_str());
            p.replace_extension(m_extension);
            m_newLinkTarget = p.string();
            logger.log(0, CLogger::Level::INFO, "Link target updated: %s", m_newLinkTarget.c_str());
            m_linkTargetUpdated = true;
            std::string lineStart = m_linkLine.substr(0, s + 1);
            std::string lineEnd = m_linkLine.substr(e, m_linkLine.length() - e);
            m_linkLine = lineStart;
            m_linkLine += m_newLinkTarget;
            m_linkLine += lineEnd;
            logger.log(0, CLogger::Level::INFO, "Link updated: %s", m_linkLine.c_str());
            m_linkTarget = m_newLinkTarget;
        }
    }
    return true;
}

bool LinkLine::hasExt()
{
    unsigned int i = 0;
    if ((i = static_cast<unsigned>(m_linkTarget.find_last_of("."))) == static_cast<unsigned>(-1))
    {
        return false;
    }
    if (i >= (m_linkTarget.length() - 1))
    {
        // find . but no extention i.e "."
        return false;
    }
    return true;
}

std::string LinkLine::getFilePathNoExt(const char* path)
{

    std::filesystem::path p = path;
    return p.replace_extension().string();
}

void LinkLine::replaceExt(const char* ext)
{
    std::filesystem::path p = m_linkTarget;
    std::string extStr = ext;
    p.replace_extension(extStr.c_str());
}

