#pragma once

#include <Planetoid/PlanetoidCore.h>

#include <Planetoid/Core/Delegate.h>
#include <Planetoid/Core/FileIO.h>

#include <filesystem>
#include <unordered_map>
#include <vector>
#include <mutex>
namespace PlanetoidEngine
{
    DECLARE_DELEGATE(FileChangedDelegate, const std::string&);
    class PE_API VirtualFileSystem
    {
    public:
        VirtualFileSystem();
        ~VirtualFileSystem();

        void Mount(const std::string& mountPoint, const std::string& path);
        void Unmount(const std::string& mountPoint);

        // Converts a path to a virtual path
        std::string ResolvePath(const std::string& path) const;

        // Converts a virtual path to a path
        std::string ResolveVirtualPath(const std::string& virtualPath) const;

        bool IsMounted(const std::string& mountPoint) const { return m_mountPoints.find(mountPoint) != m_mountPoints.end(); }

        std::string GetMountPoint(const std::string& path) const;

        std::vector<std::string> GetFiles(const std::string& path) const;
        std::vector<std::string> GetFolders(const std::string& path) const;


        void WatchFile(const std::string& path);
        void UnwatchFile(const std::string& path);
        void UpdateWatchedFiles();
        FileChangedDelegate OnWatchedFileChanged;

    private:
        std::unordered_map<std::string, std::string> m_mountPoints;
        std::unordered_map<std::string, std::filesystem::file_time_type> m_watchedFiles;
        std::mutex m_watchedFilesMutex;
    };
}