#ifndef GOMOLOGGING_H
#define GOMOLOGGING_H

#include <QString>
#include <QFile>
#include <string>
using std::string;


class GomoLogging
{
protected:
    GomoLogging();


protected:
    QFile m_logfile;


public:
    static GomoLogging* GetInstancePtr();

public:
    void logging(std::string log_line);

};

#endif // GOMOLOGGING_H
