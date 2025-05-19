#pragma once

#include <openvr_driver.h>

namespace hooks {
    namespace IVRDriverInput {
        // note: idx 1
        bool UpdateBooleanComponent(vr::VRInputComponentHandle_t& ulComponent, bool& bNewValue, double& fTimeOffset) {
            return false;
        }

        void CreateSkeletonComponent(vr::PropertyContainerHandle_t& ulContainer, const char*& pchName, const char*& pchSkeletonPath, const char*& pchBasePosePath, vr::EVRSkeletalTrackingLevel& eSkeletalTrackingLevel, const vr::VRBoneTransform_t*& pGripLimitTransforms, uint32_t& unGripLimitTransformCount, vr::VRInputComponentHandle_t*& pHandle) {

        }

		bool UpdateSkeletonComponent(vr::VRInputComponentHandle_t& ulComponent, vr::EVRSkeletalMotionRange& eMotionRange, const vr::VRBoneTransform_t*& pTransforms, uint32_t& unTransformCount) {
            return false;
        }
    };
}