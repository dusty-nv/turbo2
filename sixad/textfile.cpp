/*
 * textfile.cpp
 *
 * This file is part of the QtSixA, the Sixaxis Joystick Manager
 * Copyright 2008-2010 Filipe Coelho <falktx@gmail.com>
 *
 * QtSixA can be redistributed and/or modified under the terms of the GNU General
 * Public License (Version 2), as published by the Free Software Foundation.
 * A copy of the license is included in the QtSixA source code, or can be found
 * online at www.gnu.org/licenses.
 *
 * QtSixA is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 */

#include "textfile.h"

#include <cerrno>
#include <ctype.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/mman.h>
#include <sys/stat.h>

inline char *find_key(char *map, size_t size, const char *key, size_t len, int icase)
{
        char *ptr = map;
        size_t ptrlen = size;

        while (ptrlen > len + 1) {
                int cmp = (icase) ? strncasecmp(ptr, key, len) : strncmp(ptr, key, len);
                if (cmp == 0) {
                        if (ptr == map)
                                return ptr;

                        if ((*(ptr - 1) == '\r' || *(ptr - 1) == '\n') &&
                                                        *(ptr + len) == ' ')
                                return ptr;
                }

                if (icase) {
                        char *p1 = (char*)memchr(ptr + 1, tolower(*key), ptrlen - 1);
                        char *p2 = (char*)memchr(ptr + 1, toupper(*key), ptrlen - 1);

                        if (!p1)
                                ptr = p2;
                        else if (!p2)
                                ptr = p1;
                        else
                                ptr = (p1 < p2) ? p1 : p2;
                } else
                        ptr = (char*)memchr(ptr + 1, *key, ptrlen - 1);

                if (!ptr)
                        return NULL;

                ptrlen = size - (ptr - map);
        }

        return NULL;
}

char *read_key(const char *pathname, const char *key, int icase)
{
        struct stat st;
        char *map, *off, *end, *str = NULL;
        off_t size; size_t len;
        int fd, err = 0;

        fd = open(pathname, O_RDONLY);
        if (fd < 0)
                return NULL;

        if (flock(fd, LOCK_SH) < 0) {
                err = errno;
                goto close;
        }

        if (fstat(fd, &st) < 0) {
                err = errno;
                goto unlock;
        }

        size = st.st_size;

        map = (char*)mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
        if (!map || map == MAP_FAILED) {
                err = errno;
                goto unlock;
        }

        len = strlen(key);
        off = find_key(map, size, key, len, icase);
        if (!off) {
                err = EILSEQ;
                goto unmap;
        }

        end = strpbrk(off, "\r\n");
        if (!end) {
                err = EILSEQ;
                goto unmap;
        }

        str = (char*)malloc(end - off - len);
        if (!str) {
                err = EILSEQ;
                goto unmap;
        }

        memset(str, 0, end - off - len);
        strncpy(str, off + len + 1, end - off - len - 1);

unmap:
        munmap(map, size);

unlock:
        flock(fd, LOCK_UN);

close:
        close(fd);
        errno = err;

        return str;
}

int textfile_get_int(const char *pathname, const char *key, int default_key)
{
    char* ret_key = read_key(pathname, key, 0);
    if (ret_key != NULL)
      return atoi(ret_key);
    else
      return default_key;
}
