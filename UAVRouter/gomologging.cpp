#include "gomologging.h"

GomoLogging::GomoLogging()
    :m_logfile("log.txt")
{
    m_logfile.open(QFile::WriteOnly | QFile::Truncate);


}



GomoLogging* GomoLogging::GetInstancePtr()
{
    static GomoLogging loggingInstance;
    return &loggingInstance;


}

void  GomoLogging::logging(std::string line)
{
    m_logfile.write(line.c_str(),line.length());
    m_logfile.write("\n");
}
