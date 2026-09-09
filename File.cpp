#include "File.h"

#include <iostream>


bool File::OpenForRead(std::string fileName){

    fileHandle = CreateFileA(
        fileName.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (fileHandle == INVALID_HANDLE_VALUE){

        std::cout << "Failed to open file for reading. Error: "
            << GetLastError()
            << "\n";

        return false;
    }

    return true;
}

bool File::OpenForWrite(std::string fileName){

    fileHandle = CreateFileA(
        fileName.c_str(),
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );

    if (fileHandle == INVALID_HANDLE_VALUE){
        std::cout << "Failed to open file for writing. Error: "
            << GetLastError()
            << "\n";

        return false;
    }

    return true;
}

bool File::Read(char* buffer,DWORD bufferSize,DWORD& bytesRead){

    bool result = ReadFile(
        fileHandle,
        buffer,
        bufferSize,
        &bytesRead,
        NULL
    );


    if (!result){

        std::cout << "Failed to read file. Error: "
            << GetLastError()
            << "\n";

        return false;
    }

    return true;
}

bool File::Write(const char* buffer,DWORD bufferSize,DWORD& bytesWritten){

    bool result = WriteFile(
        fileHandle,
        buffer,
        bufferSize,
        &bytesWritten,
        NULL
    );

    if (!result){

        std::cout << "Failed to write file. Error: "
            << GetLastError()
            << "\n";

        return false;
    }

    return true;
}

void File::Close(){

    if (fileHandle != INVALID_HANDLE_VALUE){
        CloseHandle(fileHandle);

        fileHandle = INVALID_HANDLE_VALUE;
    }
}

bool File::CreateDirectory(std::string path){

    BOOL result = CreateDirectoryA(path.c_str(),NULL);

    if (result)
        return true;

    DWORD error = GetLastError();

    if (error == ERROR_ALREADY_EXISTS)
        return true;


    std::cout << "Failed to create directory. Error: "
        << error
        << "\n";


    return false;
}