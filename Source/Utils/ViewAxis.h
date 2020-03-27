/*
  ==============================================================================

    ViewAxis.h
    Created: 27 Mar 2020 2:04:49pm
    Author:  Felix Faire

  ==============================================================================
*/

#pragma once

#include "vec2.hpp"
#include "vec3.hpp"

enum ViewAxes
{
    XZ,
    XY,
    ZY
};

class Axes
{
public:
    static glm::vec2 getFlattenedPoint(const ViewAxes& axes, const glm::vec3& p)
    {
        switch (axes)
        {
            case ViewAxes::XZ:
                return glm::vec2(p.x, p.z);
                
            case ViewAxes::XY:
                return glm::vec2(p.x, p.y);
            
            case ViewAxes::ZY:
                return glm::vec2(p.z, p.y);
        }
    }
    
    static glm::vec3 getUnflattenedPoint(const ViewAxes& axes, const glm::vec2& p)
    {
        switch (axes)
        {
            case ViewAxes::XZ:
                return glm::vec3(p.x, 0.0f, p.y);
                
            case ViewAxes::XY:
                return glm::vec3(p.x, p.y, 0.0f);
            
            case ViewAxes::ZY:
                return glm::vec3(0.0f, p.y, p.x);
        }
    }
    
    static float getDepthValue(const ViewAxes& axes, const glm::vec3& p)
    {
        float v = 0.0f;
        
        if (axes == ViewAxes::XZ)
            v = -p.y;
        if (axes == ViewAxes::XY)
            v = p.z;
        if (axes == ViewAxes::ZY)
            v = -p.x;
            
        return v;
    }
    
    static std::pair<std::string, std::string> getAxesNames(const ViewAxes& axes)
    {
        switch (axes)
        {
            case ViewAxes::XZ:
                return {"X", "Z"};
                
            case ViewAxes::XY:
                return {"X", "Y"};
            
            case ViewAxes::ZY:
                return {"Z", "Y"};
        }
    }
};
