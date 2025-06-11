#pragma once

#include <Planetoid/PlanetoidCore.h>
#include <Planetoid/Core/Log.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <shlobj.h>
#include <sstream>
#include <string>
#include <vector>
#include <windows.h>

namespace PlanetoidEngine
{
    class File
    {
    public:
        File() {}
        File(const std::string& path)
            : m_Path(path) { }
        File(const File& other)
            : m_Path(other.m_Path) { }

        ~File()
        {

        }

        std::string GetPath() const { return m_Path; }
        std::string GetFileName() const
        {
            std::string fileName = m_Path;
            std::replace(fileName.begin(), fileName.end(), '\\', '/');
            size_t lastSlashIndex = fileName.find_last_of('/');
            if (lastSlashIndex != std::string::npos)
                fileName = fileName.substr(lastSlashIndex + 1);
            return fileName;
        }

        std::string GetFileNameWithoutExtension() const
        {
            std::string fileName = GetFileName();
            size_t lastDotIndex = fileName.find_last_of('.');
            if (lastDotIndex != std::string::npos)
                fileName = fileName.substr(0, lastDotIndex);
            return fileName;
        }

        std::string GetExtension() const
        {
            std::string fileName = GetFileName();
            size_t lastDotIndex = fileName.find_last_of('.');
            if (lastDotIndex != std::string::npos)
                fileName = fileName.substr(lastDotIndex + 1);
            return fileName;
        }

        std::string ReadTextContentsToString() const
        {
            std::ifstream file(m_Path);
            std::stringstream buffer;
            buffer << file.rdbuf();
            file.close();
            return buffer.str();
        }

        std::vector<std::string> ReadTextContentsToStringArray() const
        {
            std::ifstream file(m_Path);
            std::vector<std::string> lines;
            std::string line;
            while (std::getline(file, line))
                lines.push_back(line);
            file.close();
            return lines;
        }

        void WriteTextContents(const std::string& contents) const
        {
            std::ofstream file(m_Path);
            file << contents;
            file.close();
        }

        void WriteTextContents(const std::vector<std::string>& contents) const
        {
            std::ofstream file(m_Path);
            for (const std::string& line : contents)
                file << line << '\n';
            file.close();
        }

        void AppendTextContents(const std::string& contents) const
        {
            std::ofstream file(m_Path, std::ios_base::app);
            file << contents;
            file.close();
        }

        void AppendTextContents(const std::vector<std::string>& contents) const
        {
            std::ofstream file(m_Path, std::ios_base::app);
            for (const std::string& line : contents)
                file << line << std::endl;
            file.close();
        }

    private:
        std::string m_Path;
    };


    class FileIO
    {
    public:
        static bool FileExists(const std::string& path)
        {
            std::ifstream file(path);
            bool exists = file.good();
            if (exists)
                file.close();
            return file.good();
        }

        static bool FolderExists(const std::string& path)
        {
            DWORD dwAttrib = GetFileAttributesA(path.c_str());

            return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
                (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
        }

        static void FileCreate(const std::string& path)
        {
            if (FileExists(path))
                return;
            std::ofstream file(path);
            file.close();
        }

        static void FolderCreate(const std::string& path)
        {
            if (FolderExists(path))
                return;
            CreateDirectoryA(path.c_str(), NULL);
        }

        static File Open(const std::string& path)
        {
            if (!FileExists(path))
                FileCreate(path);
            return File(path);
        }

        static void FileCopy(const std::string& from, const std::string& to)
        {
            try {
                // Check if the source file exists
                if (!std::filesystem::exists(from)) {
                    std::cerr << "Source file does not exist: " << from << std::endl;
                    return;
                }

                // Open the source file in binary mode
                std::ifstream sourceStream(from, std::ios::binary);
                if (!sourceStream.is_open()) {
                    std::cerr << "Failed to open source file: " << from << std::endl;
                    return;
                }

                // Read the contents of the source file into a vector
                std::vector<char> buffer((std::istreambuf_iterator<char>(sourceStream)), std::istreambuf_iterator<char>());

                // Close the source file
                sourceStream.close();

                // Open the destination file in binary mode
                std::ofstream destStream(to, std::ios::binary);
                if (!destStream.is_open()) {
                    std::cerr << "Failed to open destination file: " << to << std::endl;
                    return;
                }

                // Write the buffer to the destination file
                destStream.write(buffer.data(), buffer.size());

                // Close the destination file
                destStream.close();

                // Log a success message
                std::cout << "DLL file copied successfully: " << from << " -> " << to << std::endl;
            } catch (const std::exception& e) {
                // Log an error if an exception occurs
                std::cerr << "Error during copy operation: " << e.what() << std::endl;
            }
        }

        static void FileDelete(const std::string& path)
        {
            std::filesystem::remove(path);
        }

        static std::string PickFolder(const std::string& title)
        {
            CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

            IFileDialog *pFileDialog;
            HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFileDialog));

