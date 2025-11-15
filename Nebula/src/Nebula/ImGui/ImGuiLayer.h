#pragma once

#include "Nebula/Layer.h"

#include "Nebula/Events/ApplicationEvent.h"
#include "Nebula/Events/KeyEvent.h"
#include "Nebula/Events/MouseEvent.h"

namespace Nebula {

	class NEBULA_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(Timestep ts) override;
		virtual void OnImGuiRender() override;

		void Begin();
		void End();
	private:
		void SetDarkThemeColors();
	private:
		float m_Time = 0.0f;
	};

}
