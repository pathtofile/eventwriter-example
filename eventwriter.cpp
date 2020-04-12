// eventwriter.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Windows.h>
#include <stdio.h>
#include <evntprov.h>
#include <fstream>
#include "provider.h"

#define AUTO_RAW_PATH "temp.man"
#define AUTO_RAW_IM_CMD "wevtutil im temp.man"
#define AUTO_RAW_UM_CMD "wevtutil um temp.man"

int auto_wevtutil_install() {
    HRESULT err;
    HMODULE hModule = GetModuleHandle(NULL);
    if (hModule == INVALID_HANDLE_VALUE) {
        return 1;
    }
    HRSRC hResource = FindResourceW(hModule, L"PROVIDER_RAW", L"PROVIDER_RAW"); // substitute RESOURCE_ID and RESOURCE_TYPE.
    if (hResource == NULL) {
        err = GetLastError();
        printf("Error: %d", err);
        return __LINE__;
    }
    HGLOBAL hMemory = LoadResource(hModule, hResource);
    if (hMemory == NULL) {
        err = GetLastError();
        printf("Error: %d", err);
        return __LINE__;
    }
    DWORD dwSize = SizeofResource(hModule, hResource);
    LPVOID lpAddress = LockResource(hMemory);

    char* bytes = new char[dwSize];
    memcpy(bytes, lpAddress, dwSize);
    if (bytes[0] == NULL) {
        err = GetLastError();
        printf("Error: %d", err);
        return __LINE__;
    }

    // Write XML to file
    std::ofstream fout;
    fout.open(AUTO_RAW_PATH, std::ios::binary | std::ios::out);
    fout.write(bytes, dwSize);
    fout.close();

    // Call wevtutil to install
    err = system(AUTO_RAW_IM_CMD);
    if (err != 0) {
        printf("Error: %d", err);
        return __LINE__;
    }

    return 0;
}

void auto_wevtutil_uninstall() {
    HRESULT err;
    err = system(AUTO_RAW_UM_CMD);
    if (err != 0) {
        printf("Error: %d", err);
    }
    // Delete temp file
    if (!DeleteFileA(AUTO_RAW_PATH)) {
        err = GetLastError();
        printf("Error: %d", err);
    }
}

int main()
{
    DWORD status = 0;
    // IF needed, run a dodgy an install the provider ourself
    status = auto_wevtutil_install();
    if (status != ERROR_SUCCESS) {
        printf("Error %ul line %d\n", status, __LINE__);
        return status;
    }

    status = EventRegistereventwriter();
    if (status != ERROR_SUCCESS) {
        printf("Error %ul line %d\n", status, __LINE__);
        goto cleanup;
    }

    status = EventWriteEW_EVENTA(L"BBBB");
    if (status != ERROR_SUCCESS) {
        printf("Error %ul line %d\n", status, __LINE__);
        goto cleanup;
    }

    status = EventUnregistereventwriter();
    if (status != ERROR_SUCCESS) {
        printf("Error %ul line %d\n", status, __LINE__);
        goto cleanup;
    }

cleanup:
    // IF needed, uninstall our provider
    auto_wevtutil_uninstall();
    return status;
}