            std::string folderPath;

            if (SUCCEEDED(hr)) {
                DWORD dwOptions;
                pFileDialog->GetOptions(&dwOptions);
                pFileDialog->SetOptions(dwOptions | FOS_PICKFOLDERS);
                pFileDialog->SetTitle(UTF8ToWStr(title).c_str());

                hr = pFileDialog->Show(NULL);

                if (SUCCEEDED(hr)) {
                    IShellItem *pItem;
                    hr = pFileDialog->GetResult(&pItem);

                    if (SUCCEEDED(hr)) {
                        LPWSTR pszFolderPath;
                        hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFolderPath);

                        if (SUCCEEDED(hr)) {
                            folderPath = WStrToUTF8(pszFolderPath);
                            CoTaskMemFree(pszFolderPath);
                        }

                        pItem->Release();
                    }
                }

                pFileDialog->Release();
            }

            CoUninitialize();

            std::replace(folderPath.begin(), folderPath.end(), '\\', '/');

            return folderPath;
        }

        static std::string PickFile(const std::string& title, const std::string& filterFileName, const std::string& filterFileExtension)
        {
            CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

            IFileDialog *pFileDialog;
            HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFileDialog));

            std::string filePath;

            if (SUCCEEDED(hr)) {
                DWORD dwOptions;
                pFileDialog->GetOptions(&dwOptions);
                pFileDialog->SetOptions(dwOptions);
                pFileDialog->SetTitle(UTF8ToWStr(title).c_str());

                std::wstring wideFilterFileName = UTF8ToWStr(filterFileName);
                std::wstring wideFilterFileExtension = UTF8ToWStr(filterFileExtension);
                COMDLG_FILTERSPEC rgSpec[] =
                {
                    { wideFilterFileName.c_str(), wideFilterFileExtension.c_str() },
                };
                pFileDialog->SetFileTypes(1, rgSpec);
                pFileDialog->SetFileTypeIndex(1);

                hr = pFileDialog->Show(NULL);

                if (SUCCEEDED(hr)) {
                    IShellItem *pItem;
                    hr = pFileDialog->GetResult(&pItem);

                    if (SUCCEEDED(hr)) {
                        LPWSTR pszFilePath;
                        hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                        if (SUCCEEDED(hr)) {
                            filePath = WStrToUTF8(pszFilePath);
                            CoTaskMemFree(pszFilePath);
                        }

                        pItem->Release();
                    }
                }

                pFileDialog->Release();
            }

            CoUninitialize();

            std::replace(filePath.begin(), filePath.end(), '\\', '/');

            return filePath;
        }

        static std::string WStrToUTF8(const wchar_t* wstr) 
        {
            int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
            std::string utf8_str(size_needed, 0);
            WideCharToMultiByte(CP_UTF8, 0, wstr, -1, &utf8_str[0], size_needed, NULL, NULL);
            
            // Remove any null characters at the end
            utf8_str.resize(strlen(utf8_str.c_str()));
            
            return utf8_str;
        }

        static std::string WStrToUTF8NonNullTerminated(const wchar_t* wstr, int size) 
        {
            int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr, size, NULL, 0, NULL, NULL);
            std::string utf8_str(size_needed, 0);
            WideCharToMultiByte(CP_UTF8, 0, wstr, size, &utf8_str[0], size_needed, NULL, NULL);
            
            return utf8_str;
        }

        static std::wstring UTF8ToWStr(const std::string& str)
        {
            int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
            std::wstring wstr(size_needed, 0);
            MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstr[0], size_needed);
            return wstr;
        }

    private:
        
    };
}