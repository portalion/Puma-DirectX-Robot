#include "diDeviceBase.h"
#include "utils/exceptions.h"

bool DeviceBase::GetState(unsigned int size, void* ptr) const
{
	if (!m_device)
		return false;
	for (auto i = 0; i < GET_STATE_RETRIES; ++i)
	{
		auto hr = m_device->GetDeviceState(size, ptr);
		if (SUCCEEDED(hr))
			return true;
		if (hr != DIERR_INPUTLOST && hr != DIERR_NOTACQUIRED)
			THROW_DX(hr);
		for (int j = 0; j < AQUIRE_RETRIES; ++j)
		{
			hr = m_device->Acquire();
			if (SUCCEEDED(hr))
				break;
			if (hr != DIERR_INPUTLOST && hr != E_ACCESSDENIED)
				THROW_DX(hr);;
		}
	}
	return false;
}
