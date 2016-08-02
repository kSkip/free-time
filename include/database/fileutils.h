#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <string>
#include <list>

typedef std::list<std::string> strlist;

strlist getDirContents(const char* path);

#endif
