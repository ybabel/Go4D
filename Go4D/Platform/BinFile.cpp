#include "BinFile.h"

using namespace std;

#ifdef VSNET
#include <windows.h>

namespace Go4D
{

TCHAR buf2[512];
LPCTSTR StrToUnicode2(const char * mess)
{
  MultiByteToWideChar(
            CP_ACP,
            MB_PRECOMPOSED,
            mess,
            strlen(mess)+1,
            (LPWSTR) buf2,
            sizeof(buf2));
  return buf2;
}

int convert(int aconst)
{
  switch (aconst)
  {
    case BinaryFile::beg : return FILE_BEGIN;
    case BinaryFile::end : return FILE_END;
    case BinaryFile::cur : return FILE_CURRENT;
    default : return -1;
  }
}

BinaryFile::BinaryFile(const char * fileName)
{
#ifdef UNICODE
  hFile = (void *)CreateFile(StrToUnicode2(fileName),
    GENERIC_READ, 0, NULL, OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL, NULL);
#else
  hFile = (void *)CreateFile(fileName,
    GENERIC_READ, 0, NULL, OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL, NULL);
#endif
  seekg(0, 0);
}

BinaryFile::~BinaryFile()
{
  close();
}

void BinaryFile::seekg(int count, int start)
{
  SetFilePointer((HANDLE)hFile, count, 0, convert(start));
}

int BinaryFile::tellg()
{
  LONG result;
  SetFilePointer((HANDLE)hFile, 0, &result, FILE_CURRENT);
  return int(result);
}

bool BinaryFile::eof()
{
  return FindClose((HANDLE)hFile)==S_OK;
}

int BinaryFile::read(char * buf, int size)
{
  DWORD readed;
  ReadFile((HANDLE)hFile, (void *)buf, size, &readed, NULL);
  return readed;
}

bool BinaryFile::is_open() const
{
  return hFile != NULL;
}

char BinaryFile::get()
{
  char result;
  read(&result, 1);
  return result;
}

void BinaryFile::close()
{
  CloseHandle((HANDLE)hFile);
  hFile = NULL;
}

}

#else
namespace Go4D
{

ios_base::seekdir convertdir(int aconst)
{
  switch (aconst)
  {
    case BinaryFile::beg : return fstream::beg;
    case BinaryFile::end : return fstream::end;
    case BinaryFile::cur : return fstream::cur;
    default : return fstream::beg;
  }
}

BinaryFile::BinaryFile(const char * fileName)
{
    file.open(fileName, fstream::in | fstream::out | fstream::binary);
}

BinaryFile::~BinaryFile()
{
  file.close();
}

void BinaryFile::seekg(int count, int start)
{
  file.seekg(count, convertdir(start));
}

int BinaryFile::tellg()
{
  return file.tellg();
}

bool BinaryFile::eof()
{
    return file.eof();
}

int BinaryFile::read(char * buf, int size)
{
  return file.read(buf, size).gcount();
}

bool BinaryFile::is_open() const
{
  return file;is_open();
}

char BinaryFile::get()
{
  return file.get();
}

void BinaryFile::close()
{
  file.close();
}

}

#endif
