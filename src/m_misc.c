//
// Copyright(C) 1993-1996 Id Software, Inc.
// Copyright(C) 1993-2008 Raven Software
// Copyright(C) 2005-2014 Simon Howard
// Copyright(C) 2016-2023 Julian Nechaevsky
// Copyright(C) 2020-2025 Leonid Murin (Dasperal)
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// DESCRIPTION:
//      Miscellaneous.
//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#else
#include <sys/types.h>
#include <unistd.h>
#endif

#include "doomtype.h"
#include "d_name.h"
#include "i_system.h"
#include "i_timer.h"
#include "m_argv.h"
#include "m_misc.h"
#include "z_zone.h"
#include "jn.h"

wchar_t* ConvertToUtf8(const char* str);

//
// Recursively create all directories that make up the given path, including the last component.
//

void M_MakeDirectory(char *path)
{
    char* parentDir = M_DirName(path);
    if(!M_FileExists(parentDir))
        M_MakeDirectory(parentDir);
    free(parentDir);
#ifdef _WIN32
    wchar_t *wdir = ConvertToUtf8(path);
    if(!wdir)
        return;
    _wmkdir(wdir);
    free(wdir);
#else
    mkdir(path, 0755);
#endif
}

// Check if a file exists

boolean M_FileExists(char *filename)
{
#ifdef _WIN32
    wchar_t *wpath = NULL;
    wpath = ConvertToUtf8(filename);
    boolean ret = INVALID_FILE_ATTRIBUTES != GetFileAttributesW(wpath);
    free(wpath);
    return ret;
#else
    FILE *fstream;

    fstream = fopen(filename, "r");

    if (fstream != NULL)
    {
        fclose(fstream);
        return true;
    }
    else
    {
        // If we can't open because the file is a directory, the 
        // "file" exists at least!
        // [Dasperal] For some reason, I get EACCES or ENOENT here for a directory on Win7
        return errno == EISDIR;
    }
#endif
}

// Check if a file exists by probing for common case variation of its filename.
// Returns a newly allocated string that the caller is responsible for freeing.

char *M_FileCaseExists(char *path)
{
    char *path_dup, *filename, *ext;

    path_dup = M_StringDuplicate(path);

    // 0: actual path
    if (M_FileExists(path_dup))
    {
        return path_dup;
    }

    filename = strrchr(path_dup, DIR_SEPARATOR);
    if (filename != NULL)
    {
        filename++;
    }
    else
    {
        filename = path_dup;
    }

    // 1: lowercase filename, e.g. doom2.wad
    M_ForceLowercase(filename);

    if (M_FileExists(path_dup))
    {
        return path_dup;
    }

    // 2: uppercase filename, e.g. DOOM2.WAD
    M_ForceUppercase(filename);

    if (M_FileExists(path_dup))
    {
        return path_dup;
    }

    // 3. uppercase basename with lowercase extension, e.g. DOOM2.wad
    ext = strrchr(path_dup, '.');
    if (ext != NULL && ext > filename)
    {
        M_ForceLowercase(ext + 1);

        if (M_FileExists(path_dup))
        {
            return path_dup;
        }
    }

    // 4. lowercase filename with uppercase first letter, e.g. Doom2.wad
    if (strlen(filename) > 1)
    {
        M_ForceLowercase(filename + 1);

        if (M_FileExists(path_dup))
        {
            return path_dup;
        }
    }

    // 5. no luck
    free(path_dup);
    return NULL;
}

//
// Determine the length of an open file.
//

long M_FileLength(FILE *handle)
{ 
    long savedpos;
    long length;

    // save the current position in the file
    savedpos = ftell(handle);
    
    // jump to the end and find the length
    fseek(handle, 0, SEEK_END);
    length = ftell(handle);

    // go back to the old location
    fseek(handle, savedpos, SEEK_SET);

    return length;
}

