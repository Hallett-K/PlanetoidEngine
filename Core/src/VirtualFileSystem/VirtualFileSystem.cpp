#include <Planetoid/VirtualFileSystem/VirtualFileSystem.h>

#include <filesystem>

namespace PlanetoidEngine
{
    VirtualFileSystem::VirtualFileSystem()
    {

    }

    VirtualFileSystem::~VirtualFileSystem()
    {

    }

    void VirtualFileSystem::Mount(const std::string& mountPoint, const std::string& path)
    {
        std::string lowercasePath = path;
        std::transform(lowercasePath.begin(), lowercasePath.end(), lowercasePath.begin(), [](unsigned char c) { return std::tolower(c); });
        m_mountPoints[mountPoint] = lowercasePath;
    }

    void VirtualFileSystem::Unmount(const std::string& mountPoint)
    {
        m_mountPoints.erase(mountPoint);
    }

    std::string VirtualFileSystem::ResolvePath(const std::string& path) const
    {
        std::string lowerpath = path;
        std::transform(lowerpath.begin(), lowerpath.end(), lowerpath.begin(), [](unsigned char c) { return std::tolower(c); });
        size_t longestMatchLength = 0;
        std::string virtualPath;

        // Iterate through all mount points
        for (const auto& pair : m_mountPoints) 
        {
            const std::string& mountPoint = pair.first;
            const std::string& mountPath = pair.second;

            // Check if the path starts with the mount point
            if (lowerpath.compare(0, mountPath.length(), mountPath) == 0) 
            {
                // Update if the current mount point is a longer match
                if (mountPath.length() > longestMatchLength) 
                {
                    longestMatchLength = mountPath.length();
                    virtualPath = mountPoint;
                }
            }
        }

        // If a match is found, append the relative path
        if (!virtualPath.empty()) 
        {
            virtualPath += lowerpath.substr(longestMatchLength);
        }
        else
        {
            // If no match is found, return the path as-is
            virtualPath = lowerpath;
        }

        return virtualPath;
    }

    std::string VirtualFileSystem::ResolveVirtualPath(const std::string& path) const
    {
        size_t longestMatchLength = 0;
        std::string physicalPath;

        // Iterate through all mount points
        for (const auto& pair : m_mountPoints) {
            const std::string& mountPoint = pair.first;
            const std::string& mountPath = pair.second;

            if (path.compare(0, mountPoint.length(), mountPoint) == 0) 
            {
                if (mountPoint.length() > longestMatchLength) 
                {
                    longestMatchLength = mountPoint.length();
                    physicalPath = mountPath;
                }
            }
        }

        if (!physicalPath.empty()) 
        {
            physicalPath += path.substr(longestMatchLength);
        }
        else
        {
            physicalPath = path;
        }

        return physicalPath;
    }

    std::string VirtualFileSystem::GetMountPoint(const std::string& path) const
    {
        if (m_mountPoints.find(path) != m_mountPoints.end())
        {
            return m_mountPoints.at(path);
        }
        else
        {
            return "";
        }
    }

    std::vector<std::string> VirtualFileSystem::GetFiles(const std::string& path) const
    {
        std::vector<std::string> files;
        std::string physicalPath = ResolveVirtualPath(path);
        if (physicalPath.empty())
        {
            return files;
        }

        for (const auto& entry : std::filesystem::directory_iterator(physicalPath))
        {
            if (entry.is_regular_file())
            {
                std::string fileName = entry.path().string();
                std::replace(fileName.begin(), fileName.end(), '\\', '/');
                size_t lastSlashIndex = fileName.find_last_of('/');
                if (lastSlashIndex != std::string::npos)
                    fileName = fileName.substr(lastSlashIndex + 1);
                files.push_back(fileName);
            }
        }

        return files;
    }

    std::vector<std::string> VirtualFileSystem::GetFolders(const std::string& path) const
    {
        std::vector<std::string> folders;
        std::string physicalPath = ResolveVirtualPath(path);
        if (physicalPath.empty())
        {
            return folders;
        }

        for (const auto& entry : std::filesystem::directory_iterator(physicalPath))
        {
            if (entry.is_directory())
            {
                // Push the folder name without the path
                std::string folderName = entry.path().string();
                std::replace(folderName.begin(), folderName.end(), '\\', '/');
                size_t lastSlashIndex = folderName.find_last_of('/');
                if (lastSlashIndex != std::string::npos)
                    folderName = folderName.substr(lastSlashIndex + 1);
                folders.push_back(folderName);
            }
        }

        return folders;
    }

