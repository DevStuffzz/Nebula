

#include <Nebula.h>

class ExampleLayer : public Nebula::Layer {
public:
	ExampleLayer() : Layer("Example") {

	}

	void OnUpdate() override {
		NB_INFO("ExampleLayer::OnUpdate");
	}
	
	void OnEvent(Nebula::Event& event) override {
		NB_TRACE("{0}", event.ToString());
	}
};

class Sandbox : public Nebula::Application {
public:
	Sandbox() {
		PushLayer(new ExampleLayer());
	}

	~Sandbox() {

	}
};



Nebula::Application* Nebula::CreateApplication() {
	return new Sandbox();
}