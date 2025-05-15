#include <driver/TrackedDeviceProvider.hpp>
#include <driver/TrackedController.hpp>
#include <hook/HookManager.hpp>
#include <core/LeapConnection.hpp>

#include <openvr_driver.h>

vr::EVRInitError TrackedDeviceProvider::Init(vr::IVRDriverContext* pDriverContext)
{
    VR_INIT_SERVER_DRIVER_CONTEXT(pDriverContext);
    m_HookManager.Initialize(pDriverContext);

    //m_trackers.push_back(TrackedObject("Wrist_Tracker_Left", true, 1));
    //m_trackers.push_back(TrackedObject("Wrist_Tracker_Right", false, 1));
    //m_trackers.push_back(TrackedObject("Elbow_Tracker_Left", true, 2));
    //m_trackers.push_back(TrackedObject("Elbow_Tracker_Right", false, 2));

    vr::VRServerDriverHost()->TrackedDeviceAdded("Leap_Left_Hand", vr::TrackedDeviceClass_Controller, &m_Left);
    vr::VRServerDriverHost()->TrackedDeviceAdded("Leap_Right_Hand", vr::TrackedDeviceClass_Controller, &m_Right);

    /*
    for (auto& tracker : m_trackers) {
        vr::VRServerDriverHost()->TrackedDeviceAdded(tracker.getSerial(), vr::TrackedDeviceClass_GenericTracker, &tracker);
    }
    */

    bool connected = m_LeapConnection.Init();

    if (connected)
        m_LeapConnection.Begin();

    return vr::VRInitError_None;
}

void TrackedDeviceProvider::Cleanup()
{
    m_LeapConnection.End();

    VR_CLEANUP_SERVER_DRIVER_CONTEXT();
}

void TrackedDeviceProvider::RunFrame()
{
    // TODO: only update when new frame is received, this is fucking stupid.
    LeapHand left { };
    LeapHand right { };

    if (m_LeapConnection.IsConnected())
    {
        left = m_LeapConnection.GetHand(vr::TrackedControllerRole_LeftHand);
        right = m_LeapConnection.GetHand(vr::TrackedControllerRole_RightHand);
    }

    m_Left.Update({left, right });
    m_Right.Update({ left, right });

    /*
    for (auto& tracker : m_trackers) {
        tracker.Update(tracker.isLeft() ? left : right);
    }
    */
}