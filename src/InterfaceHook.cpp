#include <InterfaceHook.hpp>
#include <StateManager.hpp>
#include <LeapC.h>

#include <stdint.h>
#include <string>

#include <openvr_driver.h>
#include <rcmp.hpp>

#include <glm/gtx/quaternion.hpp>

#ifdef _WIN32
    constexpr int vtable_offset = 0;
#else
    constexpr int vtable_offset = 1;
#endif

void InterfaceHook::Init(vr::IVRDriverContext* pDriverContext)
{
    void** vtable = *((void***)pDriverContext);

    rcmp::hook_indirect_function<void*(*)(void* self, const char* pchInterfaceVersion, void* peError)>(vtable + 0 + vtable_offset, [this](auto orig, void* self, const char* pchInterfaceVersion, void* peError) -> void* {
        void* interfacePtr = orig(self, pchInterfaceVersion, peError);
        GetGenericInterface(interfacePtr, pchInterfaceVersion);
        return interfacePtr;
    });
}

void InterfaceHook::GetGenericInterface(void* interfacePtr, const char* pchInterfaceVersion)
{
   std::string interfaceName(pchInterfaceVersion);

   if (interfaceName == "IVRServerDriverHost_006")
   {
       void** vtable = *((void***)interfacePtr);

       if (!m_IVRServerDriverHostHooked_006)
       {
        
           rcmp::hook_indirect_function<bool(void* self, const char *pchDeviceSerialNumber, vr::ETrackedDeviceClass eDeviceClass, vr::ITrackedDeviceServerDriver *pDriver)>(vtable + 0 + vtable_offset, [this](auto orig, void* self, const char *pchDeviceSerialNumber, vr::ETrackedDeviceClass eDeviceClass, vr::ITrackedDeviceServerDriver *pDriver) -> bool {
                    std::string pchSerial(pchDeviceSerialNumber);
                    if (pchSerial != "LMHAND-0000" && pchSerial != "LMHAND-0001")
                        return orig(self, pchDeviceSerialNumber, eDeviceClass, pDriver);
                   return false;
           });

           rcmp::hook_indirect_function<void(*)(void* self, uint32_t unWhichDevice, const vr::DriverPose_t& newPose, uint32_t unPoseStructSize)>(vtable + 1 + vtable_offset, [this](auto orig, void* self, uint32_t unWhichDevice, const vr::DriverPose_t& newPose, uint32_t unPoseStructSize) -> void
           {
                    auto pose = newPose;
                    auto props = vr::VRProperties()->TrackedDeviceToPropertyContainer(unWhichDevice);
                    auto manufacturer = vr::VRProperties()->GetStringProperty(props, vr::ETrackedDeviceProperty::Prop_ManufacturerName_String);
                    auto trackingSystem = vr::VRProperties()->GetStringProperty(props, vr::ETrackedDeviceProperty::Prop_TrackingSystemName_String);
                    auto device_class = vr::VRProperties()->GetInt32Property(props, vr::ETrackedDeviceProperty::Prop_DeviceClass_Int32);
                    auto role = vr::VRProperties()->GetInt32Property(props, vr::ETrackedDeviceProperty::Prop_ControllerRoleHint_Int32);

                    if (device_class == vr::TrackedDeviceClass_Controller && manufacturer != "Ultraleap")
                    {
                        if (vr::VRSettings()->GetBool("driver_leapify", "automaticControllerSwitching"))
                        {
                            ControllerState& state = StateManager::Get().getState(unWhichDevice);

                            auto calculateVelocityMagnitude = [](const vr::DriverPose_t& pose)
                                {
                                    return pose.vecVelocity[0] * pose.vecVelocity[0] +
                                        pose.vecVelocity[1] * pose.vecVelocity[1] +
                                        pose.vecVelocity[2] * pose.vecVelocity[2];
                                };

                            if (state.m_velocity.update(calculateVelocityMagnitude(pose)) < 0.000030 && !StateManager::Get().getShouldWeHiJackTheController())
                            {
                                if (state.timestamp == -1)
                                    state.timestamp = LeapGetNow();

                                if ((LeapGetNow() - state.timestamp) >= 3000000) // LeapGetNow is in micro seconds
                                {
                                    state.timestamp = -1;
                                    StateManager::Get().setShouldWeHiJackTheController(true);
                                }
                            }

                            if (StateManager::Get().getShouldWeHiJackTheController())
                            {
                                state.isIdle = true;
                                pose.deviceIsConnected = false;
                            } else {
                                state.isIdle = false;
                            }

                            StateManager::Get().updateControllerState(unWhichDevice, state);
                        }

                        if (vr::VRSettings()->GetBool("driver_leapify", "handTrackingEnabled") && vr::VRSettings()->GetBool("driver_leapify", "positionalDataPassthrough"))
                        {
                            pose = StateManager::Get().getLeapPose(static_cast<vr::ETrackedControllerRole>(role));
                            pose.deviceIsConnected = newPose.deviceIsConnected;
                        }
                    }

                    orig(self, unWhichDevice, pose, unPoseStructSize);
           });

           m_IVRServerDriverHostHooked_006 = true;
       }
   }

   if (interfaceName == "IVRDriverInput_003")
   {
       void** vtable = *((void***)interfacePtr);

       if (!m_IVRDriverInputHooked_003)
       {
           rcmp::hook_indirect_function<vr::EVRInputError(void* self, vr::PropertyContainerHandle_t ulContainer, const char* pchName, const char* pchSkeletonPath, const char* pchBasePosePath, vr::EVRSkeletalTrackingLevel eSkeletalTrackingLevel, const vr::VRBoneTransform_t* pGripLimitTransforms, uint32_t unGripLimitTransformCount, vr::VRInputComponentHandle_t* pHandle)>(vtable + 5 + vtable_offset, [](auto orig, void* self, vr::PropertyContainerHandle_t ulContainer, const char* pchName, const char* pchSkeletonPath, const char* pchBasePosePath, vr::EVRSkeletalTrackingLevel eSkeletalTrackingLevel, const vr::VRBoneTransform_t* pGripLimitTransforms, uint32_t unGripLimitTransformCount, vr::VRInputComponentHandle_t* pHandle) -> vr::EVRInputError
           {
                   auto result = orig(self, ulContainer, pchName, pchSkeletonPath, pchBasePosePath, eSkeletalTrackingLevel, pGripLimitTransforms, unGripLimitTransformCount, pHandle);

                   auto role = vr::VRProperties()->GetInt32Property(ulContainer, vr::ETrackedDeviceProperty::Prop_ControllerRoleHint_Int32);

                   if (role == vr::TrackedControllerRole_LeftHand)
                       StateManager::Get().addTransformHook(*pHandle, vr::TrackedControllerRole_LeftHand);
                   if (role == vr::TrackedControllerRole_RightHand)
                       StateManager::Get().addTransformHook(*pHandle, vr::TrackedControllerRole_RightHand);

                   return result;
           });
           
           rcmp::hook_indirect_function<vr::EVRInputError(void* self, vr::VRInputComponentHandle_t ulComponent, vr::EVRSkeletalMotionRange eMotionRange, const vr::VRBoneTransform_t* pTransforms, uint32_t unTransformCount)>(vtable + 6 + vtable_offset, [](auto orig, void* self, vr::VRInputComponentHandle_t ulComponent, vr::EVRSkeletalMotionRange eMotionRange, const vr::VRBoneTransform_t* pTransforms, uint32_t unTransformCount) -> vr::EVRInputError
           {
                   auto role = StateManager::Get().getTransformHookRole(ulComponent);

                   if (vr::VRSettings()->GetBool("driver_leapify", "skeletalDataPassthrough") && StateManager::Get().getLeapTransform(role) != nullptr)
                        return orig(self, ulComponent, vr::VRSkeletalMotionRange_WithoutController, StateManager::Get().getLeapTransform(role), 31);
                   else
                        return orig(self, ulComponent, eMotionRange, pTransforms, unTransformCount);
           });

           rcmp::hook_indirect_function<vr::EVRInputError(void* self, vr::VRInputComponentHandle_t ulComponent, bool bNewValue, double fTimeOffset)>(vtable + 1 + vtable_offset, [](auto orig, void* self, vr::VRInputComponentHandle_t ulComponent, bool bNewValue, double fTimeOffset) -> vr::EVRInputError
           {
                   auto yes = StateManager::Get().getShouldWeHiJackTheController();
                   if (yes) {
                    StateManager::Get().setShouldWeHiJackTheController(false);
                    return orig(self, ulComponent, false, 0);
                   } else {
                    return orig(self, ulComponent, bNewValue, fTimeOffset);
                   }
           });

           m_IVRDriverInputHooked_003 = true;
       }
   }
}
