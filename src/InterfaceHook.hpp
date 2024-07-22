#pragma once

#include <openvr_driver.h>
#include <vector>

class InterfaceHook
{
public:
    explicit InterfaceHook() { }
    ~InterfaceHook() { }
    void Init(vr::IVRDriverContext* pDriverContext);
private:
    void GetGenericInterface(void* interfacePtr, const char* pchInterfaceVersion);
    void CreateBooleanComponent(vr::PropertyContainerHandle_t ulContainer, const char* pchName, vr::VRInputComponentHandle_t* pHandle);
    void CreateScalarComponent(vr::PropertyContainerHandle_t ulContainer, const char* pchName, vr::VRInputComponentHandle_t* pHandle);

    bool m_IVRServerDriverHostHooked_006 { false };
    bool m_IVRDriverInputHooked_003 { false };
};