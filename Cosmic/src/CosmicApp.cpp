#include <Nebula.h>
#include "EditorLayer.h"

class CosmicEditor : public Nebula::Application
{
public:
	CosmicEditor(int argc, char** argv)
	{
		NB_INFO("Cosmic Editor Initialized");
		
		std::string projectPath;
		if (argc > 1)
		{
			projectPath = argv[1];
			NB_INFO("Launching with project: {0}", projectPath);
		}
		
		m_EditorLayer = new Cosmic::EditorLayer(projectPath);
		PushLayer(m_EditorLayer);
	}

	~CosmicEditor()
	{
	}

	void Exit(int code = 0) override {
		m_EditorLayer->SetRuntime(false);
		GetWindow().SetCursorVisible(true);
		GetWindow().SetCursorLockMode(0);
	}
private:
	Cosmic::EditorLayer* m_EditorLayer;
};

Nebula::Application* Nebula::CreateApplication(int argc, char** argv)
{
	return new CosmicEditor(argc, argv);
}
