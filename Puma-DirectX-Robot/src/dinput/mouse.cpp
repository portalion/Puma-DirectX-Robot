#include "mouse.h"

using namespace std;

bool Mouse::GetState(MouseState& state) const
{
	return DeviceBase::GetState(sizeof(DIMOUSESTATE), reinterpret_cast<void*>(&state.m_state));
}
