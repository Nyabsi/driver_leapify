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

    auto processInputForHands = [](LeapHand& l, LeapHand& r) {

        auto abD = [](LEAP_VECTOR a, LEAP_VECTOR b) -> float {
            
            double dx = std::abs(a.x - b.x);
            double dy = std::abs(a.y - b.y);
            double dz = std::abs(a.z - b.z);

            return std::sqrt(dx * dx + dy * dy + dz * dz);
        };

        // only update menu for left hand
        l.gestures.menu =
            abD(l.index.distal.next_joint, r.index.distal.next_joint) < 15 &&
            abD(l.thumb.distal.next_joint, r.thumb.distal.next_joint) < 15 &&
            abD(l.middle.distal.next_joint, r.middle.distal.next_joint) >= 45 &&
            abD(l.ring.distal.next_joint, r.ring.distal.next_joint) >= 45 && 
            abD(l.pinky.distal.next_joint, r.pinky.distal.next_joint) >= 45 &&
            l.index.is_extended && l.thumb.is_extended && r.index.is_extended && r.thumb.is_extended;

        l.gestures.index = abD(l.thumb.distal.next_joint, l.index.distal.next_joint) < 15 && abD(l.thumb.distal.next_joint, l.middle.distal.next_joint) >= 25;
        r.gestures.index = abD(r.thumb.distal.next_joint, r.index.distal.next_joint) < 15 && abD(r.thumb.distal.next_joint, r.middle.distal.next_joint) >= 25;
    };

    processInputForHands(left, right);

    m_Left.Update(left);
    m_Right.Update(right);
}