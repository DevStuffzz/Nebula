#pragma once
#pragma warning(disable: 4251)

#include "Nebula/Core.h"
#include <unordered_map>

namespace Nebula {
	class NEBULA_API Input {
	public:
		// Returns true only on the frame the key was pressed
		inline static bool IsKeyPressed(int keycode) { return s_Instance->IsKeyPressedImpl(keycode); }
		
		// Returns true while the key is held down
		inline static bool IsKeyDown(int keycode) { return s_Instance->IsKeyDownImpl(keycode); }
		
		// Check if all specified keys are pressed (one frame)
		template<typename... Keys>
		inline static bool IsKeyPressed(int firstKey, Keys... otherKeys) {
			if (!s_Instance->IsKeyPressedImpl(firstKey))
				return false;
			if constexpr (sizeof...(otherKeys) > 0)
				return IsKeyPressed(otherKeys...);
			return true;
		}
		
		// Check if all specified keys are down (continuous)
		template<typename... Keys>
		inline static bool IsKeyDown(int firstKey, Keys... otherKeys) {
			if (!s_Instance->IsKeyDownImpl(firstKey))
				return false;
			if constexpr (sizeof...(otherKeys) > 0)
				return IsKeyDown(otherKeys...);
			return true;
		}
		
		// Called at the end of each frame to update key states
		inline static void Update() { s_Instance->UpdateImpl(); }

		inline static bool IsMouseButtonPressed(int button) { return s_Instance->IsMouseButtonPressedImpl(button); }
		inline static float GetMouseX() { return s_Instance->GetMouseXImpl(); }
		inline static float GetMouseY() { return s_Instance->GetMouseYImpl(); }
		inline static std::pair<float, float> GetMousePos() { return s_Instance->GetMousePosImpl(); }
	
	protected:
		virtual bool IsKeyPressedImpl(int keycode) = 0;
		virtual bool IsKeyDownImpl(int keycode) = 0;
		virtual void UpdateImpl() = 0;

		virtual bool IsMouseButtonPressedImpl(int button) = 0;
		virtual float GetMouseXImpl() = 0;
		virtual float GetMouseYImpl() = 0;
		virtual std::pair<float, float> GetMousePosImpl() = 0;

	private:
		static Input* s_Instance;
	};
}