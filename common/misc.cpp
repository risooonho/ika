/*
look at misc.h :P
*/

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include "misc.h"

bool IsPowerOf2(uint i)
{
    return (i & (i - 1)) == 0;
}

uint NextPowerOf2(uint i)
{
    if (IsPowerOf2(i)) return i;
    
    uint j = 1;
    while ((j <<= 1) < i);
    return j;
}

int sgn(int x)
{
    if (x < 0) return -1;
    if (x > 0) return  1;
    return 0;
}

char* va(char* format, ...)
{
    va_list argptr;
    static char string[1024];
    
    va_start(argptr, format);
    vsprintf(string, format, argptr);
    va_end(argptr);
    
    return string;
}

int Random(int min, int max)
{
    int i;
    
    if (max < min)
        swap(max, min);
    
    i=(rand()%(max - min))+min;
    
    return i;
}

#include <string>
#include <sstream>

const string Trim(const std::string& s)
{
    uint start = 0, end = s.length();

    while (s[start] == ' ' && start < s.length()) start++;
    while (s[end - 1] == ' ' && end > 0) end--;
    if (start >= end) 
        return "";
    else
        return s.substr(start, end - start);
}

string Lower(const string& s)
{
    string t(s);

    for (uint i = 0; i < t.length(); i++)
        if (t[i] >= 'A' && t[i] <= 'Z')
            t[i] ^= 32;
    
    return t;
}

string Upper(const string& s)
{
    string t(s);

    for (uint i = 0; i < t.length(); i++)
        if (t[i] >= 'a' && t[i] <= 'z')
            t[i] ^= 32;

    return t;
}

string ToString(int i)
{
    std::stringstream s;
    s << i << '\0';
    return s.str();
}

//---------------------------------------
// put this in its own file? --andy

string Path::Directory(const string& s, const string& relativeto)
{
    int p = s.rfind(Path::cDelimiter);
    if (p==string::npos) return "";

    string sPath = s.substr(0, p + 1);

    // FIXME?  This assumes that relativeto and s are both absolute paths.
    // Or, at the least, that the two paths have the same reference point.
    unsigned int i = 0;
    for (; i < sPath.length(); i++)
        if (i >= relativeto.length() || sPath[i]!=relativeto[i])
            break;        

    p = sPath.rfind(cDelimiter, i);    // go back to the last slash we found

    return p!=string::npos ? sPath.substr(p) : sPath;
}

string Path::Filename(const string& s)
{
    int p = s.rfind(Path::cDelimiter);

    if (p==string::npos) return s;
    return s.substr(p + 1);
}

string Path::Extension(const string& s)
{
    int pos = s.rfind('.');
    if (pos == string::npos)
        return "";

    return s.substr(pos + 1);
}

string Path::ReplaceExtension(const string& s, const string& extension)
{
    int pos = s.rfind('.');
    if (pos == string::npos)
        return s + "." + extension;

    return s.substr(0, pos + 1) + extension;
}

bool Path::Compare(const string& s, const string& t)
{
#ifdef WIN32
    return Upper(s) == Upper(t);
#else
    return s == t;
#endif
}
