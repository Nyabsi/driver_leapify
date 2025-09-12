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
                    // Prevent Pimax Hand Tracking driver from loading forcefully
                    if (pchSerial != "LMHAND-0000" && pchSerial != "LMHAND-0001")
                        return orig(self, pchDeviceSerialNumber, eDeviceClass, pDriver);
                   return false;
           });

           rcmp::hook_indirect_function<void(*)(void* self, uint32_t unWhichDevice, const vr::DriverPose_t& newPose, uint32_t unPoseStructSize)>(vtable + 1 + vtable_offset, [this](auto orig, void* self, uint32_t unWhichDevice, const vr::DriverPose_t& newPose, uint32_t unPoseStructSize) -> void
               {
                   auto pose = newPose;

                   auto props = vr::VRProperties()->TrackedDeviceToPropertyContainer(unWhichDevice);
                   auto manufacturer = vr::VRProperties()->GetStringProperty(props, vr::Prop_ManufacturerName_String);
                   auto trackingSystem = vr::VRProperties()->GetStringProperty(props, vr::Prop_TrackingSystemName_String);
                   auto device_class = vr::VRProperties()->GetInt32Property(props, vr::Prop_DeviceClass_Int32);
                   auto role = vr::VRProperties()->GetInt32Property(props, vr::Prop_ControllerRoleHint_Int32);

                   if (device_class == vr::TrackedDeviceClass_Controller && manufacturer != "Ultraleap")
                   {
                       if (vr::VRSettings()->GetBool("driver_leapify", "automaticControllerSwitching"))
                       {
                           ControllerState& state = StateManager::Get().getState(unWhichDevice);

                           if (!state.isIdle)
                           {
                               float deltaVelocity[3] = {
                                   newPose.vecVelocity[0] - state.lastVelocity[0],
                                   newPose.vecVelocity[1] - state.lastVelocity[1],
                                   newPose.vecVelocity[2] - state.lastVelocity[2]
                               };

                               auto calculateSquaredMagnitude = [](const float v[3]) {
                                   return v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
                                   };

                               float accelerationMagnitude = calculateSquaredMagnitude(deltaVelocity);

                               const float accelerationThreshold = 0.0120f;
                               bool impactDetected = accelerationMagnitude > accelerationThreshold;

                               state.lastVelocity[0] = newPose.vecVelocity[0];
                               state.lastVelocity[1] = newPose.vecVelocity[1];
                               state.lastVelocity[2] = newPose.vecVelocity[2];

                               if (impactDetected && state.touch && !state.trigger && !state.grip && !state.pad)
                               {
                                   for (auto& dev : StateManager::Get().getControllerStates()) {
                                       dev.second.isIdle = true;
                                       StateManager::Get().updateControllerState(dev.first, dev.second);
                                   }
                               }
                           }
                           else
                           {
                               pose.deviceIsConnected = false;
                           }
                           state.role = role;
                           StateManager::Get().updateControllerState(unWhichDevice, state);

                       }

                       if (vr::VRSettings()->GetBool("driver_leapify", "handTrackingEnabled") &&
                           vr::VRSettings()->GetBool("driver_leapify", "positionalDataPassthrough"))
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

   if (interfaceName == "IVRDriverInput_003" || interfaceName == "IVRDriverInput_004") // SteamVR 2.13.x+ has bumped interface
   {
       void** vtable = *((void***)interfacePtr);

       if (!m_IVRDriverInputHooked_003)
       {
           rcmp::hook_indirect_function<vr::EVRInputError(void* self, vr::PropertyContainerHandle_t ulContainer, const char* pchName, vr::VRInputComponentHandle_t* pHandle)>(vtable + 0 + vtable_offset, [this](auto orig, void* self, vr::PropertyContainerHandle_t ulContainer, const char* pchName, vr::VRInputComponentHandle_t* pHandle) -> vr::EVRInputError
               {
                   vr::EVRInputError result;
                   result = orig(self, ulContainer, pchName, pHandle);
                   auto role = vr::VRProperties()->GetInt32Property(ulContainer, vr::ETrackedDeviceProperty::Prop_ControllerRoleHint_Int32);
                   auto manufacturer = vr::VRProperties()->GetStringProperty(ulContainer, vr::Prop_ManufacturerName_String);


                   if (role == vr::TrackedControllerRole_LeftHand && manufacturer != "Ultraleap")
                       StateManager::Get().addButtonHook(*pHandle, vr::TrackedControllerRole_LeftHand, std::string(pchName));
                   if (role == vr::TrackedControllerRole_RightHand && manufacturer != "Ultraleap")
                       StateManager::Get().addButtonHook(*pHandle, vr::TrackedControllerRole_RightHand, std::string(pchName));
                   return result;
               });

           rcmp::hook_indirect_function<vr::EVRInputError(void* self, vr::VRInputComponentHandle_t ulComponent, bool bNewValue, double fTimeOffset)>(vtable + 1 + vtable_offset, [](auto orig, void* self, vr::VRInputComponentHandle_t ulComponent, bool bNewValue, double fTimeOffset) -> vr::EVRInputError
               {
                   auto button = StateManager::Get().getButton(ulComponent);

                   for (auto& dev : StateManager::Get().getControllerStates()) {
                       if (dev.second.role == button.role) {
                           if (dev.second.isIdle && button.path.find("/click") != -1) {
                               for (auto& dev : StateManager::Get().getControllerStates()) {
                                   dev.second.isIdle = false;
                                   StateManager::Get().updateControllerState(dev.first, dev.second);
                               }
                           }

                           if (button.path == "/input/thumbstick/touch" || button.path == "/input/joystick/touch") {
                               ControllerState& state = StateManager::Get().getState(dev.first);
                               state.touch = bNewValue;
                               StateManager::Get().updateControllerState(dev.first, state);
                           }

                           if (button.path == "/input/trigger/touch") {
                               ControllerState& state = StateManager::Get().getState(dev.first);
                               state.trigger = bNewValue;
                               StateManager::Get().updateControllerState(dev.first, state);
                           }

                           if (button.path == "/input/grip/touch") {
                               ControllerState& state = StateManager::Get().getState(dev.first);
                               state.grip = bNewValue;
                               StateManager::Get().updateControllerState(dev.first, state);
                           }

                           if (button.path == "/input/trackpad/touch") {
                               ControllerState& state = StateManager::Get().getState(dev.first);
                               state.pad = bNewValue;
                               StateManager::Get().updateControllerState(dev.first, state);
                           }
                       }
                   }

                   return orig(self, ulComponent, bNewValue, fTimeOffset);
               });
           
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
           m_IVRDriverInputHooked_003 = true;
       }
   }
}
