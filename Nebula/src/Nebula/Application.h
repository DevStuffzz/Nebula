#pragma once

#include "Core.h"

namespace Nebula {
	class NEBULA_API Application
	{
	public:
		Application();
		~Application();

		void Run();
	};

	// To be defined in Client
	Application* CreateApplication();
}

