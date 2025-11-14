

#include <Nebula.h>

class ExampleLayer : public Nebula::Layer {
public:
	ExampleLayer() : Layer("Example") {

	}

	void OnUpdate() override {
	}
	
	void OnEvent(Nebula::Event& event) override {
	}

	void OnImGuiRender() override {
		ImGui::ImText("Hello from client");
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