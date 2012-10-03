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
#ifndef MODIFY_H
#define MODIFY_H
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include "zip.h"
#include "unzip.h"

ZRESULT RemoveFileFromZip(const TCHAR *zipfn, const TCHAR *zename);

ZRESULT AddFileToZip(const TCHAR *zipfn, const TCHAR *zename, const TCHAR *zefn);

// This program shows how to modify an existing zipfile --
// add to it, remove from it
// AddFileToZip: eg. zipfn="c:\\archive.zip", zefn="c:\\docs\\file.txt", zename="file.txt"
// If the zipfile already contained something called zename (case-insensitive), it is removed.
// These two functions are defined below.
/*
void main()
{ printf("This program shows how to modify an existing zipfile -- add to it, remove from it.\n");
// First we'll create some small files
const char *s; FILE *f;
CreateDirectory("\\z",0);
s="a contents\r\n"; f=fopen("\\z\\a.txt","wt"); fputs(s,f); fclose(f);
s="b stuff\r\n";    f=fopen("\\z\\b.txt","wt"); fputs(s,f); fclose(f);
s="c something\r\n";f=fopen("\\z\\c.txt","wt"); fputs(s,f); fclose(f);
s="c fresh\r\n";    f=fopen("\\z\\c2.txt","wt");fputs(s,f); fclose(f);
s="d up\r\n";       f=fopen("\\z\\d.txt","wt"); fputs(s,f); fclose(f);
// and create a zip file to be working with
printf("Creating '\\z\\modify.zip' with zna.txt, znb.txt, znc.txt and a folder...\n");
HZIP hz = CreateZip("\\z\\modify.zip",0);
ZipAdd(hz,"zna.txt","\\z\\a.txt");
ZipAdd(hz,"znb.txt","\\z\\b.txt");
ZipAdd(hz,"znc.txt","\\z\\c.txt");
ZipAddFolder(hz,"znsub");
CloseZip(hz);
printf("Adding znsub\\znd.txt to the zip file...\n");
AddFileToZip("\\z\\modify.zip","znsub\\znd.txt","\\z\\d.txt");
printf("Removing znb.txt from the zip file...\n");
RemoveFileFromZip("\\z\\modify.zip","znb.txt");
printf("Updating znc.txt in the zip file...\n");
AddFileToZip("\\z\\modify.zip","znc.txt","\\z\\c2.txt");
return;
}*/
// AddFileToZip: adds a file to a zip, possibly replacing what was there before
// zipfn ="c:\\archive.zip"             (the fn of the zip file)
// zefn  ="c:\\my documents\\file.txt"  (the fn of the file to be added)
// zename="file.txt"                    (the name that zefn will take inside the zip)
// If zefn is empty, we just delete zename from the zip archive.
// The way it works is that we create a temporary zipfile, and copy the original
// contents into the new one (with the appropriate addition or substitution)
// and then remove the old one and rename the new one. NB. we are case-insensitive.
#endif // MODIFY_H