boolean M_PathWritable(const char* path)
{
    boolean ret = 0;
    if(!M_FileExists(path))
    {
        const char* parentDir = M_DirName(path);
        ret = M_PathWritable(parentDir);
        free(parentDir);
        return ret;
    }

#ifdef _WIN32
    DWORD length = 0;
    DWORD genericAccessRights = GENERIC_WRITE;

    wchar_t* wpath = ConvertToUtf8(path);
    if(!GetFileSecurityW(wpath, OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION, NULL,
        0, &length) &&
       ERROR_INSUFFICIENT_BUFFER == GetLastError())
    {
        PSECURITY_DESCRIPTOR security = malloc(length);

        if(security &&
           GetFileSecurityW(wpath, OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION,
               security, length, &length))
        {
            HANDLE hToken = NULL;

            if(OpenProcessToken(GetCurrentProcess(),
                TOKEN_IMPERSONATE | TOKEN_QUERY | TOKEN_DUPLICATE | STANDARD_RIGHTS_READ, &hToken))
            {
                HANDLE hImpersonatedToken = NULL;

                if(DuplicateToken(hToken, SecurityImpersonation, &hImpersonatedToken))
                {
                    GENERIC_MAPPING mapping = {0xFFFFFFFF};
                    PRIVILEGE_SET privileges = {0};
                    DWORD grantedAccess = 0, privilegesLength = sizeof(privileges);
                    BOOL result = FALSE;

                    mapping.GenericRead = FILE_GENERIC_READ;
                    mapping.GenericWrite = FILE_GENERIC_WRITE;
                    mapping.GenericExecute = FILE_GENERIC_EXECUTE;
                    mapping.GenericAll = FILE_ALL_ACCESS;

                    MapGenericMask(&genericAccessRights, &mapping);

                    if(AccessCheck(security, hImpersonatedToken, genericAccessRights,
                        &mapping, &privileges, &privilegesLength, &grantedAccess, &result))
                    {
                        ret = result == TRUE;
                    }
                    CloseHandle(hImpersonatedToken);
                }
                CloseHandle(hToken);
            }
            free(security);
        }
    }
    free(wpath);
#else
    ret = access(path, W_OK) == 0;
#endif
    return ret;
}

//
// M_WriteFile
//

boolean M_WriteFile(const char *name, void *source, int length)
{
    FILE *handle;
    int	count;
	
    handle = fopen(name, "wb");

    if (handle == NULL)
	return false;

    count = fwrite(source, 1, length, handle);
    fclose(handle);
	
    if (count < length)
	return false;
		
    return true;
}

boolean M_WriteFileTimeout(const char *name, void *source, int length, int delay)
{
    boolean res;
    int timeout = I_GetTimeMS() + delay;

    res = M_WriteFile(name, source, length);

    // tolerate the given delay when opening a file for writing
    while (res == false && I_GetTimeMS() < timeout)
    {
        I_Sleep(10);
        res = M_WriteFile(name, source, length);
    }

    return res;
}

//
// M_ReadFile
//

int M_ReadFile(char *name, byte **buffer)
{
    FILE *handle;
    int	count, length;
    byte *buf;
	
    handle = fopen(name, "rb");
    if (handle == NULL)
        I_QuitWithError(english_language ?
                        "Couldn't read file %s" :
                        "Невозможно прочитать файл %s",
                        name);

    // find the size of the file by seeking to the end and
    // reading the current position

    length = M_FileLength(handle);
    
    buf = Z_Malloc (length, PU_STATIC, NULL);
    count = fread(buf, 1, length, handle);
    fclose (handle);
	
    if (count < length)
        I_QuitWithError(english_language ?
                        "Couldn't read file %s" :
                        "Невозможно прочитать файл %s",
                        name);
		
    *buffer = buf;
    return length;
}

// Returns the path to a temporary file of the given name, stored
// inside the system temporary directory.
//
// The returned value must be freed with Z_Free after use.

char *M_TempFile(char *s)
{
    char *tempdir;

#ifdef _WIN32

    // Check the TEMP environment variable to find the location.

    tempdir = getenv("TEMP");

    if (tempdir == NULL)
    {
        tempdir = ".";
    }
#else
    // In Unix, just use /tmp.

    tempdir = "/tmp";
#endif

    return M_StringJoin(tempdir, DIR_SEPARATOR_S, s, NULL);
}

FILE *M_fopen(const char *filename, const char *mode)
{
#ifdef _WIN32
    wchar_t* wname = ConvertToUtf8(filename);
    if(!wname)
        return NULL;
    wchar_t* wmode = ConvertToUtf8(mode);
    if(!wmode)
    {
        free(wname);
        return NULL;
    }

    FILE* file = _wfopen(wname, wmode);
    free(wname);
    free(wmode);
    return file;
#else
    return fopen(filename, mode);
#endif
}

int M_remove(const char* path)
{
#ifdef _WIN32
    wchar_t* wpath = ConvertToUtf8(path);
    if(!wpath)
        return 0;
    int ret = _wremove(wpath);
    free(wpath);
    return ret;
#else
    return remove(path);
#endif
}

