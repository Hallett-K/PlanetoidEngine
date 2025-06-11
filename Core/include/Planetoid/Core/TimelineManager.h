#pragma once

#include <Planetoid/PlanetoidCore.h>

#include <vector>

namespace PlanetoidEngine
{
    class Timeline;
    class TimelineManager
    {
    public:
        TimelineManager() = default;
        TimelineManager(const TimelineManager&) = delete;
        TimelineManager& operator=(const TimelineManager&) = delete;

        void Update(float deltaTime);
        void AddTimeline(const Timeline& timeline);
        void RemoveTimeline(const Timeline& timeline);

    private:
        std::vector<Timeline> m_activeTimelines;
    };
}