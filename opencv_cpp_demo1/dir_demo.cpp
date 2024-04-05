#include <iostream>
#include <string>
#include <windows.h>

void listFiles(const std::string& path) {
    std::string searchPath = path + "\\*.*";
    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);

    if (hFind == INVALID_HANDLE_VALUE) {
        std::cerr << "Error opening directory: " << path << std::endl;
        return;
    }

    do {
        std::string filename = findData.cFileName;

        // Skip "." and ".." entries
        if (filename != "." && filename != "..") {
            std::cout << filename << std::endl;

            // Check if it's a directory and recurse if necessary
            if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                if (filename != "." && filename != "..") {
                    std::string subdirectory = path + "\\" + filename;
                    listFiles(subdirectory);
                }
            }
        }
    } while (FindNextFileA(hFind, &findData) != 0);

    FindClose(hFind);
}

int main() {
    std::string directoryPath = "../../data/Sample1";

    listFiles(directoryPath);

    return 0;
}
