#include "nbpch.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Nebula {

	RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;

}
