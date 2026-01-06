#pragma once

#include "Nebula/ImGui/NebulaGui.h"
#include "Nebula/Log.h"
#include <vector>
#include <string>
#include <glm/glm.hpp>

namespace Cosmic {



	class ConsoleWindow
	{
	public:
		static void OnImGuiRender()
		{
			auto& messages = Nebula::Log::GetClientMessages();


			Nebula::NebulaGui::Begin("Console");

			// Clear button
			if (Nebula::NebulaGui::Button("Clear"))
			{
				Nebula::Log::ClearClientMessages();
			}

			Nebula::NebulaGui::SameLine();

			// Copy button
			if (Nebula::NebulaGui::Button("Copy All"))
			{
				std::string allText;
				for (const auto& msg : *messages)
				{
					allText += msg.Message + "\n";
				}
				Nebula::NebulaGui::SetClipboardText(allText.c_str());
			}

			Nebula::NebulaGui::Separator();

			// Display messages with context menu for copying
			for (size_t i = 0; i < messages->size(); i++)
			{
				const auto& msg = messages->at(i);
				glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
				
				switch (msg.Level)
				{
				case Nebula::LogLevel::LOG_TRACE:
					color = { 0.7f, 0.7f, 0.7f, 1.0f };
					break;
				case Nebula::LogLevel::LOG_INFO:
					color = { 0.3f, 0.7f, 0.7f, 1.0f };
					break;
				case Nebula::LogLevel::LOG_WARN:
					color = { 1.0f, 0.8f, 0.0f, 1.0f };
					break;
				case Nebula::LogLevel::LOG_ERROR:
					color = { 1.0f, 0.2f, 0.2f, 1.0f };
					break;
				}

				Nebula::NebulaGui::TextColored(color, msg.Message.c_str());

				// Right-click context menu for individual messages
				std::string popupID = "LogContextMenu_" + std::to_string(i);
				if (Nebula::NebulaGui::BeginPopupContextItem(popupID.c_str()))
				{
					if (Nebula::NebulaGui::MenuItem("Copy"))
					{
						Nebula::NebulaGui::SetClipboardText(msg.Message.c_str());
					}
					Nebula::NebulaGui::EndPopup();
				}
			}

			// Auto-scroll to bottom
			if (Nebula::NebulaGui::GetScrollY() >= Nebula::NebulaGui::GetScrollMaxY())
			{
				Nebula::NebulaGui::SetScrollHereY(1.0f);
			}

			Nebula::NebulaGui::End();
		}

		static void AddLog(Nebula::LogMessage message)
		{
			auto& messages = Nebula::Log::GetClientMessages();
			
			// Limit console history to prevent memory issues
			if (messages->size() > 1000)
			{
				messages->erase(messages->begin());
			}
			messages->push_back(message);
		}

		static void AddLog(const std::string& message, Nebula::LogLevel level)
		{
			AddLog(Nebula::LogMessage(message, level));
		}

		static void Clear()
		{
			Nebula::Log::ClearClientMessages();
		}

	};


}
