// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <windows.h>

// Function to patch the memory
void PatchMemory(HANDLE hProcess, LPCVOID address, LPCVOID patch, SIZE_T size) {
    DWORD oldProtect;
    // Change the protection to allow writing
    VirtualProtectEx(hProcess, (LPVOID)address, size, PAGE_EXECUTE_READWRITE, &oldProtect);
    // Write the patch
    WriteProcessMemory(hProcess, (LPVOID)address, patch, size, NULL);
    // Restore the old protection
    VirtualProtectEx(hProcess, (LPVOID)address, size, oldProtect, &oldProtect);
}

// DLL entry point
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        // Get the handle to the current process
        HANDLE hProcess = GetCurrentProcess();

        // Get the base address of the module (DEM2.exe)
        HMODULE hExeModule = GetModuleHandle(NULL);
        if (hExeModule == NULL) {
            return FALSE;
        }

        // Calculate the addresses to patch
        BYTE* addr1 = (BYTE*)hExeModule + 0x50C081;
        BYTE* addr2 = (BYTE*)hExeModule + 0x50C0B1;

        // Define the patches
        BYTE patch1[] = { 0xc6, 0x44, 0x24, 0x0f, 0x01 }; // sets the initial value of the debug boolean to true
        BYTE patch2[] = { 0xeb, 0xf7 }; // skips the code that sets the debug boolean to false

        // Apply the patches
        PatchMemory(hProcess, addr1, patch1, sizeof(patch1));
        PatchMemory(hProcess, addr2, patch2, sizeof(patch2));
    }
    return TRUE;
}
