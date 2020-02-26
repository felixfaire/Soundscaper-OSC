/*
  ==============================================================================

    ConvexHullPath.h
    Created: 26 Feb 2020 10:00:50pm
    Author:  Felix Faire

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "vec2.hpp"

class ConvexHullPath
{
public:
    ConvexHullPath()
    {
    }
    
    void updatePoints(const std::vector<glm::vec2>& points, const glm::vec2& centerPos)
    {
        mRadiallySortedPoints = points;
        
        // sort positions cache clockwise for drawing hull
        std::sort(mRadiallySortedPoints.begin(),
                  mRadiallySortedPoints.end(), [centerPos](const glm::vec2& a, const glm::vec2& b) {
                    const float aa = std::atan2(a.x - centerPos.x, a.y - centerPos.y);
                    const float ab = std::atan2(b.x - centerPos.x, b.y - centerPos.y);
                    return aa < ab;
                  });
        
        mPath.clear();
        
        const int sz = (int)mRadiallySortedPoints.size();

        for (int i = 0; i < sz; ++i)
        {
            auto p = mRadiallySortedPoints[i];
            const auto& prevp = mRadiallySortedPoints[(i - 1 + sz) % sz];
            const auto& nextp = mRadiallySortedPoints[(i + 1) % sz];
            auto prevT = p - prevp;
            auto nextT = nextp - p;
            prevT = glm::normalize(glm::vec2(-prevT.y, prevT.x));
            nextT = glm::normalize(glm::vec2(-nextT.y, nextT.x));
            p += (prevT + nextT) * mOffset * (1.0f - 0.5f * glm::dot(prevT, nextT));
            
            if (i == 0)
                mPath.startNewSubPath(p.x, p.y);
            else
                mPath.lineTo(p.x, p.y);
        }
        
        mPath.closeSubPath();
        mPath = mPath.createPathWithRoundedCorners(mOffset);
    }
    
    float                   mOffset = 15.0f;
    std::vector<glm::vec2>  mRadiallySortedPoints;
    Path                    mPath;
};
