#pragma once
#pragma warning(disable: 4251)

#include "Scene.h"
#include <string>

namespace Nebula {

	class NEBULA_API SceneSerializer
	{
	public:
		SceneSerializer(Scene* scene);

		// Serialize scene to JSON and save to .nebscene file
		bool Serialize(const std::string& filepath);
		
		// Deserialize scene from .nebscene file
		bool Deserialize(const std::string& filepath);

		// Serialize to JSON string (for debugging/copying)
		std::string SerializeToString();

	private:
		Scene* m_Scene;
	};

}
