#pragma once
#pragma warning(disable: 4251)

#include "Nebula/Core.h"
#include <string>
#include <memory>

namespace Nebula {

	class NEBULA_API Skybox
	{
	public:
		virtual ~Skybox() = default;

		virtual void Bind() const = 0;
		virtual void Render() const = 0;
		virtual uint32_t GetRendererID() const = 0;

		static std::shared_ptr<Skybox> Create(const std::string& directoryPath);
	};

}
