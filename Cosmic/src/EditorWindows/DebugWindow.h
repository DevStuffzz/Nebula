#pragma once

#include "Nebula/ImGui/NebulaGui.h"
#include "Nebula/Scene/Scene.h"
#include "Nebula/Scene/Components.h"
#include "Nebula/Application.h"
#include <memory>
#include <chrono>

namespace Cosmic {

	class DebugWindow
	{
	public:
		static void OnImGuiRender(const std::shared_ptr<Nebula::Scene>& scene, bool isRuntimeMode)
		{
			Nebula::NebulaGui::Begin("Debug Info");

			// FPS Counter
			UpdateFPS();
			Nebula::NebulaGui::Text("FPS: %.1f (%.2f ms)", s_FPS, s_FrameTime);
			Nebula::NebulaGui::Separator();

			// Scene Stats
			if (scene)
			{
				Nebula::NebulaGui::Text("Scene Statistics");

				// Entity count
				auto entities = scene->GetAllEntities();
				Nebula::NebulaGui::Text("  Total Entities: %d", (int)entities.size());

				// Script components
				auto& registry = scene->GetRegistry();
				auto scriptView = registry.view<Nebula::ScriptComponent>();
				int activeScripts = 0;
				for (auto entity : scriptView)
				{
					auto& sc = scriptView.get<Nebula::ScriptComponent>(entity);
					for (const auto& scriptPath : sc.ScriptPaths)
					{
						if (!scriptPath.empty())
							activeScripts++;
					}
				}
				Nebula::NebulaGui::Text("  Active Scripts: %d", activeScripts);

				// Audio sources
				auto audioView = registry.view<Nebula::AudioSourceComponent>();
				int totalAudioSources = 0;
				int playingAudioSources = 0;
				for (auto entity : audioView)
				{
					totalAudioSources++;
					auto& audio = audioView.get<Nebula::AudioSourceComponent>(entity);
					if (audio.IsPlaying)
						playingAudioSources++;
				}
				Nebula::NebulaGui::Text("  Audio Sources: %d (Playing: %d)", totalAudioSources, playingAudioSources);

				// Audio listeners
				auto listenerView = registry.view<Nebula::AudioListenerComponent>();
				int activeListeners = 0;
				for (auto entity : listenerView)
				{
					auto& listener = listenerView.get<Nebula::AudioListenerComponent>(entity);
					if (listener.Active)
						activeListeners++;
				}
				Nebula::NebulaGui::Text("  Audio Listeners: %d (Active: %d)", 
					(int)listenerView.size(), activeListeners);

				// Physics objects
				auto rigidBodyView = registry.view<Nebula::RigidBodyComponent>();
				int dynamicBodies = 0;
				int staticBodies = 0;
				int kinematicBodies = 0;
				for (auto entity : rigidBodyView)
				{
					auto& rb = rigidBodyView.get<Nebula::RigidBodyComponent>(entity);
					if (rb.Type == Nebula::RigidBodyComponent::BodyType::Dynamic)
					{
						if (rb.IsKinematic)
							kinematicBodies++;
						else
							dynamicBodies++;
					}
					else if (rb.Type == Nebula::RigidBodyComponent::BodyType::Static)
					{
						staticBodies++;
					}
				}
				Nebula::NebulaGui::Text("  Physics Bodies: %d", (int)rigidBodyView.size());
				Nebula::NebulaGui::Text("    Dynamic: %d", dynamicBodies);
				Nebula::NebulaGui::Text("    Kinematic: %d", kinematicBodies);
				Nebula::NebulaGui::Text("    Static: %d", staticBodies);

				// Mesh renderers
				auto meshView = registry.view<Nebula::MeshRendererComponent>();
				Nebula::NebulaGui::Text("  Mesh Renderers: %d", (int)meshView.size());

				// Cameras
				auto cameraView = registry.view<Nebula::CameraComponent>();
				int primaryCameras = 0;
				for (auto entity : cameraView)
				{
					auto& cam = cameraView.get<Nebula::CameraComponent>(entity);
					if (cam.Primary)
						primaryCameras++;
				}
				Nebula::NebulaGui::Text("  Cameras: %d (Primary: %d)", (int)cameraView.size(), primaryCameras);

				// Lights
				auto pointLightView = registry.view<Nebula::PointLightComponent>();
				auto dirLightView = registry.view<Nebula::DirectionalLightComponent>();
				Nebula::NebulaGui::Text("  Point Lights: %d", (int)pointLightView.size());
				Nebula::NebulaGui::Text("  Directional Lights: %d", (int)dirLightView.size());

				Nebula::NebulaGui::Separator();

				// Rendering Stats (placeholder - would need to be tracked by renderer)
				Nebula::NebulaGui::Text("Rendering (Estimated)");
				Nebula::NebulaGui::Text("  Draw Calls: ~%d", (int)meshView.size() + (int)pointLightView.size());
				Nebula::NebulaGui::Separator();

				// Runtime mode indicator
				Nebula::NebulaGui::Text("Runtime Mode: %s", isRuntimeMode ? "ON" : "OFF");
			}
			else
			{
				Nebula::NebulaGui::TextColored({ 1.0f, 0.5f, 0.0f, 1.0f }, "No Active Scene");
			}

			Nebula::NebulaGui::End();
		}

	private:
		static void UpdateFPS()
		{
			static auto lastTime = std::chrono::high_resolution_clock::now();
			static int frameCount = 0;
			static float fpsUpdateTimer = 0.0f;

			auto currentTime = std::chrono::high_resolution_clock::now();
			std::chrono::duration<float> deltaTime = currentTime - lastTime;
			lastTime = currentTime;

			float dt = deltaTime.count();
			s_FrameTime = dt * 1000.0f; // Convert to milliseconds

			frameCount++;
			fpsUpdateTimer += dt;

			// Update FPS every 0.5 seconds
			if (fpsUpdateTimer >= 0.5f)
			{
				s_FPS = frameCount / fpsUpdateTimer;
				frameCount = 0;
				fpsUpdateTimer = 0.0f;
			}
		}

		inline static float s_FPS = 0.0f;
		inline static float s_FrameTime = 0.0f;
	};

}
