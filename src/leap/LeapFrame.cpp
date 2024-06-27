#include "LeapFrame.h"
#include "LeapHand.h"

LeapFrame::LeapFrame()
{
    m_leftHand = new LeapHand(true);
    m_rightHand = new LeapHand(false);
    m_lastFrameId = 0U;
    m_event = {};
}

LeapFrame::~LeapFrame()
{
    delete m_leftHand;
    delete m_rightHand;
}

LEAP_TRACKING_EVENT* LeapFrame::GetEvent()
{
    return &m_event;
}

const LeapHand* LeapFrame::GetLeftHand() const
{
    return m_leftHand;
}

const LeapHand* LeapFrame::GetRightHand() const
{
    return m_rightHand;
}

void LeapFrame::Update()
{
    if(m_lastFrameId != m_event.tracking_frame_id)
    {
        bool l_skipLeft = false;
        bool l_skipRight = false;

        for(uint32_t i = 0U; i < m_event.nHands; i++)
        {
            switch(m_event.pHands[i].type)
            {
                case eLeapHandType_Left:
                {
                    if(!l_skipLeft)
                    {
                        m_leftHand->Update(m_event.pHands[i]);
                        l_skipLeft = true;
                    }
                } break;
                case eLeapHandType_Right:
                {
                    if(!l_skipRight)
                    {
                        m_rightHand->Update(m_event.pHands[i]);
                        l_skipRight = true;
                    }
                } break;
            }
        }

        if(!l_skipLeft)
            m_leftHand->Update();
        if(!l_skipRight)
            m_rightHand->Update();

        m_lastFrameId = m_event.tracking_frame_id;
    }
}
