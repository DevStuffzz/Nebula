#pragma once
#pragma warning(disable: 4251)


#include "Nebula/Core.h"
#include "Nebula/Events/Event.h"
#include "Nebula/Timestep.h"

namespace Nebula {

	class NEBULA_API Layer {
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(Timestep ts) {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}

		inline const std::string& GetName() const { return m_DebugName; }

	protected:
		std::string m_DebugName;
	};
}