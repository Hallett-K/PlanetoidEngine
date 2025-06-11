#pragma once

#include <Planetoid/PlanetoidCore.h>

#include <cstdint>
#include <functional>
#include <string>

namespace PlanetoidEngine
{
    class PE_API UUID 
    {
    public:
        UUID();
        UUID(uint64_t uuid1, uint64_t uuid2);
        UUID(const std::string& uuidString);
        UUID(const UUID& other);

        ~UUID();

        uint64_t GetUUID1() const { return m_uuid1; }
        uint64_t GetUUID2() const { return m_uuid2; }

        std::string ToString() const;

        bool operator==(const UUID& other) const
        {
            return m_uuid1 == other.m_uuid1 && m_uuid2 == other.m_uuid2;
        }

        bool operator!=(const UUID& other) const
        {
            return !(*this == other);
        }

    private:
        uint64_t m_uuid1;
        uint64_t m_uuid2;
    };
}

namespace std
{
    template<>
    struct hash<PlanetoidEngine::UUID>
    {
        size_t operator()(const PlanetoidEngine::UUID& uuid) const
        {
            return hash<uint64_t>()(uuid.GetUUID1()) ^ hash<uint64_t>()(uuid.GetUUID2());
        }
    };
}