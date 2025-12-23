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
		
		PushLayer(new Cosmic::EditorLayer(projectPath));
	}

	~CosmicEditor()
	{
	}
};

Nebula::Application* Nebula::CreateApplication(int argc, char** argv)
{
	return new CosmicEditor(argc, argv);
}
