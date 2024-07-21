#include <InterfaceHook.hpp>
#include <DeviceController.hpp>

#include <stdint.h>
#include <string>

#include <openvr_driver.h>
#include <rcmp.hpp>

void InterfaceHook::Init(vr::IVRDriverContext* pDriverContext)
{
    void** vtable = *((void***)pDriverContext);
#ifdef _WIN32
    int vtable_offset = 0; // on MSVC vtable starts at 0
#else
    int vtable_offset = 1; // on gcc/Clang vtable starts at 1
#endif
    rcmp::hook_indirect_function<void*(*)(void* self, const char* pchInterfaceVersion, void* peError)>(vtable + 0 + vtable_offset, [this](auto orig, void* self, const char* pchInterfaceVersion, void* peError) -> void* {
        void* interfacePtr = orig(self, pchInterfaceVersion, peError);
        GetGenericInterface(interfacePtr, pchInterfaceVersion);
        return interfacePtr;
    });
}

void InterfaceHook::GetGenericInterface(void* interfacePtr, const char* pchInterfaceVersion)
{
   std::string interfaceName(pchInterfaceVersion);

#ifdef _WIN32
   int vtable_offset = 0; // on MSVC vtable starts at 0
#else
   int vtable_offset = 1; // on gcc/Clang vtable starts at 1
#endif

   if (interfaceName == "IVRServerDriverHost_006")
   {
       void** vtable = *((void***)interfacePtr);

       if (!m_IVRServerDriverHostHooked_006)
       {
           rcmp::hook_indirect_function<void(*)(void* self, uint32_t unWhichDevice, const vr::DriverPose_t& newPose, uint32_t unPoseStructSize)>(vtable + 1 + vtable_offset, [](auto orig, void* self, uint32_t unWhichDevice, const vr::DriverPose_t& newPose, uint32_t unPoseStructSize) -> void
           {
               auto pose = newPose;

               auto props = vr::VRProperties()->TrackedDeviceToPropertyContainer(unWhichDevice);

               auto deviceClass = vr::VRProperties()->GetInt32Property(props, vr::ETrackedDeviceProperty::Prop_DeviceClass_Int32);
               auto deviceRole = static_cast<vr::ETrackedControllerRole>(vr::VRProperties()->GetInt32Property(props, vr::ETrackedDeviceProperty::Prop_ControllerRoleHint_Int32));
               auto manufacturerName = vr::VRProperties()->GetStringProperty(props, vr::ETrackedDeviceProperty::Prop_ManufacturerName_String);
               auto deviceSerial = vr::VRProperties()->GetStringProperty(props, vr::ETrackedDeviceProperty::Prop_SerialNumber_String);

               if (deviceClass == vr::TrackedDeviceClass_Controller && deviceSerial.find("Leap_Hand") == -1)
               {
                   if (pose.deviceIsConnected && pose.poseIsValid && pose.vecPosition[0] != 0 && pose.vecPosition[1] != 0 && pose.vecPosition[2] != 0)
                   {
                       if (DeviceController::get().GetController(deviceRole).m_objectId == 999)
                       {
                           DeviceController::get().UpdateController(deviceRole, unWhichDevice, deviceSerial, manufacturerName);
                       }
                       else 
                       {
                           if (DeviceController::get().GetController(deviceRole).m_objectId == unWhichDevice)
                           {
                               DeviceController::get().UpdateControllerPose(deviceRole, pose);
                               pose.deviceIsConnected = false;
                           }
                       }
                   }
                   else 
                   {
                       if (DeviceController::get().GetController(deviceRole).m_objectId == unWhichDevice) 
                       {
                           DeviceController::get().UpdateController(deviceRole, 999, "", "");
                       }
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
           rcmp::hook_indirect_function<vr::EVRInputError(void* self, vr::PropertyContainerHandle_t ulContainer, const char* pchName, vr::VRInputComponentHandle_t* pHandle)>(vtable + 0 + vtable_offset, [](auto orig, void* self,vr::PropertyContainerHandle_t ulContainer, const char* pchName, vr::VRInputComponentHandle_t* pHandle) -> vr::EVRInputError
           {
               vr::EVRInputError result;
               result = orig(self, ulContainer, pchName, pHandle);

               auto deviceSerial = vr::VRProperties()->GetStringProperty(ulContainer, vr::ETrackedDeviceProperty::Prop_SerialNumber_String);
               auto manufacturerName = vr::VRProperties()->GetStringProperty(ulContainer, vr::ETrackedDeviceProperty::Prop_ManufacturerName_String);
               auto deviceRole = vr::VRProperties()->GetInt32Property(ulContainer, vr::ETrackedDeviceProperty::Prop_ControllerRoleHint_Int32);
               std::string inputName(pchName);

               if (inputName == "/input/system/click")
               {
                   if (deviceRole == vr::TrackedControllerRole_LeftHand)
                       DeviceController::get().getComponent(deviceSerial, 0).m_orig = *pHandle;
                   if (deviceRole == vr::TrackedControllerRole_RightHand)
                       DeviceController::get().getComponent(deviceSerial, 1).m_orig = *pHandle;
               }

               if (inputName == "/input/system/touch")
               {
                   if (deviceRole == vr::TrackedControllerRole_LeftHand)
                       DeviceController::get().getComponent(deviceSerial, 2).m_orig = *pHandle;
                   if (deviceRole == vr::TrackedControllerRole_RightHand)
                       DeviceController::get().getComponent(deviceSerial, 3).m_orig = *pHandle;
               }

               if (inputName == "/input/trigger/click")
               {
                   if (deviceRole == vr::TrackedControllerRole_LeftHand)
                       DeviceController::get().getComponent(deviceSerial, 4).m_orig = *pHandle;
                   if (deviceRole == vr::TrackedControllerRole_RightHand)
                       DeviceController::get().getComponent(deviceSerial, 5).m_orig = *pHandle;
               }

               if (inputName == "/input/trigger/touch")
               {
                   if (deviceRole == vr::TrackedControllerRole_LeftHand)
                       DeviceController::get().getComponent(deviceSerial, 6).m_orig = *pHandle;
                   if (deviceRole == vr::TrackedControllerRole_RightHand)
                       DeviceController::get().getComponent(deviceSerial, 7).m_orig = *pHandle;
               }

               if (manufacturerName == "Oculus")
               {
                   if (inputName == "/input/x/click")
                   {
                       DeviceController::get().getComponent(deviceSerial, 32).m_orig = *pHandle;
                   }

                   if (inputName == "/input/a/click")
                   {
                       DeviceController::get().getComponent(deviceSerial, 33).m_orig = *pHandle;
                   }

                   if (inputName == "/input/y/click")
                   {
                       DeviceController::get().getComponent(deviceSerial, 36).m_orig = *pHandle;
                   }

                   if (inputName == "/input/b/click")
                   {
                       DeviceController::get().getComponent(deviceSerial, 37).m_orig = *pHandle;
                   }

                   if (inputName == "/input/x/touch")
                   {
                       DeviceController::get().getComponent(deviceSerial, 34).m_orig = *pHandle;
                   }

                   if (inputName == "/input/a/touch")
                   {
                       DeviceController::get().getComponent(deviceSerial, 35).m_orig = *pHandle;
                   }

                   if (inputName == "/input/y/touch")
                   {
                       DeviceController::get().getComponent(deviceSerial, 38).m_orig = *pHandle;
                   }

                   if (inputName == "/input/b/touch")
                   {
                       DeviceController::get().getComponent(deviceSerial, 39).m_orig = *pHandle;
                   }

                   if (inputName == "/input/joystick/click")
                   {
                       if (deviceRole == vr::TrackedControllerRole_LeftHand)
                           DeviceController::get().getComponent(deviceSerial, 24).m_orig = *pHandle;
                       if (deviceRole == vr::TrackedControllerRole_RightHand)
                           DeviceController::get().getComponent(deviceSerial, 25).m_orig = *pHandle;
                   }

                   if (inputName == "/input/joystick/touch")
                   {
                       if (deviceRole == vr::TrackedControllerRole_LeftHand)
                           DeviceController::get().getComponent(deviceSerial, 26).m_orig = *pHandle;
                       if (deviceRole == vr::TrackedControllerRole_RightHand)
                           DeviceController::get().getComponent(deviceSerial, 27).m_orig = *pHandle;
                   }
               }
               else
               {
                   if (inputName == "/input/a/click")
                   {
                       if (deviceRole == vr::TrackedControllerRole_LeftHand)
                           DeviceController::get().getComponent(deviceSerial, 32).m_orig = *pHandle;
                       if (deviceRole == vr::TrackedControllerRole_RightHand)
                           DeviceController::get().getComponent(deviceSerial, 33).m_orig = *pHandle;
                   }

                   if (inputName == "/input/b/click")
                   {
                       if (deviceRole == vr::TrackedControllerRole_LeftHand)
                           DeviceController::get().getComponent(deviceSerial, 34).m_orig = *pHandle;
                       if (deviceRole == vr::TrackedControllerRole_RightHand)
                           DeviceController::get().getComponent(deviceSerial, 35).m_orig = *pHandle;
                   }

                   if (inputName == "/input/a/touch")
                   {
                       if (deviceRole == vr::TrackedControllerRole_LeftHand)
                           DeviceController::get().getComponent(deviceSerial, 34).m_orig = *pHandle;
                       if (deviceRole == vr::TrackedControllerRole_RightHand)
                           DeviceController::get().getComponent(deviceSerial, 35).m_orig = *pHandle;
                   }

                   if (inputName == "/input/b/touch")
                   {
                       if (deviceRole == vr::TrackedControllerRole_LeftHand)
                           DeviceController::get().getComponent(deviceSerial, 38).m_orig = *pHandle;
                       if (deviceRole == vr::TrackedControllerRole_RightHand)
                           DeviceController::get().getComponent(deviceSerial, 39).m_orig = *pHandle;
                   }

                   if (inputName == "/input/thumbstick/click")
                   {
                       if (deviceRole == vr::TrackedControllerRole_LeftHand)
                           DeviceController::get().getComponent(deviceSerial, 24).m_orig = *pHandle;
                       if (deviceRole == vr::TrackedControllerRole_RightHand)
                           DeviceController::get().getComponent(deviceSerial, 25).m_orig = *pHandle;
                   }

                   if (inputName == "/input/thumbstick/touch")
                   {
                       if (deviceRole == vr::TrackedControllerRole_LeftHand)
                           DeviceController::get().getComponent(deviceSerial, 26).m_orig = *pHandle;
                       if (deviceRole == vr::TrackedControllerRole_RightHand)
                           DeviceController::get().getComponent(deviceSerial, 27).m_orig = *pHandle;
                   }
               }

               if (inputName == "/input/grip/touch")
               {
                   if (deviceRole == vr::TrackedControllerRole_LeftHand)
                       DeviceController::get().getComponent(deviceSerial, 18).m_orig = *pHandle;
                   if (deviceRole == vr::TrackedControllerRole_RightHand)
                       DeviceController::get().getComponent(deviceSerial, 19).m_orig = *pHandle;
               }

               return result;
           });

           rcmp::hook_indirect_function<vr::EVRInputError(void* self, vr::VRInputComponentHandle_t ulComponent, bool bNewValue, double fTimeOffset)>(vtable + 1 + vtable_offset, [](auto orig, void* self, vr::VRInputComponentHandle_t ulComponent, bool bNewValue, double fTimeOffset) -> vr::EVRInputError
           {
               for (auto& component : DeviceController::get().GetComponents())
               {
                   if (DeviceController::get().GetController(vr::TrackedControllerRole_LeftHand).m_serial == component.first || DeviceController::get().GetController(vr::TrackedControllerRole_RightHand).m_serial == component.first)
                   {
                       for (auto& inputs : component.second)
                       {
                           if (inputs.second.m_orig == ulComponent)
                           {
                               return orig(self, DeviceController::get().getComponentOverride(inputs.first).m_override, bNewValue, 0);
                           }
                       }
                   }
               }

               return orig(self, ulComponent, bNewValue, fTimeOffset);
           });

           rcmp::hook_indirect_function<vr::EVRInputError(void* self, vr::PropertyContainerHandle_t ulContainer, const char* pchName, vr::VRInputComponentHandle_t* pHandle, vr::EVRScalarType eType, vr::EVRScalarUnits eUnits)>(vtable + 2 + vtable_offset, [](auto orig, void* self, vr::PropertyContainerHandle_t ulContainer, const char* pchName, vr::VRInputComponentHandle_t* pHandle, vr::EVRScalarType eType, vr::EVRScalarUnits eUnits) -> vr::EVRInputError
           {
                   vr::EVRInputError result;
                   result = orig(self, ulContainer, pchName, pHandle, eType, eUnits);

                   auto deviceSerial = vr::VRProperties()->GetStringProperty(ulContainer, vr::ETrackedDeviceProperty::Prop_SerialNumber_String);
                   auto manufacturerName = vr::VRProperties()->GetStringProperty(ulContainer, vr::ETrackedDeviceProperty::Prop_ManufacturerName_String);
                   auto deviceRole = vr::VRProperties()->GetInt32Property(ulContainer, vr::ETrackedDeviceProperty::Prop_ControllerRoleHint_Int32);
                   std::string inputName(pchName);

                   if (manufacturerName == "Oculus")
                   {
                       if (inputName == "/input/joystick/x")
                       {
                           if (deviceRole == vr::TrackedControllerRole_LeftHand)
                               DeviceController::get().getComponent(deviceSerial, 28).m_orig = *pHandle;
                           if (deviceRole == vr::TrackedControllerRole_RightHand)
                               DeviceController::get().getComponent(deviceSerial, 29).m_orig = *pHandle;
                       }

                       if (inputName == "/input/joystick/y")
                       {
                           if (deviceRole == vr::TrackedControllerRole_LeftHand)
                               DeviceController::get().getComponent(deviceSerial, 30).m_orig = *pHandle;
                           if (deviceRole == vr::TrackedControllerRole_RightHand)
                               DeviceController::get().getComponent(deviceSerial, 31).m_orig = *pHandle;
                       }
                   }
                   else
                   {
                       if (inputName == "/input/thumbstick/x")
                       {
                           if (deviceRole == vr::TrackedControllerRole_LeftHand)
                               DeviceController::get().getComponent(deviceSerial, 28).m_orig = *pHandle;
                           if (deviceRole == vr::TrackedControllerRole_RightHand)
                               DeviceController::get().getComponent(deviceSerial, 29).m_orig = *pHandle;
                       }

                       if (inputName == "/input/thumbstick/y")
                       {
                           if (deviceRole == vr::TrackedControllerRole_LeftHand)
                               DeviceController::get().getComponent(deviceSerial, 30).m_orig = *pHandle;
                           if (deviceRole == vr::TrackedControllerRole_RightHand)
                               DeviceController::get().getComponent(deviceSerial, 31).m_orig = *pHandle;
                       }
                   }

                   if (inputName == "/input/trigger/value")
                   {
                       if (deviceRole == vr::TrackedControllerRole_LeftHand)
                           DeviceController::get().getComponent(deviceSerial, 8).m_orig = *pHandle;
                       if (deviceRole == vr::TrackedControllerRole_RightHand)
                           DeviceController::get().getComponent(deviceSerial, 9).m_orig = *pHandle;
                   }

                   if (inputName == "/input/grip/force")
                   {
                       if (deviceRole == vr::TrackedControllerRole_LeftHand)
                           DeviceController::get().getComponent(deviceSerial, 20).m_orig = *pHandle;
                       if (deviceRole == vr::TrackedControllerRole_RightHand)
                           DeviceController::get().getComponent(deviceSerial, 21).m_orig = *pHandle;
                   }

                   if (inputName == "/input/grip/value")
                   {
                       if (deviceRole == vr::TrackedControllerRole_LeftHand)
                           DeviceController::get().getComponent(deviceSerial, 22).m_orig = *pHandle;
                       if (deviceRole == vr::TrackedControllerRole_RightHand)
                           DeviceController::get().getComponent(deviceSerial, 23).m_orig = *pHandle;
                   }

                   return result;
           });

           rcmp::hook_indirect_function<vr::EVRInputError(void* self, vr::VRInputComponentHandle_t ulComponent, float fNewValue, double fTimeOffset)>(vtable + 3 + vtable_offset, [](auto orig, void* self, vr::VRInputComponentHandle_t ulComponent, float fNewValue, double fTimeOffset) -> vr::EVRInputError
           {
               for (auto& component : DeviceController::get().GetComponents())
               {
                   if (DeviceController::get().GetController(vr::TrackedControllerRole_LeftHand).m_serial == component.first || DeviceController::get().GetController(vr::TrackedControllerRole_RightHand).m_serial == component.first)
                   {
                       for (auto& inputs : component.second)
                       {
                           if (inputs.second.m_orig == ulComponent)
                           {
                               if (DeviceController::get().GetController(vr::TrackedControllerRole_LeftHand).m_manufacturer == "Oculus" || DeviceController::get().GetController(vr::TrackedControllerRole_RightHand).m_manufacturer == "Oculus")
                               {
                                   if (inputs.first == 8)
                                   {
                                       orig(self, DeviceController::get().getComponentOverride(40).m_override, fNewValue, 0);
                                   }

                                   if (inputs.first == 9)
                                   {
                                       orig(self, DeviceController::get().getComponentOverride(41).m_override, fNewValue, 0);
                                   }

                                   if (inputs.first == 22)
                                   {
                                       orig(self, DeviceController::get().getComponentOverride(42).m_override, fNewValue, 0);
                                       orig(self, DeviceController::get().getComponentOverride(44).m_override, fNewValue, 0);
                                       orig(self, DeviceController::get().getComponentOverride(46).m_override, fNewValue, 0);
                                   }

                                   if (inputs.first == 23)
                                   {
                                       orig(self, DeviceController::get().getComponentOverride(43).m_override, fNewValue, 0);
                                       orig(self, DeviceController::get().getComponentOverride(45).m_override, fNewValue, 0);
                                       orig(self, DeviceController::get().getComponentOverride(47).m_override, fNewValue, 0);
                                   }
                               }

                               return orig(self, DeviceController::get().getComponentOverride(inputs.first).m_override, fNewValue, 0);
                           }
                       }
                   }
               }
               return orig(self, ulComponent, fNewValue, fTimeOffset);
           });

           rcmp::hook_indirect_function<vr::EVRInputError(void* self, vr::VRInputComponentHandle_t ulComponent, vr::EVRSkeletalMotionRange eMotionRange, const vr::VRBoneTransform_t* pTransforms, uint32_t unTransformCount)>(vtable + 6 + vtable_offset, [](auto orig, void* self, vr::VRInputComponentHandle_t ulComponent, vr::EVRSkeletalMotionRange eMotionRange, const vr::VRBoneTransform_t* pTransforms, uint32_t unTransformCount) -> vr::EVRInputError
           {
               for (auto& component : DeviceController::get().GetComponents())
               {
                   if (DeviceController::get().GetController(vr::TrackedControllerRole_LeftHand).m_serial == component.first || DeviceController::get().GetController(vr::TrackedControllerRole_RightHand).m_serial == component.first)
                   {
                       for (auto& inputs : component.second)
                       {
                           if (inputs.second.m_orig == ulComponent)
                           {
                               return orig(self, DeviceController::get().getComponentOverride(inputs.first).m_override, vr::VRSkeletalMotionRange_WithController, pTransforms, unTransformCount);
                           }
                       }
                   }
               }
               return orig(self, ulComponent, eMotionRange, pTransforms, unTransformCount);
           });

           m_IVRDriverInputHooked_003 = true;
       }
   }
}