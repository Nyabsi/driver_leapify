#pragma once

#include <openvr_driver.h>
#include <LeapC.h>
#include <thread>

class LeapPoller;
class LeapFrame;
class LeapController;
class ControllerInput;

class ServerDriver final : public vr::IServerTrackedDeviceProvider
{
    static const char* const ms_interfaces[];

    bool m_connectionState;
    LeapPoller *m_leapPoller;
    LeapFrame *m_leapFrame;
    LeapController *m_leftController;
    LeapController *m_rightController;
    ControllerInput *m_controllerInput;

    ServerDriver(const ServerDriver &that) = delete;
    ServerDriver& operator=(const ServerDriver &that) = delete;

    vr::EVRInitError Init(vr::IVRDriverContext *pDriverContext);
    void Cleanup();
    const char* const* GetInterfaceVersions();
    void RunFrame();
    bool ShouldBlockStandbyMode();
    void EnterStandby();
    void LeaveStandby();
public:
    ServerDriver();
    ~ServerDriver();
};
