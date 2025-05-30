#include <core/LeapConnection.hpp>

#include <thread>
#include <chrono>
#include <cstring>
#include <assert.h>

#include <LeapC.h>
#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>

using namespace std::chrono_literals;

bool LeapConnection::Init()
{
	eLeapRS result {};

	LeapCreateConnection(nullptr, &m_connection);

	result = LeapOpenConnection(m_connection);

	LeapSetTrackingMode(m_connection, eLeapTrackingMode_HMD); // Force to HMD mode

	return result == eLeapRS::eLeapRS_Success;
}

void LeapConnection::Begin()
{
	m_isPolling.exchange(true);

	m_pollThread = std::thread(&LeapConnection::Poll, this);
	m_pollThread.detach();
}

void LeapConnection::End()
{
	m_isPolling.exchange(false);

	if (m_pollThread.joinable())
		m_pollThread.join();
}

LeapHand LeapConnection::GetHand(vr::ETrackedControllerRole role) const
{
	if (role == vr::TrackedControllerRole_LeftHand)
		return m_LeftHand;

	if (role == vr::TrackedControllerRole_RightHand)
		return m_RightHand;

	return LeapHand();
}

void LeapConnection::Poll()
{
	eLeapRS result { eLeapRS::eLeapRS_Success };

	while (m_isPolling)
	{
		LEAP_CONNECTION_MESSAGE message { };
		result = LeapPollConnection(m_connection, 1000, &message);

		if (result == eLeapRS::eLeapRS_Timeout)
			continue;

		switch (message.type)
		{
			case eLeapEventType_Device:
				m_DeviceCount.exchange(m_DeviceCount + 1);
				break;
			case eLeapEventType_DeviceLost:
				m_DeviceCount.exchange(m_DeviceCount - 1);
				break;
			case eLeapEventType_IMU:
			{
				LEAP_IMU_EVENT event = *message.imu_event;
				m_accelerometer.exchange(event.accelerometer);
				break;
			}
			case eLeapEventType_Tracking:
			{
				LeapHand data { };
				LEAP_TRACKING_EVENT event = *message.tracking_event;
				data.timestamp = event.info.timestamp;

				if (m_frameId != event.tracking_frame_id)
				{
					bool skipLeft = false;
					bool skipRight = false;

					for (uint32_t i = 0; i < event.nHands; i++)
					{
						auto& hand = event.pHands[i];

						data.role = static_cast<vr::ETrackedControllerRole>(hand.type + 1);
						memcpy(&data.digits, &hand.digits, sizeof(data.digits));
						data.palm = hand.palm;
						data.arm = hand.arm;
						data.accelerometer = m_accelerometer;

						if (data.role == vr::TrackedControllerRole_LeftHand)
						{
							m_LeftHand.exchange(data);
							skipLeft = true;
						}
							
						if (data.role == vr::TrackedControllerRole_RightHand)
						{
							m_RightHand.exchange(data);
							skipRight = true;
						}
					}

					if (!skipLeft)
						m_LeftHand.exchange(LeapHand());

					if (!skipRight)
						m_RightHand.exchange(LeapHand());

					m_frameId.exchange(event.tracking_frame_id);
				}

				break;
			}
			default:
				break;
		}

		std::this_thread::sleep_for(10ns);
	}
}