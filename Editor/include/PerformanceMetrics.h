#pragma once

#include <vector>


struct PerformanceMetricsContext
{
    std::vector<float> frameTimes;
    float frameTimeAverage = 0.0f;
    float frameTimeMax = -1.0f;
    float frameTimeMin = FLT_MAX;

    float maxFrameTime = 10.0f;
    float minFrameTime = 0.0f;

    void Update(float deltaTime)
    {
        if (deltaTime > maxFrameTime || deltaTime < minFrameTime)
        {
            return;
        }

        frameTimes.push_back(deltaTime);
        if (deltaTime > frameTimeMax)
            frameTimeMax = deltaTime;
        if (deltaTime < frameTimeMin)
            frameTimeMin = deltaTime;
    }

    void Reset()
    {
        frameTimes.clear();
        frameTimeAverage = 0.0f;
        frameTimeMax = -1.0f;
        frameTimeMin = FLT_MAX;
    }

    void CalculateAverageFrameTime()
    {
        frameTimeAverage = 0;
        for (float frameTime : frameTimes)
            frameTimeAverage += frameTime;
        frameTimeAverage /= frameTimes.size();
    }
};