int M_rename(const char* oldname, const char* newname)
{
#ifdef _WIN32
    wchar_t *wold = ConvertToUtf8(oldname);
    if(!wold)
        return 0;

    wchar_t *wnew = ConvertToUtf8(newname);
    if(!wnew)
    {
        free(wold);
        return 0;
    }

    int ret = _wrename(wold, wnew);
    free(wold);
    free(wnew);
    return ret;
#else
    return rename(oldname, newname);
#endif
}

int M_stat(const char* path, struct stat* buf)
{
#ifdef _WIN32
    struct _stat wbuf;

    wchar_t* wpath = ConvertToUtf8(path);
    if(!wpath)
        return -1;

    int ret = _wstat(wpath, &wbuf);

    // The _wstat() function expects a struct _stat* parameter that is
    // incompatible with struct stat*. We copy only the required compatible
    // field.
    buf->st_mode = wbuf.st_mode;
    free(wpath);
    return ret;
#else
    return stat(path, buf);
#endif
}

boolean M_StrToInt(const char *str, int *result)
{
    return sscanf(str, " 0x%x", result) == 1
        || sscanf(str, " 0X%x", result) == 1
        || sscanf(str, " 0%o", result) == 1
        || sscanf(str, " %d", result) == 1;
}

// Returns the directory portion of the given path, without the trailing
// slash separator character. If no directory is described in the path,
// the string "." is returned. In either case, the result is newly allocated
// and must be freed by the caller after use.
char* M_DirName(const char *path)
{
    char* p = path ? strrchr(path, DIR_SEPARATOR) : NULL;
    if(p == NULL)
    {
        return M_StringDuplicate(".");
    }
    else
    {
        char* result = M_StringDuplicate(path);
        result[p - path] = '\0';
        return result;
    }
}

/**
 * Returns the filename described by the given path (without the directory name).
 * The result points inside path and nothing new is allocated.
 */
const char *M_FileName(const char *path)
{
    const char *pf, *pb;

    pf = strrchr(path, '/');
#ifdef _WIN32
    pb = strrchr(path, '\\');
    // [FG] allow C:filename
    if (pf == NULL && pb == NULL)
    {
        pb = strrchr(path, ':');
    }
#else
    pb = NULL;
#endif
    if (pf == NULL && pb == NULL)
    {
        return path;
    }
    else
    {
        const char *p = MAX(pf, pb);
        return p + 1;
    }
}

void M_ExtractFileBase(const char* path, char* dest, const int dest_size)
{
    const char* src = path + strlen(path) - 1;

    // back up until a \ or the start
    while(src != path && *(src - 1) != DIR_SEPARATOR)
    {
        src--;
    }

    // Copy up to eight characters
    // Note: Vanilla Doom exits with an error if a filename is specified
    // with a base of more than eight characters.  To remove the 8.3
    // filename limit, instead we simply truncate the name.

    int length = 0;
    memset(dest, 0, 8);

    while(*src != '\0' && *src != '.' && length < 8)
    {
        dest[length++] = toupper((int)*src++);
    }
}

//---------------------------------------------------------------------------
//
// PROC M_ForceUppercase
//
// Change string to uppercase.
//
//---------------------------------------------------------------------------

void M_ForceUppercase(char *text)
{
    char *p;

    for (p = text; *p != '\0'; ++p)
    {
        *p = toupper(*p);
    }
}

//---------------------------------------------------------------------------
//
// PROC M_ForceLowercase
//
// Change string to lowercase.
//
//---------------------------------------------------------------------------

void M_ForceLowercase(char *text)
{
    char *p;

    for (p = text; *p != '\0'; ++p)
    {
        *p = tolower(*p);
    }
}

//
// M_StrCaseStr
//
// Case-insensitive version of strstr()
//

char *M_StrCaseStr(char *haystack, char *needle)
{
    unsigned int haystack_len;
    unsigned int needle_len;
    unsigned int len;
    unsigned int i;

    haystack_len = strlen(haystack);
    needle_len = strlen(needle);

    if (haystack_len < needle_len)
    {
        return NULL;
    }

    len = haystack_len - needle_len;

    for (i = 0; i <= len; ++i)
    {
        if (!strncasecmp(haystack + i, needle, needle_len))
        {
            return haystack + i;
        }
    }

    return NULL;
}

//
// Safe version of strdup() that checks the string was successfully
// allocated.
//

char *M_StringDuplicate(const char *orig)
{
    char *result;

    result = strdup(orig);

    if (result == NULL)
    {
        I_QuitWithError(english_language ?
                        "Failed to duplicate string (length %zu)\n" :
                        "Невозможно дублировать строку (длина %zu)\n",
                        strlen(orig));
    }

    return result;
}

