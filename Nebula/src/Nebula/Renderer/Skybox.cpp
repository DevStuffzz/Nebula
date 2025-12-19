#include "nbpch.h"
#include "Skybox.h"
#include "Platform/OpenGL/OpenGLSkybox.h"
#include "RendererAPI.h"
#include "Nebula/Log.h"

namespace Nebula {

	std::shared_ptr<Skybox> Skybox::Create(const std::string& directoryPath)
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:    return nullptr;
		case RendererAPI::API::OpenGL:  return std::make_shared<OpenGLSkybox>(directoryPath);
		}

		return nullptr;
	}

}
