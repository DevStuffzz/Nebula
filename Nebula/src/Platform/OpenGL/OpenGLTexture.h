#pragma once
#pragma warning(disable: 4251)

#include "Nebula/Renderer/Texture.h"

namespace Nebula {

	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const std::string& path, bool useNearest = false, bool repeat = true);
		virtual ~OpenGLTexture2D();

		virtual uint32_t GetWidth() const override { return m_Width; }
		virtual uint32_t GetHeight() const override { return m_Height; }
		virtual uint32_t GetRendererID() const override { return m_RendererID; }

		virtual void Bind(uint32_t slot = 0) const override;

		const std::string& GetPath() const { return m_Path; }
		bool GetUseNearest() const { return m_UseNearest; }
		bool GetRepeat() const { return m_Repeat; }

	private:
		std::string m_Path;
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID;
		bool m_UseNearest;
		bool m_Repeat;
	};

}
