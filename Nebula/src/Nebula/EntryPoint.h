#pragma once


#ifdef NEB_PLATFORM_WINDOWS

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
#error Nebula only supports Windows!
#endif