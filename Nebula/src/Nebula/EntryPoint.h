#pragma once
#pragma warning(disable: 4251)


#ifdef NB_PLATFORM_WINDOWS

// To be defined by client
extern Nebula::Application* Nebula::CreateApplication();

int main(int argc, char** argv) {
	Nebula::Log::Init();

	NB_CORE_INFO("Initialized Log!");
	auto app = Nebula::CreateApplication();
	app->Run();
	delete app;
	return 0;
}

#elif defined(NB_PLATFORM_MACOS)

// To be defined by client
extern Nebula::Application* Nebula::CreateApplication();

int main(int argc, char** argv) {
	Nebula::Log::Init();

	NB_CORE_WARN("Initialized Log!");
	auto app = Nebula::CreateApplication();
	app->Run();
	delete app;
}

#else
#error Nebula only supports Windows and macOS!
#endif