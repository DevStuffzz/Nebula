#pragma once

#include "Nebula/ImGui/NebulaGui.h"
#include "Nebula/Log.h"
#include <vector>
#include <string>
#include <glm/glm.hpp>

namespace Cosmic {

	enum class LogLevel
	{
		Info,
		Warning,
		Error
	};

	struct LogMessage
	{
		std::string Message;
		LogLevel Level;
	};

	class ConsoleWindow
	{
	public:
		static void OnImGuiRender()
		{
			Nebula::NebulaGui::Begin("Console");

			// Clear button
			if (Nebula::NebulaGui::Button("Clear"))
			{
				s_Messages.clear();
			}

			Nebula::NebulaGui::Separator();

			// Display messages
			for (const auto& msg : s_Messages)
			{
				glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
				
				switch (msg.Level)
				{
				case LogLevel::Info:
					color = { 0.7f, 0.7f, 0.7f, 1.0f };
					break;
				case LogLevel::Warning:
					color = { 1.0f, 0.8f, 0.0f, 1.0f };
					break;
				case LogLevel::Error:
					color = { 1.0f, 0.2f, 0.2f, 1.0f };
					break;
				}

				Nebula::NebulaGui::TextColored(color, msg.Message.c_str());
			}

			// Auto-scroll to bottom
			if (Nebula::NebulaGui::GetScrollY() >= Nebula::NebulaGui::GetScrollMaxY())
			{
				Nebula::NebulaGui::SetScrollHereY(1.0f);
			}

			Nebula::NebulaGui::End();
		}

		static void AddLog(const std::string& message, LogLevel level = LogLevel::Info)
		{
			s_Messages.push_back({ message, level });
			
			// Limit console history to prevent memory issues
			if (s_Messages.size() > 1000)
			{
				s_Messages.erase(s_Messages.begin());
			}
		}

		static void Clear()
		{
			s_Messages.clear();
		}

	private:
		static std::vector<LogMessage> s_Messages;
	};

	// Static member initialization
	inline std::vector<LogMessage> ConsoleWindow::s_Messages;

}
