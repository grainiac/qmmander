/****************************************************************************
 **
 ** This file is part of the Zip Utils:
 **
 ** Qmmander uses the Zip Utils for file compression.
 **
 ** The Zip Utils were created by Lucian Wischik (ljw1004)
 ** and published on CodeProject.
 **
 ** Article:  Zip Utils - clean, elegant, simple, C++/Win32
 ** Date:     06/24/2004
 ** URL:      http://www.codeproject.com/KB/files/zip_utils.aspx
 **
 ** License      : Public Domain
 ** License Infos: http://creativecommons.org/licenses/publicdomain/
 **
 ****************************************************************************/
#define _UNICODE
#include "modify.h"

ZRESULT RemoveFileFromZip(const TCHAR *zipfn, const TCHAR *zename)
{
    return AddFileToZip(zipfn, zename, 0);
}

ZRESULT AddFileToZip(const TCHAR *zipfn, const TCHAR *zename, const TCHAR *zefn)
{
    if (GetFileAttributes(zipfn) == 0xFFFFFFFF || (zefn != 0 && GetFileAttributes(zefn) == 0xFFFFFFFF))
    {
        return ZR_NOFILE;
    }
    // Expected size of the new zip will be the size of the old zip plus the size of the new file
    HANDLE hf = CreateFile(zipfn, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if (hf == INVALID_HANDLE_VALUE)
    {
        return ZR_NOFILE;
    }
    DWORD size = GetFileSize(hf, 0);
    CloseHandle(hf);
    if (zefn != 0)
    {
        hf = CreateFile(zefn, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
        if (hf == INVALID_HANDLE_VALUE)
        {
            return ZR_NOFILE;
        }
        size += GetFileSize(hf, 0);
        CloseHandle(hf);
    }
    size *= 2; // just to be on the safe side.
    //
    HZIP hzsrc = OpenZip(zipfn, 0);
    if (hzsrc == 0)
    {
        return ZR_READ;
    }
    HZIP hzdst = CreateZip(0, size, 0);
    if (hzdst == 0)
    {
        CloseZip(hzsrc);
        return ZR_WRITE;
    }
    // hzdst is created in the system pagefile
    // Now go through the old zip, unzipping each item into a memory buffer, and adding it to the new one
    char *buf = 0;
    unsigned int bufsize = 0; // we'll unzip each item into this memory buffer
    ZIPENTRY ze;
    ZRESULT zr = GetZipItem(hzsrc,  - 1, &ze);
    int numitems = ze.index;
    if (zr != ZR_OK)
    {
        CloseZip(hzsrc);
        CloseZip(hzdst);
        return zr;
    }
    for (int i = 0; i < numitems; i++)
    {
        zr = GetZipItem(hzsrc, i, &ze);
        if (zr != ZR_OK)
        {
            CloseZip(hzsrc);
            CloseZip(hzdst);
            return zr;
        }
        if (_tcsicmp(ze.name, zename) == 0)
        {
            continue;
        }
        // don't copy over the old version of the file we're changing
        if (ze.attr &FILE_ATTRIBUTE_DIRECTORY)
        {
            zr = ZipAddFolder(hzdst, ze.name);
            if (zr != ZR_OK)
            {
                CloseZip(hzsrc);
                CloseZip(hzdst);
                return zr;
            }
            continue;
        }
        if (ze.unc_size > (long)bufsize)
        {
            if (buf != 0)
            {
                delete [] buf;
            }
            bufsize = ze.unc_size * 2;
            buf = new char[bufsize];
        }
        zr = UnzipItem(hzsrc, i, buf, bufsize);
        if (zr != ZR_OK)
        {
            CloseZip(hzsrc);
            CloseZip(hzdst);
            return zr;
        }
        zr = ZipAdd(hzdst, ze.name, buf, bufsize);
        if (zr != ZR_OK)
        {
            CloseZip(hzsrc);
            CloseZip(hzdst);
            return zr;
        }
    }
    delete [] buf;
    // Now add the new file
    if (zefn != 0)
    {
        zr = ZipAdd(hzdst, zename, zefn);
        if (zr != ZR_OK)
        {
            CloseZip(hzsrc);
            CloseZip(hzdst);
            return zr;
        }
    }
    zr = CloseZip(hzsrc);
    if (zr != ZR_OK)
    {
        CloseZip(hzdst);
        return zr;
    }
    //
    // The new file has been put into pagefile memory. Let's store it to disk, overwriting the original zip
    zr = ZipGetMemory(hzdst, (void **) &buf, &size);
    if (zr != ZR_OK)
    {
        CloseZip(hzdst);
        return zr;
    }
    hf = CreateFile(zipfn, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    if (hf == INVALID_HANDLE_VALUE)
    {
        CloseZip(hzdst);
        return ZR_WRITE;
    }
    DWORD writ;
    WriteFile(hf, buf, size, &writ, 0);
    CloseHandle(hf);
    zr = CloseZip(hzdst);
    if (zr != ZR_OK)
    {
        return zr;
    }
    return ZR_OK;
}