    void VirtualFileSystem::WatchFile(const std::string& path)
    {
        std::string physicalPath = ResolveVirtualPath(path);
        if (physicalPath.empty())
        {
            return;
        }

        std::lock_guard<std::mutex> lock(m_watchedFilesMutex);
        if (m_watchedFiles.find(path) != m_watchedFiles.end())
        {
            return;
        }

        m_watchedFiles[path] = std::filesystem::last_write_time(physicalPath);
    }

    void VirtualFileSystem::UnwatchFile(const std::string& path)
    {
        std::lock_guard<std::mutex> lock(m_watchedFilesMutex);
        m_watchedFiles.erase(path);
    }

    void VirtualFileSystem::UpdateWatchedFiles()
    {
        std::lock_guard<std::mutex> lock(m_watchedFilesMutex);
        
        // Create a copy of the watched files to avoid iterator invalidation
        auto watchedFilesCopy = m_watchedFiles;
        
        for (auto& pair : watchedFilesCopy)
        {
            const std::string& path = pair.first;
            const std::filesystem::file_time_type& lastWriteTime = pair.second;

            std::string physicalPath = ResolveVirtualPath(path);
            if (physicalPath.empty())
            {
                continue;
            }

            if (!PlanetoidEngine::FileIO::FileExists(physicalPath))
            {
                continue;
            }

            std::string parentPath = physicalPath.substr(0, physicalPath.find_last_of('/'));
            std::string fileName = physicalPath.substr(physicalPath.find_last_of('/') + 1);

            HANDLE hDir = CreateFile(
            parentPath.c_str(),
            FILE_LIST_DIRECTORY,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
            NULL,
            OPEN_EXISTING,
            FILE_FLAG_BACKUP_SEMANTICS,
            NULL
            );

            if (hDir == INVALID_HANDLE_VALUE) {
                PlanetoidEngine::Log::Error("Failed to open directory " + parentPath + " for watching");
                continue;
            }

            DWORD dwBytesReturned;
            char buffer[1024];

            /*if (ReadDirectoryChangesW(
                hDir,
                buffer,
                sizeof(buffer),
                FALSE,
                FILE_NOTIFY_CHANGE_LAST_WRITE,
                &dwBytesReturned,
                NULL,
                NULL))
            {
                FILE_NOTIFY_INFORMATION* fileInfo = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(buffer);

                do {
                    PlanetoidEngine::Log::Info("File changed: " + path);
                    std::string changedFileName = PlanetoidEngine::FileIO::WStrToUTF8NonNullTerminated(fileInfo->FileName, fileInfo->FileNameLength / sizeof(wchar_t));
                    if (changedFileName.compare(fileName) == 0)
                    {
                        if (fileInfo->Action == FILE_ACTION_REMOVED || PlanetoidEngine::FileIO::FileExists(physicalPath) == false)
                        {
                            break;
                        }
                        std::filesystem::file_time_type newLastWriteTime = std::filesystem::last_write_time(physicalPath);
                        if (newLastWriteTime != lastWriteTime && fileInfo->Action == FILE_ACTION_MODIFIED)
                        {
                            PlanetoidEngine::Log::Info("File changed: " + path);
                            // Release the mutex before broadcasting to avoid potential deadlocks
                            m_watchedFilesMutex.unlock();
                            OnWatchedFileChanged.Broadcast(path);
                            m_watchedFilesMutex.lock();
                            
                            // Update the timestamp in our copy
                            m_watchedFiles[path] = newLastWriteTime;
                        }
                    }
                    fileInfo = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(reinterpret_cast<char*>(fileInfo) + fileInfo->NextEntryOffset);
                } while (fileInfo->NextEntryOffset != 0);
            }*/

           std::filesystem::file_time_type newLastWriteTime = std::filesystem::last_write_time(physicalPath);
           if (newLastWriteTime != lastWriteTime)
           {
                PlanetoidEngine::Log::Info("File changed: " + path);
                // Release the mutex before broadcasting to avoid potential deadlocks
                m_watchedFilesMutex.unlock();
                OnWatchedFileChanged.Broadcast(path);
                m_watchedFilesMutex.lock();
                m_watchedFiles[path] = newLastWriteTime;
           }
            CloseHandle(hDir);
        }
    }
}