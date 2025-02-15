#pragma once

#include <TrackedController.hpp>
#include <TrackedObject.hpp>
#include <InterfaceHook.hpp>
#include <LeapConnection.hpp>

#include <openvr_driver.h>
#include <vector>

class TrackedDeviceProvider : public vr::IServerTrackedDeviceProvider 
{
public:
    ~TrackedDeviceProvider() { }

    vr::EVRInitError Init(vr::IVRDriverContext* pDriverContext);

    void Cleanup();

    const char* const* GetInterfaceVersions() { return vr::k_InterfaceVersions; }

    void RunFrame();

    bool ShouldBlockStandbyMode() { return false; }

    void EnterStandby() { }
    
    void LeaveStandby() { }
private:
    TrackedController m_Left { vr::ETrackedControllerRole::TrackedControllerRole_LeftHand };
    TrackedController m_Right { vr::ETrackedControllerRole::TrackedControllerRole_RightHand };
    InterfaceHook m_InterfaceHook { };
    LeapConnection m_LeapConnection { };
    std::vector<TrackedObject> m_trackers { };
};