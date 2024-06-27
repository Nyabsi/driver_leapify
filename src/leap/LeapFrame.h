#pragma once

#include <LeapC.h>

class LeapHand;
class LeapFrame
{
    LEAP_TRACKING_EVENT m_event;
    LeapHand *m_leftHand;
    LeapHand *m_rightHand;
    int64_t m_lastFrameId;
public:
    LeapFrame();
    ~LeapFrame();

    LEAP_TRACKING_EVENT* GetEvent();
    const LeapHand* GetLeftHand() const;
    const LeapHand* GetRightHand() const;

    void Update();
};

