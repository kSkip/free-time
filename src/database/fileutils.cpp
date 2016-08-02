#include "database/fileutils.h"

#include <dirent.h>
#include <sys/stat.h>
#include <sstream>
#include <stdexcept>

int isRegFile(const char *path);

strlist getDirContents(const char* path)
{

    strlist files;

    DIR* dir;
    struct dirent* ent;

    /*
     * Open the directory and throw
     * runtime_error if opendir()
     * returns NULL
     */

    if((dir = opendir(path)) != NULL)
    {

        while((ent = readdir(dir)) != NULL)
        {

            std::string dirpath = path;
            std::string filename = ent->d_name;

            /*
             * We only want regular files
             */
            if(isRegFile((dirpath+"/"+filename).c_str()))
            {

                files.push_back(filename);

            }

        }

        closedir(dir);

    }
    else
    {

        /*
         * Most likely directory didn't exist
         */
        std::ostringstream oss;
        oss << "Could not open directory: ";
        oss << path;
        throw std::runtime_error(oss.str());

    }

	return files;

}

/*
 * checks that the path string
 * indicates a regular file
 */
int isRegFile(const char *path)
{

    struct stat path_stat;

    stat(path, &path_stat);

    return S_ISREG(path_stat.st_mode);

}
