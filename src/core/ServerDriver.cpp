#include "ServerDriver.h"

#include <leap/LeapPoller.h>
#include <leap/LeapFrame.h>
#include <leap/LeapHand.h>

#include <controller/LeapController.h>
#include <utils/Utils.h>

#include <controller/ControllerInput.h>

extern char g_modulePath[];

const char* const ServerDriver::ms_interfaces[]
{
    vr::ITrackedDeviceServerDriver_Version,
    vr::IServerTrackedDeviceProvider_Version,
    nullptr
};

ServerDriver::ServerDriver()
{
    m_leapPoller = nullptr;
    m_leapFrame = nullptr;
    m_connectionState = false;
    m_leftController = nullptr;
    m_rightController = nullptr;
    m_controllerInput = nullptr;
}

ServerDriver::~ServerDriver()
{
}

vr::EVRInitError ServerDriver::Init(vr::IVRDriverContext *pDriverContext)
{
    VR_INIT_SERVER_DRIVER_CONTEXT(pDriverContext);

    m_leftController = new LeapController(true);
    m_rightController = new LeapController(false);

    m_controllerInput = new ControllerInput();

    vr::VRServerDriverHost()->TrackedDeviceAdded(m_leftController->GetSerialNumber().c_str(), vr::TrackedDeviceClass_Controller, m_leftController);
    vr::VRServerDriverHost()->TrackedDeviceAdded(m_rightController->GetSerialNumber().c_str(), vr::TrackedDeviceClass_Controller, m_rightController);

    m_leapFrame = new LeapFrame();
    m_leapPoller = new LeapPoller();
    m_leapPoller->Start();
    m_leapPoller->SetTrackingMode(_eLeapTrackingMode::eLeapTrackingMode_HMD);
    m_leapPoller->SetPolicy(eLeapPolicyFlag::eLeapPolicyFlag_OptimizeHMD, eLeapPolicyFlag::eLeapPolicyFlag_OptimizeScreenTop);

    return vr::VRInitError_None;
}

void ServerDriver::Cleanup()
{
    delete m_leftController;
    m_leftController = nullptr;

    delete m_rightController;
    m_rightController = nullptr;

    m_leapPoller->Stop();
    delete m_leapPoller;
    m_leapPoller = nullptr;

    delete m_leapFrame;
    m_leapFrame = nullptr;

    delete m_controllerInput;
    m_controllerInput = nullptr;

    m_connectionState = false;

    VR_CLEANUP_SERVER_DRIVER_CONTEXT();
}

const char* const* ServerDriver::GetInterfaceVersions()
{
    return ms_interfaces;
}

void ServerDriver::RunFrame()
{
    if (m_connectionState != m_leapPoller->IsConnected())
    {
        m_connectionState = m_leapPoller->IsConnected();
        m_leftController->SetEnabled(m_connectionState);
        m_rightController->SetEnabled(m_connectionState);

        m_leapPoller->SetTrackingMode(_eLeapTrackingMode::eLeapTrackingMode_HMD);
        m_leapPoller->SetPolicy(eLeapPolicyFlag::eLeapPolicyFlag_OptimizeHMD, eLeapPolicyFlag::eLeapPolicyFlag_OptimizeScreenTop);
    }

    if (m_connectionState && m_leapPoller->GetFrame(m_leapFrame->GetEvent()))
        m_leapFrame->Update();

    if (m_controllerInput->IsConnected())
        m_controllerInput->Update(m_leftController, m_rightController);

    // Update devices
    m_leftController->RunFrame(m_leapFrame->GetLeftHand());
    m_rightController->RunFrame(m_leapFrame->GetRightHand());
}

bool ServerDriver::ShouldBlockStandbyMode()
{
    return false;
}

void ServerDriver::EnterStandby()
{
}

void ServerDriver::LeaveStandby()
{
}
