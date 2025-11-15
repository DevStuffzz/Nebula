#include <Nebula.h>
#include "EditorLayer.h"

class CosmicEditor : public Nebula::Application
{
public:
	CosmicEditor()
	{
		NB_INFO("Cosmic Editor Initialized");
		PushLayer(new Cosmic::EditorLayer());
	}

	~CosmicEditor()
	{
	}
};

Nebula::Application* Nebula::CreateApplication()
{
	return new CosmicEditor();
}
