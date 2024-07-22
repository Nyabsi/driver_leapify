#include <LeapConnection.hpp>

#include <LeapC.h>
#include <thread>
#include <assert.h>

using namespace std::chrono_literals;

bool LeapConnection::Init()
{
	eLeapRS result {};

	LeapCreateConnection(nullptr, &m_connection);

	result = LeapOpenConnection(m_connection);

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
			case eLeapEventType_Tracking:
			{
				LeapHand data { };
				LEAP_TRACKING_EVENT event = *message.tracking_event;

				for (uint32_t i = 0; i < event.nHands; i++)
				{
					auto& hand = event.pHands[i];

					data.role = static_cast<vr::ETrackedControllerRole>(hand.type + 1);
					memcpy(&data.digits, &hand.digits, sizeof(data.digits));
					data.arm = hand.arm;

					if (data.role == vr::TrackedControllerRole_LeftHand)
						m_LeftHand.exchange(data);
					if (data.role == vr::TrackedControllerRole_RightHand)
						m_RightHand.exchange(data);
				}

				break;
			}
			default:
				break;
		}

		std::this_thread::sleep_for(1ms);
	}
}