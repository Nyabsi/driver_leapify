#include <hook/HookManager.hpp>
#include <util/HashString.hpp>

#include <rcmp.hpp>
#include <openvr_driver.h>

#include <hook/hooks/IVRDriverInput_003.hpp>
#include <hook/hooks/IVRServerDriverHost_006.hpp>

#ifdef _WIN32
    constexpr int vtable_offset = 0;
#else
    constexpr int vtable_offset = 1;
#endif

HookManager::HookManager()
{
    m_IVRDriverInputHooked = false;
    m_IVRServerDriverHostHooked = false;
}

HookManager::~HookManager()
{

}

void HookManager::Initialize(vr::IVRDriverContext* pContext)
{
    void** pVtable = *((void***)pContext);

    auto GetGenericInterface = [this](auto orig, void* self, const char* pchInterfaceVersion, void* peError) -> void* {
        void* pTablePtr = orig(self, pchInterfaceVersion, peError);

        uint64_t hash = util::hashString(std::string(pchInterfaceVersion));
        switch (hash) {
            case "IVRDriverInput_003"_hash:
                hookIVRDriverInput(pTablePtr);
                break;
            case "IVRServerDriverHost_006"_hash:
                hookIVRServerDriverHost(pTablePtr);
                break;
            default:
                break; // ¯\_(ツ)_/¯
        }

        return pTablePtr;
    };

    rcmp::hook_indirect_function<void*(*)(void* self, const char* pchInterfaceVersion, void* peError)>(pVtable + 0 + vtable_offset, GetGenericInterface);
}

void HookManager::hookIVRDriverInput(void* pTablePtr)
{
    void** pVtable = *((void***)pTablePtr);

    if (!m_IVRDriverInputHooked)
    {
        m_IVRDriverInputHooked = true;
    }
}

void HookManager::hookIVRServerDriverHost(void* pTablePtr)
{
    void** pVtable = *((void***)pTablePtr);

    if (!m_IVRServerDriverHostHooked)
    {
        auto _TrackedDevicePoseUpdated = [this](auto orig, void* self, uint32_t unWhichDevice, const vr::DriverPose_t& newPose, uint32_t unPoseStructSize) -> void {
            hooks::IVRServerDriverHost::TrackedDevicePoseUpdated(unWhichDevice, newPose, unPoseStructSize);
        };

        rcmp::hook_indirect_function<void(*)(void* self, uint32_t unWhichDevice, const vr::DriverPose_t& newPose, uint32_t unPoseStructSize)>(pVtable + 1 + vtable_offset, _TrackedDevicePoseUpdated);

        m_IVRServerDriverHostHooked = true;
    }
}