#pragma once

#include <stdio.h>

#ifdef NEB_PLATFORM_WINDOWS

// To be defined by client
extern Nebula::Application* Nebula::CreateApplication();

int main(int argc, char** argv) {
	printf("Welcome to Nebula Engine!!!");
	auto app = Nebula::CreateApplication();
	app->Run();
	delete app;
}

#else
#error Nebula only supports Windows!
#endif