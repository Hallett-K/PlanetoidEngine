#pragma once

#include <Planetoid/PlanetoidCore.h>

#include <Planetoid/Scenes/Entity.h>

namespace PlanetoidEngine
{
    class Timeline;
    struct PE_API TimelineEventContext 
    {
        Timeline* timeline;
        Entity owner;
    };

    class PE_API TimelineEvent
    {
    protected:
        enum class EventType
        {
            UNDEFINED = 0,
            SingleFire,
            Duration
        };

        EventType m_type;

        TimelineEventContext m_context;

    public:
        TimelineEvent(Timeline* timeline, Entity owner)
            : m_context({ timeline, owner }), m_type(EventType::UNDEFINED) {}
        TimelineEvent(Timeline* timeline, Entity owner, EventType type)
            : m_context({ timeline, owner }), m_type(type) {}
    };

    class PE_API SingleFireEvent : public TimelineEvent
    {
    public:
        SingleFireEvent(Timeline* timeline, Entity owner)
            : TimelineEvent(timeline, owner, EventType::SingleFire) {}
        
        virtual void OnFire() = 0;
    };

    class PE_API DurationEvent : public TimelineEvent
    {
    public:
        DurationEvent(Timeline* timeline, Entity owner)
            : TimelineEvent(timeline, owner, EventType::Duration) {}

        virtual void OnBeginFire() = 0;
        virtual void OnUpdate(float deltaTime) = 0;
        virtual void OnEndFire() = 0;
    };
    
    class PE_API TimelineTrack
    {
    public:
        TimelineTrack() = default;
        TimelineTrack(const TimelineTrack&) = default;
        TimelineTrack& operator=(const TimelineTrack&) = default;

        void AddEvent(TimelineEvent* event, float startTime, float endTime);

        void Update(float deltaTime);
    private:
        struct EventInfo
        {
            TimelineEvent* event;
            float startTime;
            float endTime;
        };

        bool CanAddEvent(float startTime, float endTime);
        std::vector<EventInfo> m_events;
        std::unordered_map<const SingleFireEvent*, bool> m_singleFireEventFired;
        std::unordered_map<const DurationEvent*, bool> m_durationEventFired;
    };

    class PE_API Timeline 
    {
    public:
        Timeline() = default;
        Timeline(const Timeline&) = default;
        Timeline& operator=(const Timeline&) = default;

        void AddTrack(const TimelineTrack& track);
        void RemoveTrack(const TimelineTrack& track);

        void Update(float deltaTime);
    private:
        friend class TimelineManager;
        std::vector<TimelineTrack> m_tracks;
    };
}