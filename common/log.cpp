
#include "log.h"
#include <stdarg.h>
#include <fstream>
#include <ios>
#include <stdio.h>

using std::ios;

// -----------------------

namespace Log
{
    bool bLogging=false;
    string sLogname;

    std::ofstream logfile;
};

// -----------------------

void Log::Init(const char* fname)
{
    if (bLogging) return;
    bLogging=true;

    remove(fname);

    sLogname=fname;

    logfile.open(fname);

#ifdef LOG_CALLBACK
    remove("callback.log");
#endif
};

void Log::Writen(const char* s,...)
{
    if (!bLogging) return;
    
    char sTemp[1024];
    va_list lst;
    
    va_start(lst,s);
    vsprintf(sTemp,s,lst);
    va_end(lst);
  
    logfile << sTemp;
//    std::ofstream f(sLogname.c_str(),ios::app);
//    f << sTemp;
}

void Log::Write(const char* s,...)
{
    if (!bLogging) return;

    char sTemp[1024];
    va_list lst;
    
    va_start(lst,s);
    vsprintf(sTemp,s,lst);
    va_end(lst);

    logfile << sTemp;
//    std::ofstream f(sLogname.c_str(),ios::app);
//    f << sTemp << std::endl;
}

// -----------------------

#ifdef LOG_CALLBACK

string CCallbackLog::sHistory;

CCallbackLog::CCallbackLog(const char* s)
    : nOldlen(sHistory.length())
{
    sHistory+="-> ";
    sHistory+=s;
    
    std::ofstream f("callback.log",ios::app);
    f << sHistory << std::endl;
}

CCallbackLog::~CCallbackLog()
{
    sHistory=sHistory.substr(0,nOldlen);
}

#endif