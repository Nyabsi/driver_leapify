#include <InterfaceHook.hpp>

#include <stdint.h>
#include <string>

#include <openvr_driver.h>
#include <rcmp.hpp>

void InterfaceHook::Init(vr::IVRDriverContext* pDriverContext)
{
    void** vtable = *((void***)pDriverContext);

    rcmp::hook_indirect_function<void*(*)(void* self, const char* pchInterfaceVersion, void* peError)>(vtable + 0 * sizeof(void*), [this](auto orig, void* self, const char* pchInterfaceVersion, void* peError) -> void* {
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
           rcmp::hook_indirect_function<bool(void* self, const char* pchDeviceSerialNumber, int eDeviceClass, void* pDriver)>(vtable + 0, [this](auto orig, void* self, const char* pchDeviceSerialNumber, int eDeviceClass, void* pDriver) -> bool
           {
               return orig(self, pchDeviceSerialNumber, eDeviceClass, pDriver);
           });

           rcmp::hook_indirect_function<void(*)(void* self, int unWhichDevice, const vr::DriverPose_t& newPose, int unPoseStructSize)>(vtable + 1, [](auto orig, void* self, int unWhichDevice, const vr::DriverPose_t& newPose, int unPoseStructSize) -> void {
               orig(self, unWhichDevice, newPose, unPoseStructSize);
           });

           m_IVRServerDriverHostHooked_006 = true;
       }
   }
}