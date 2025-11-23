#pragma once

#include "Script.h"
#include "Components.h"
#include "Nebula/Input.h"
#include "Nebula/Keycodes.h"

namespace Nebula {

	class CameraController : public Script
	{
	public:
		void OnUpdate(float dt) override
		{
			auto& transform = GetComponent<TransformComponent>();
			float speed = 5.0f;

			if (Nebula::Input::IsKeyPressed(NB_KEY_W))
				transform.Position.z -= speed * dt;
			if (Nebula::Input::IsKeyPressed(NB_KEY_S))
				transform.Position.z += speed * dt;
			if (Nebula::Input::IsKeyPressed(NB_KEY_A))
				transform.Position.x -= speed * dt;
			if (Nebula::Input::IsKeyPressed(NB_KEY_D))
				transform.Position.x += speed * dt;
		}
	};

}