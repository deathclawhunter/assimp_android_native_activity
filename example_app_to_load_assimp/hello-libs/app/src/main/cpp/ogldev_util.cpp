/*

	Copyright 2014 Etay Meiri

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <iostream>
#include <fstream>
#ifdef WIN32
#include <Windows.h>
#else
#include <sys/time.h>
#endif

#include "ogldev_util.h"

static int ReadLine(FILE *fp, char *buf, int bufSize) {
    int readNum = 0;
    char *ptr = buf;
    *ptr = 0;

    do {
        readNum = fread(ptr, 1, 1, fp);

        // hit new line char
        if (buf[0] != 0 && *ptr == '\r' || *ptr == '\n') {
            *ptr = 0;
            break;
        } else if (buf[0] == 0) {
            if (*ptr == '\r' || *ptr == '\n') {
                do {
                    readNum = fread(ptr, 1, 1, fp);
                } while (readNum > 0 && (*ptr == '\r' || *ptr == '\n'));

                if (readNum <= 0) {
                    break;
                }
            }
        }

        // hit buffer limit
        if (ptr < buf + bufSize) {
            ptr++;
        } else {
            *ptr = 0;
            break;
        }

    } while (readNum > 0);

    return readNum;
}

bool ReadFile(const char* pFileName, string& outFile) {
    bool ret = false;
    char buf[BUFSIZ + 1];
    buf[BUFSIZ] = 0;

    FILE *f1 = fopen(pFileName, "rb");
    if (f1 != NULL) {
        int readStr = 0;
        do {
            memset(buf, 0, BUFSIZ);
            readStr = ReadLine(f1, buf, BUFSIZ);
            if (strlen(buf) > 0) {
                ret = true;
                outFile.append(buf);
                outFile.append("\n");
            }
        } while (readStr > 0);
        fclose(f1);
    }

    return ret;
}

void OgldevError(const char* pFileName, uint line, const char* pError)
{
#ifdef WIN32
    char msg[1000];
    _snprintf_s(msg, sizeof(msg), "%s:%d: %s", pFileName, line, pError);
    MessageBoxA(NULL, msg, NULL, 0);
#else
    fprintf(stderr, "%s:%d: %s\n", pFileName, line, pError);
#endif    
}


void OgldevFileError(const char* pFileName, uint line, const char* pFileError)
{
#ifdef WIN32
    char msg[1000];
    _snprintf_s(msg, sizeof(msg), "%s:%d: unable to open file `%s`", pFileName, line, pFileError);
    MessageBoxA(NULL, msg, NULL, 0);
#else
    fprintf(stderr, "%s:%d: unable to open file `%s`\n", pFileName, line, pFileError);
#endif    
}


long long GetCurrentTimeMillis()
{
#ifdef WIN32    
    return GetTickCount();
#else
    timeval t;
    gettimeofday(&t, NULL);

    long long ret = t.tv_sec * 1000 + t.tv_usec / 1000;
    return ret;
#endif    
}

#ifdef WIN32
#if (_MSC_VER == 1900)
#elif (_MSC_VER == 1800)
#else
float fmax(float a, float b)
{
    if (a > b)
        return a;
    else
        return b;
}
#endif
#endif