#include <TrackedDeviceProvider.hpp>
#include <TrackedController.hpp>
#include <InterfaceHook.hpp>
#include <LeapConnection.hpp>

#include <openvr_driver.h>

vr::EVRInitError TrackedDeviceProvider::Init(vr::IVRDriverContext* pDriverContext)
{
    VR_INIT_SERVER_DRIVER_CONTEXT(pDriverContext);
    m_InterfaceHook.Init(pDriverContext);

    vr::VRServerDriverHost()->TrackedDeviceAdded("Leap_Left_Hand", vr::TrackedDeviceClass_Controller, &m_Left);
    vr::VRServerDriverHost()->TrackedDeviceAdded("Leap_Right_Hand", vr::TrackedDeviceClass_Controller, &m_Right);

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

    // HACK: this is used to show the menu
    auto calcLength = [](LEAP_VECTOR a, LEAP_VECTOR b) -> float
        {
            double dx = std::abs(a.x - b.x);
            double dy = std::abs(a.y - b.y);
            double dz = std::abs(a.z - b.z);

            double index_tip_distance_squared = dx * dx + dy * dy + dz * dz;
            return std::sqrt(index_tip_distance_squared);
        };

    left.menu = calcLength(left.index.distal.next_joint, right.index.distal.next_joint) < 20 && calcLength(left.thumb.distal.next_joint, right.thumb.distal.next_joint) < 20 && calcLength(left.middle.distal.next_joint, right.middle.distal.next_joint) > 40 && calcLength(left.ring.distal.next_joint, right.ring.distal.next_joint) > 40;

    m_Left.Update(left);
    m_Right.Update(right);
}