#pragma once

#include <openvr_driver.h>
#include <vector>

#include <SimpleKalmanFilter.hpp>

class InterfaceHook
{
public:
    explicit InterfaceHook() { }
    ~InterfaceHook() { }
    void Init(vr::IVRDriverContext* pDriverContext);
private:
    void GetGenericInterface(void* interfacePtr, const char* pchInterfaceVersion);

    bool m_IVRServerDriverHostHooked_006 { false };
    bool m_IVRDriverInputHooked_003 { false };

    SimpleKalmanFilter m_KalmanFilter;
};