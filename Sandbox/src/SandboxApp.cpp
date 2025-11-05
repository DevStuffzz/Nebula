

#include <Nebula.h>


class Sandbox : public Nebula::Application {
public:
	Sandbox() {

	}

	~Sandbox() {

	}
};



Nebula::Application* Nebula::CreateApplication() {
	return new Sandbox();
}