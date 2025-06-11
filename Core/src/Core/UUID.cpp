#include <Planetoid/Core/UUID.h>

#include <chrono>
#include <iomanip>
#include <random>
#include <sstream>

static std::random_device rd;
static std::mt19937_64 gen(rd());
static std::uniform_int_distribution<uint64_t> dis;

namespace PlanetoidEngine
{
    UUID::UUID()
    {
        // Current date and time as an unsigned 64-bit integer
        m_uuid1 = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        m_uuid2 = dis(gen);
    }

    UUID::UUID(uint64_t uuid1, uint64_t uuid2)
        : m_uuid1(uuid1), m_uuid2(uuid2)
    {
    }

    UUID::UUID(const std::string& uuidString)
    {
        // Split the string into two parts at the dash
        std::string uuid1String = uuidString.substr(0, uuidString.find('-'));
        std::string uuid2String = uuidString.substr(uuidString.find('-') + 1);

        // Convert the two parts to unsigned 64-bit integers
        m_uuid1 = std::stoull(uuid1String, nullptr, 16);
        m_uuid2 = std::stoull(uuid2String, nullptr, 16);
    }

    UUID::UUID(const UUID& other)
        : m_uuid1(other.m_uuid1), m_uuid2(other.m_uuid2)
    {
    }

    UUID::~UUID()
    {
    }

    std::string UUID::ToString() const
    {
        std::stringstream ss;
        ss << std::hex << std::setfill('0') << std::setw(16) << m_uuid1 << "-" << std::setw(16) << m_uuid2;
        return ss.str();
    }
}