#include "nbpch.h"

#include "Application.h"

#include "Nebula/Events/ApplicationEvent.h"
#include "Nebula/Log.h"

namespace Nebula {
	Application::Application()
	{

	}

	Application::~Application()
	{

	}
	
	void Application::Run() {
		WindowResizeEvent e(1280, 720);
		if (e.IsInCategory(EventCategoryApplication))
		{
			NB_TRACE(e.ToString());
		}
		if (e.IsInCategory(EventCategoryInput))
		{
			NB_TRACE(e.ToString());
		}

		while (true);
	}
}