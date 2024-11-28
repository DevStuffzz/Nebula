#pragma once


#ifdef NB_PLATFORM_WINDOWS

extern Nebula::Application* Nebula::CreateApplication();

int main(int argc, char** argv) {
	printf("Nebula Engine\n");
	auto app = Nebula::CreateApplication();
	app->Run();
	delete app;
}

#endif // NB_PLATFORM_WINDOWS
