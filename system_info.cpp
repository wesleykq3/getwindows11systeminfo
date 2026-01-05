#include <windows.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <psapi.h>

// 获取Windows版本信息
std::string GetWindowsVersion() {
    HKEY hKey;
    DWORD dwType, dwSize = 256;
    char buffer[256];
    
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, 
                     "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 
                     0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        
        if (RegQueryValueEx(hKey, "ProductName", NULL, &dwType, 
                           (LPBYTE)buffer, &dwSize) == ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return std::string(buffer);
        }
        RegCloseKey(hKey);
    }
    return "Unknown Windows Version";
}

// 获取磁盘空间信息
void GetDiskSpace(const char* drivePath) {
    ULARGE_INTEGER freeBytes, totalBytes, totalFreeBytes;
    
    if (GetDiskFreeSpaceEx(drivePath, &freeBytes, &totalBytes, &totalFreeBytes)) {
        std::cout << "Drive " << drivePath << ":\n";
        std::cout << "  Total: " << std::fixed << std::setprecision(2) 
                  << (double)totalBytes.QuadPart / (1024 * 1024 * 1024) << " GB\n";
        std::cout << "  Free: " << std::fixed << std::setprecision(2) 
                  << (double)freeBytes.QuadPart / (1024 * 1024 * 1024) << " GB\n";
        std::cout << "  Used: " << std::fixed << std::setprecision(2) 
                  << (double)(totalBytes.QuadPart - freeBytes.QuadPart) / (1024 * 1024 * 1024) 
                  << " GB (" << std::setprecision(1)
                  << (100.0 * (totalBytes.QuadPart - freeBytes.QuadPart) / totalBytes.QuadPart)
                  << "%)\n";
    } else {
        std::cout << "Cannot get disk info for " << drivePath << "\n";
    }
    std::cout << std::endl;
}

// 获取内存信息
void GetMemoryInfo() {
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    
    if (GlobalMemoryStatusEx(&memInfo)) {
        std::cout << "Memory Info:\n";
        std::cout << "  Total Physical: " << std::fixed << std::setprecision(2) 
                  << (double)memInfo.ullTotalPhys / (1024 * 1024 * 1024) << " GB\n";
        std::cout << "  Available Physical: " << std::fixed << std::setprecision(2) 
                  << (double)memInfo.ullAvailPhys / (1024 * 1024 * 1024) << " GB\n";
        std::cout << "  Used Physical: " << std::fixed << std::setprecision(2) 
                  << (double)(memInfo.ullTotalPhys - memInfo.ullAvailPhys) / (1024 * 1024 * 1024)
                  << " GB (" << std::setprecision(1)
                  << (100.0 * (memInfo.ullTotalPhys - memInfo.ullAvailPhys) / memInfo.ullTotalPhys)
                  << "%)\n";
        std::cout << "  Total Virtual: " << std::fixed << std::setprecision(2) 
                  << (double)memInfo.ullTotalVirtual / (1024 * 1024 * 1024) << " GB\n";
        std::cout << "  Available Virtual: " << std::fixed << std::setprecision(2) 
                  << (double)memInfo.ullAvailVirtual / (1024 * 1024 * 1024) << " GB\n";
    }
    std::cout << std::endl;
}

int main() {
    std::cout << "=== Windows System Information Tool ===\n\n";
    
    std::string version = GetWindowsVersion();
    std::cout << "OS: " << version << "\n\n";
    
    std::cout << "=== Memory Information ===\n";
    GetMemoryInfo();
    
    std::cout << "=== Disk Information ===\n";
    // 获取所有驱动器
    DWORD drives = GetLogicalDrives();
    char driveLetter[] = "A:\\";
    
    for (int i = 0; i < 26; i++) {
        if (drives & (1 << i)) {
            driveLetter[0] = 'A' + i;
            DWORD type = GetDriveType(driveLetter);
            
            if (type == DRIVE_FIXED) {
                GetDiskSpace(driveLetter);
            }
        }
    }
    
    std::cout << "Press any key to exit...";
    std::cin.get();
    
    return 0;
}