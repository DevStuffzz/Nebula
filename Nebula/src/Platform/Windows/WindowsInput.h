#pragma once
#pragma warning(disable: 4251)

#include "Nebula/Input.h"
#include <unordered_map>

namespace Nebula {

	class WindowsInput : public Input {
	protected:
		virtual bool IsKeyPressedImpl(int keycode) override;
		virtual bool IsKeyDownImpl(int keycode) override;
		virtual void UpdateImpl() override;
		virtual bool IsMouseButtonPressedImpl(int button) override;
		virtual float GetMouseXImpl() override;
		virtual float GetMouseYImpl() override;
		virtual std::pair<float, float> GetMousePosImpl() override;
	
	private:
		std::unordered_map<int, bool> m_KeyStates; // Current frame key states
		std::unordered_map<int, bool> m_PrevKeyStates; // Previous frame key states
	};

}