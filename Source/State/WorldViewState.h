/*
  ==============================================================================

    WorldViewState.h
    Created: 30 Mar 2020 7:01:20pm
    Author:  Administrator

  ==============================================================================
*/

#pragma once

#include "gtx/matrix_transform_2d.hpp"
#include "../Utils/ViewAxes.h"

class WorldViewState
{
public:
    WorldViewState()
    {
    }

    float getHeightBaselineProportion()
    {
        if (mCurrentAxes == ViewAxes::XZ)
            return 0.5f;
        else
            return 0.75f;
    }

    glm::vec3 getRectToWorld(const glm::vec2& p, bool translate = true)
    {
        const auto pt = mRectToWorld * glm::vec3(p.x, p.y, translate ? 1.0f : 0.0f);
        return Axes::getUnflattenedPoint(mCurrentAxes, glm::vec2(pt.x, pt.y));
    }

    glm::vec2 getWorldToRect(const glm::vec3& p)
    {
        const auto p2D = Axes::getFlattenedPoint(mCurrentAxes, p);
        const auto pt = mWorldToRect * glm::vec3(p2D.x, p2D.y, 1.0f);
        return glm::vec2(pt.x, pt.y);
    }

    void updateZoomExtents(const Rectangle<float>& bounds, const std::vector<glm::vec3>& positions)
    {
        mWindowDiameter = 1.0f;
        const auto worldWindowCenter = glm::vec3(0.0f);//getRectToWorld(glm::vec2(bounds.getCentreX(), bounds.getCentreY()));
        const glm::vec2 wc = Axes::getFlattenedPoint(mCurrentAxes, worldWindowCenter);

        for (const auto& p : positions)
        {
            const float l = glm::distance(Axes::getFlattenedPoint(mCurrentAxes, p), wc);

            if (l > mWindowDiameter)
                mWindowDiameter = l;
        }

        mWindowDiameter *= 2.2f;

        // Update new scale window diameter
        updateMatrices(bounds);
    }

    void updateMatrices(const Rectangle<float> b)
    {
        const float minDim = jmin(b.getWidth(), b.getHeight());

        glm::mat3 t = glm::mat3(1.0f);
        t = glm::scale(t, glm::vec2(1.0f) * mWindowDiameter);
        t = glm::scale(t, glm::vec2(1.0f / minDim));
        t = glm::scale(t, glm::vec2(1.0f, -1.0f)); // flip y up
        t = glm::translate(t, -getRectCenter(b));

        mRectToWorld = t;
        mWorldToRect = glm::inverse(t);
    }

    glm::vec2 getRectCenter(const Rectangle<float>& b) { return glm::vec2(b.getCentreX(), b.getHeight() * getHeightBaselineProportion()); }



    ViewAxes   mCurrentAxes = ViewAxes::XZ;
    glm::mat3  mRectToWorld;
    glm::mat3  mWorldToRect;
    float      mWindowDiameter = 4.0f;
};
