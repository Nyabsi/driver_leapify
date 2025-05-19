#pragma once

#include <openvr_driver.h>

class HookManager {
private:
    bool m_IVRDriverInputHooked;
    bool m_IVRServerDriverHostHooked;

    void hookGenericInterface(void* pTablePtr);
    void hookIVRDriverInput(void* pTablePtr);
    void hookIVRServerDriverHost(void* pTablePtr);
public:
    explicit HookManager();
    ~HookManager();

    void Initialize(vr::IVRDriverContext* pContext);
    // Oh? Deinitializer? The hooks self-destruct on-close, good enough.
};