//
// String replace function.
//

char *M_StringReplace(const char *haystack, const char *needle,
                      const char *replacement)
{
    char *result, *dst;
    const char *p;
    size_t needle_len = strlen(needle);
    size_t result_len, dst_len;

    // Iterate through occurrences of 'needle' and calculate the size of
    // the new string.
    result_len = strlen(haystack) + 1;
    p = haystack;

    for (;;)
    {
        p = strstr(p, needle);
        if (p == NULL)
        {
            break;
        }

        p += needle_len;
        result_len += strlen(replacement) - needle_len;
    }

    // Construct new string.

    result = malloc(result_len);
    if (result == NULL)
    {
        I_QuitWithError(english_language ?
                        "M_StringReplace: Failed to allocate new string" :
                        "M_StringReplace: не удалось обнаружить новую строку");
        return NULL;
    }

    dst = result; dst_len = result_len;
    p = haystack;

    while (*p != '\0')
    {
        if (!strncmp(p, needle, needle_len))
        {
            M_StringCopy(dst, replacement, dst_len);
            p += needle_len;
            dst += strlen(replacement);
            dst_len -= strlen(replacement);
        }
        else
        {
            *dst = *p;
            ++dst; --dst_len;
            ++p;
        }
    }

    *dst = '\0';

    return result;
}

// Safe string copy function that works like OpenBSD's strlcpy().
// Returns true if the string was not truncated.

boolean M_StringCopy(char *dest, const char *src, size_t dest_size)
{
    size_t len;

    if (dest_size >= 1)
    {
        dest[dest_size - 1] = '\0';
        strncpy(dest, src, dest_size - 1);
    }
    else
    {
        return false;
    }

    len = strlen(dest);
    return src[len] == '\0';
}

// Safe string concat function that works like OpenBSD's strlcat().
// Returns true if string not truncated.

boolean M_StringConcat(char *dest, const char *src, size_t dest_size)
{
    size_t offset;

    offset = strlen(dest);
    if (offset > dest_size)
    {
        offset = dest_size;
    }

    return M_StringCopy(dest + offset, src, dest_size - offset);
}

// Returns true if 's' begins with the specified prefix.

boolean M_StringStartsWith(const char *s, const char *prefix)
{
    return strlen(s) > strlen(prefix)
        && strncmp(s, prefix, strlen(prefix)) == 0;
}

// Returns true if 's' ends with the specified suffix.

boolean M_StringEndsWith(const char *s, const char *suffix)
{
    return strlen(s) >= strlen(suffix)
        && strcmp(s + strlen(s) - strlen(suffix), suffix) == 0;
}

// Return a newly-malloced string with all the strings given as arguments
// concatenated together.

char *M_StringJoin(const char *s, ...)
{
    char *result;
    const char *v;
    va_list args;
    size_t result_len;

    result_len = strlen(s) + 1;

    va_start(args, s);
    for (;;)
    {
        v = va_arg(args, const char *);
        if (v == NULL)
        {
            break;
        }

        result_len += strlen(v);
    }
    va_end(args);

    result = malloc(result_len);

    if (result == NULL)
    {
        I_QuitWithError(english_language ?
                        "M_StringJoin: Failed to allocate new string." :
                        "M_StringJoin: не удалось обнаружить новую строку");
        return NULL;
    }

    M_StringCopy(result, s, result_len);

    va_start(args, s);
    for (;;)
    {
        v = va_arg(args, const char *);
        if (v == NULL)
        {
            break;
        }

        M_StringConcat(result, v, result_len);
    }
    va_end(args);

    return result;
}

void M_FreeStringArray_NullTerminated(const char** array)
{
    const char** temp = array;
    while(*temp != NULL)
    {
        free(*temp);
        temp++;
    }
    free(array);
}

// On Windows, vsnprintf() is _vsnprintf().
#ifdef _WIN32
#if _MSC_VER < 1400 /* not needed for Visual Studio 2008 */
#define vsnprintf _vsnprintf
#endif
#endif

// Safe, portable vsnprintf().
int M_vsnprintf(char *buf, size_t buf_len, const char *s, va_list args)
{
    int result;

    if (buf_len < 1)
    {
        return 0;
    }

    // Windows (and other OSes?) has a vsnprintf() that doesn't always
    // append a trailing \0. So we must do it, and write into a buffer
    // that is one byte shorter; otherwise this function is unsafe.
    result = vsnprintf(buf, buf_len, s, args);

    // If truncated, change the final char in the buffer to a \0.
    // A negative result indicates a truncated buffer on Windows.
    if (result < 0 || result >= buf_len)
    {
        buf[buf_len - 1] = '\0';
        result = buf_len - 1;
    }

    return result;
}

