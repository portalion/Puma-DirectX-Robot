#include "keyboard.h"

using namespace std;

bool Keyboard::GetState(KeyboardState& state) const
{
	return DeviceBase::GetState(KeyboardState::STATE_TAB_LENGTH*sizeof(BYTE), reinterpret_cast<void*>(&state.m_keys));
}