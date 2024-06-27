#include "ControllerButton.h"

ControllerButton::ControllerButton()
{
    m_handle = vr::k_ulInvalidInputComponentHandle;
    m_inputType = IT_Boolean;
    m_state = false;
    m_value = 0.f;
    m_updated = false;
}

ControllerButton::~ControllerButton()
{
}

vr::VRInputComponentHandle_t ControllerButton::GetHandle() const
{
    return m_handle;
}

vr::VRInputComponentHandle_t& ControllerButton::GetHandleRef()
{
    return m_handle;
}

void ControllerButton::SetInputType(unsigned char p_type)
{
    m_inputType = p_type;
}

unsigned char ControllerButton::GetInputType() const
{
    return m_inputType;
}

void ControllerButton::SetState(bool p_state)
{
    if(m_inputType == IT_Boolean)
    {
        if(m_state != p_state)
        {
            m_state = p_state;
            m_updated = true;
        }
    }
}

bool ControllerButton::GetState() const
{
    return m_state;
}

void ControllerButton::SetValue(float p_value)
{
    if(m_inputType == IT_Float)
    {
        if(m_value != p_value)
        {
            m_value = p_value;
            m_updated = true;
        }
    }
}

float ControllerButton::GetValue() const
{
    return m_value;
}

bool ControllerButton::IsUpdated() const
{
    return m_updated;
}

void ControllerButton::ResetUpdate()
{
    m_updated = false;
}
