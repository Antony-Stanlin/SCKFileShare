#pragma once

#include <Windows.h>

#include <string>


class File
{

private:

    HANDLE fileHandle = INVALID_HANDLE_VALUE;


public:

    bool OpenForRead(std::string fileName);

    bool OpenForWrite(std::string fileName);

    bool Read(char* buffer, DWORD bufferSize, DWORD& bytesRead);

    bool Write(const char* buffer, DWORD bufferSize, DWORD& bytesWritten);

    void Close();

    static bool CreateDirectory(std::string path);

};