// Safe, portable snprintf().
int M_snprintf(char *buf, size_t buf_len, const char *s, ...)
{
    va_list args;
    int result;
    va_start(args, s);
    result = M_vsnprintf(buf, buf_len, s, args);
    va_end(args);
    return result;
}

#ifdef _WIN32
wchar_t* ConvertToUtf8(const char* str)
{
    int wlen = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
    if(!wlen)
    {
        errno = EINVAL;
        printf("Warning: Failed to convert path to UTF8\n");
        return NULL;
    }

    wchar_t* wstr = malloc(sizeof(wchar_t) * wlen);
    if(!wstr)
    {
        printf("ConvertToUtf8: Failed to allocate new string\n");
        return NULL;
    }

    if(MultiByteToWideChar(CP_UTF8, 0, str, -1, wstr, wlen) == 0)
    {
        errno = EINVAL;
        printf("Warning: Failed to convert path to UTF8\n");
        free(wstr);
        return NULL;
    }

    return wstr;
}

char *M_OEMToUTF8(const char *oem)
{
    unsigned int len = strlen(oem) + 1;
    wchar_t *tmp;
    char *result;

    tmp = malloc(len * sizeof(wchar_t));
    MultiByteToWideChar(CP_OEMCP, 0, oem, len, tmp, len);
    result = malloc(len * 4);
    WideCharToMultiByte(CP_UTF8, 0, tmp, len, result, len * 4, NULL, NULL);
    free(tmp);

    return result;
}
#endif

//
// M_NormalizeSlashes
//
// Remove trailing slashes, translate backslashes to slashes
// The string to normalize is passed and returned in str
//
// killough 11/98: rewritten
//
// [STRIFE] - haleyjd 20110210: Borrowed from Eternity and adapted to respect
// the DIR_SEPARATOR define used by Choco Doom. This routine originated in
// BOOM.
//
void M_NormalizeSlashes(char *str)
{
    char *p;

    // Convert all slashes/backslashes to DIR_SEPARATOR
    for (p = str; *p; p++)
    {
        if ((*p == '/' || *p == '\\') && *p != DIR_SEPARATOR)
        {
            *p = DIR_SEPARATOR;
        }
    }

    // Remove trailing slashes
    while (p > str && *--p == DIR_SEPARATOR)
    {
        *p = 0;
    }

    // Collapse multiple slashes
    for (p = str; (*str++ = *p); )
    {
        if (*p++ == DIR_SEPARATOR)
        {
            while (*p == DIR_SEPARATOR)
            {
                p++;
            }
        }
    }
}

//
// RD_M_FindInternalResource
//
// [Dasperal] Returns a newly-malloced string containing the path to the given resource.
// Terminates program with an error if the resource is not found
//
char* RD_M_FindInternalResource(char* resourceName)
{
    char* retVal;
#ifndef __APPLE__
    if(!packageResourcesDir)
    {
    #ifndef _WIN32
        if(M_StringStartsWith(exedir, "/usr/"))
        {
            char* temp = M_DirName(exedir);
            char* pkg_dir = M_DirName(temp);
            packageResourcesDir = M_StringJoin(pkg_dir, DIR_SEPARATOR_S, "share", DIR_SEPARATOR_S, PACKAGE_TARNAME, DIR_SEPARATOR_S, NULL);
            free(pkg_dir);
            free(temp);
        }
        else if(M_StringStartsWith(exedir, "/opt/"))
        {
            char* temp = M_DirName(exedir);
            char* pkg_dir = M_DirName(temp);
            packageResourcesDir = M_StringJoin(pkg_dir, DIR_SEPARATOR_S, "share", DIR_SEPARATOR_S, NULL);
            free(pkg_dir);
            free(temp);
        }
        else
        {
    #endif
            packageResourcesDir = M_StringJoin(exedir, "base", DIR_SEPARATOR_S, NULL);
    #ifndef _WIN32
        }
    #endif
    }
#endif
    retVal = M_StringJoin(packageResourcesDir, resourceName, NULL);
    if(!M_FileExists(retVal))
    {
        I_QuitWithError(english_language ?
                        "Internal resource \"%s\" not found!" :
                        "Внутренний ресурс \"%s\" не найден!",
                        retVal);
        free(retVal);
        return NULL;
    }
    return retVal;
}
