#pragma once

#include <openvr_driver.h>

class ControllerButton final
{
    vr::VRInputComponentHandle_t m_handle;
    unsigned char m_inputType;
    float m_value;
    bool m_state;
    bool m_updated;

    ControllerButton(const ControllerButton &that) = delete;
    ControllerButton& operator=(const ControllerButton &that) = delete;
public:
    enum InputType : unsigned char
    {
        IT_None = 0U,
        IT_Boolean,
        IT_Float
    };

    ControllerButton();
    ~ControllerButton();

    vr::VRInputComponentHandle_t GetHandle() const;
    vr::VRInputComponentHandle_t& GetHandleRef();

    void SetInputType(unsigned char p_type);
    unsigned char GetInputType() const;

    void SetState(bool p_state);
    bool GetState() const;

    void SetValue(float p_value);
    float GetValue() const;

    bool IsUpdated() const;
    void ResetUpdate();
};
