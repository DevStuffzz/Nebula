#include "nbpch.h"
#include "Scene.h"
#include "Components.h"
#include "Nebula/Renderer/Renderer.h"
#include "Nebula/Renderer/Material.h"
#include "Nebula/Renderer/Framebuffer.h"
#include "Nebula/Renderer/Shader.h"
#include "Nebula/Renderer/RenderCommand.h"
#include "Nebula/Renderer/Mesh.h"
#include "Nebula/Renderer/Skybox.h"
#include "Platform/OpenGL/OpenGLSkybox.h"
#include "Nebula/Application.h"
#include "Nebula/Scripting/ScriptEngine.h"
#include "Nebula/Scripting/ScriptGlue.h"
#include "Nebula/Physics/PhysicsWorld.h"
#include "Nebula/Physics/PhysicsDebugDraw.h"
#include "Nebula/Audio/AudioEngine.h"
#include "Nebula/Audio/AudioClip.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h> // TODO: Move viewport save/restore to platform-agnostic RenderCommand
namespace Nebula {

	Scene::Scene(const std::string& name)
		: m_Name(name), m_GlobalIllumination(0.1f, 0.1f, 0.1f)
	{
		// Initialize physics
		m_PhysicsWorld = std::make_unique<PhysicsWorld>();
		m_PhysicsWorld->Init();

		// Initialize audio
		m_AudioEngine = std::unique_ptr<AudioEngine>(AudioEngine::Create());
		if (m_AudioEngine)
		{
			if (!m_AudioEngine->Init())
			{
				NB_CORE_ERROR("Failed to initialize audio engine");
				m_AudioEngine.reset();
			}
		}

		// Register script reload callback for hot-reloading
		// TODO: Re-implement script hot-reloading for C# scripts
		// ScriptEngine::RegisterScriptReloadCallback([this](const std::string& scriptPath) {
		// 	this->ClearScriptInitialization(scriptPath);
		// });

		// Initialize shadow shader
		NB_CORE_INFO("Creating scene: {0}", name);
		try
		{
			Shader* rawShader = Shader::Create("Library/shaders/Shadow.glsl");
			if (rawShader)
			{
				m_ShadowShader = std::shared_ptr<Shader>(rawShader);
				NB_CORE_INFO("Shadow shader loaded successfully");
			}
			else
			{
				NB_CORE_WARN("Failed to create shadow shader - shadows will be disabled");
			}
		}
		catch (const std::exception& e)
		{
			NB_CORE_ERROR("Exception loading shadow shader: {0} - shadows will be disabled", e.what());
		}
		catch (...)
		{
			NB_CORE_ERROR("Unknown exception loading shadow shader - shadows will be disabled");
		}
	}

	Scene::~Scene()
	{
		if (m_PhysicsWorld)
		{
			m_PhysicsWorld->Shutdown();
		}

		// AudioEngine::Shutdown() already handles destroying all sources
		if (m_AudioEngine)
		{
			m_AudioEngine->Shutdown();
		}
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		
		// Every entity gets a transform and tag component by default
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<TagComponent>(name.empty() ? "Entity" : name);
		
		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}

	void Scene::Clear()
	{
		m_Registry.clear();
		
		// Clear script initialization tracking
		// TODO: Re-implement for C# scripts
		// m_LuaScriptInitialized.clear();
		
		// Reinitialize physics world to clear all bodies
		if (m_PhysicsWorld)
		{
			m_PhysicsWorld->Shutdown();
			m_PhysicsWorld->Init();
		}
	}

	void Scene::OnRuntimeStart()
	{
		// Initialize script engine with this scene
		ScriptEngine::OnRuntimeStart(this);

		// Initialize all script entities
		auto view = m_Registry.view<ScriptComponent>();
		NB_CORE_INFO("OnRuntimeStart: Found {} entities with ScriptComponent", view.size());
		for (auto entity : view)
		{
			Entity ent = { entity, this };
			auto& sc = ent.GetComponent<ScriptComponent>();
			auto& tag = ent.GetComponent<TagComponent>();
			NB_CORE_INFO("Creating script instance for entity '{}' with class '{}'", tag.Tag, sc.ClassName);
			ScriptEngine::OnCreateEntity(ent);
		}
	}

	void Scene::OnRuntimeStop()
	{
		// Destroy all script instances
		auto view = m_Registry.view<ScriptComponent>();
		for (auto entity : view)
		{
			Entity ent = { entity, this };
			ScriptEngine::OnDestroyEntity(ent);
		}

		// Shutdown script engine runtime
		ScriptEngine::OnRuntimeStop();
	}

	void Scene::OnUpdate(float deltaTime)
	{
		// Check for script file changes (hot-reloading)
		// TODO: Re-implement hot-reloading for C# scripts
		// ScriptEngine::CheckForScriptChanges();

		// Update physics
		if (m_PhysicsWorld)
		{
			// Step physics simulation
			m_PhysicsWorld->Step(deltaTime);

			// Sync transforms from physics to entities
			auto rigidBodyView = m_Registry.view<RigidBodyComponent, TransformComponent>();
			for (auto entityID : rigidBodyView)
			{
				Entity entity = { entityID, this };
				auto& rb = rigidBodyView.get<RigidBodyComponent>(entityID);
				
				// Only sync dynamic bodies (physics controls them)
				if (rb.Type == RigidBodyComponent::BodyType::Dynamic && !rb.IsKinematic)
				{
					m_PhysicsWorld->SyncTransformFromPhysics(entity);
				}
			}
		}

		// Update C# scripts
	{
		ScriptGlue::Update(deltaTime); // Update delayed destroys and other systems
		ScriptGlue::UpdateMouseState(); // Update mouse delta
		
		auto view = m_Registry.view<ScriptComponent>();
		for (auto entity : view)
		{
			Entity ent = { entity, this };
			ScriptEngine::OnUpdateEntity(ent, deltaTime);
		}
	}
		// Update audio listener (find active camera with listener component)
		if (m_AudioEngine)
		{
			auto listenerView = m_Registry.view<AudioListenerComponent, TransformComponent>();
			for (auto entityID : listenerView)
			{
				auto& listener = listenerView.get<AudioListenerComponent>(entityID);
				if (listener.Active)
				{
					auto& transform = listenerView.get<TransformComponent>(entityID);
					m_AudioEngine->SetListenerPosition(transform.Position);
					
					// Calculate forward and up vectors from rotation
					glm::vec3 forward(0.0f, 0.0f, -1.0f);
					glm::vec3 up(0.0f, 1.0f, 0.0f);
					glm::quat rotation = glm::quat(glm::radians(transform.Rotation));
					forward = rotation * forward;
					up = rotation * up;
					m_AudioEngine->SetListenerOrientation(forward, up);
					
					break; // Only one active listener
				}
			}

			// Update audio sources
			auto audioView = m_Registry.view<AudioSourceComponent, TransformComponent>();
			for (auto entityID : audioView)
			{
				Entity entity = { entityID, this };
				auto& audioSource = audioView.get<AudioSourceComponent>(entityID);
				auto& transform = audioView.get<TransformComponent>(entityID);

				// Load audio clip if not already loaded
				if (!audioSource.Clip && !audioSource.AudioClipPath.empty())
				{
					audioSource.Clip = std::shared_ptr<AudioClip>(AudioClip::Create(audioSource.AudioClipPath));
				}

				// Create audio source if needed
				if (audioSource.RuntimeSourceID == 0)
				{
					audioSource.RuntimeSourceID = m_AudioEngine->CreateSource();
					
					if (audioSource.RuntimeSourceID != 0)
					{
						// Set initial properties
						m_AudioEngine->SetSourceVolume(audioSource.RuntimeSourceID, audioSource.Volume);
						m_AudioEngine->SetSourcePitch(audioSource.RuntimeSourceID, audioSource.Pitch);
						m_AudioEngine->SetSourceLoop(audioSource.RuntimeSourceID, audioSource.Loop);
						m_AudioEngine->SetSourceSpatial(audioSource.RuntimeSourceID, audioSource.Spatial);
						m_AudioEngine->SetSourceRolloffFactor(audioSource.RuntimeSourceID, audioSource.RolloffFactor);
						m_AudioEngine->SetSourceReferenceDistance(audioSource.RuntimeSourceID, audioSource.ReferenceDistance);
						m_AudioEngine->SetSourceMaxDistance(audioSource.RuntimeSourceID, audioSource.MaxDistance);
						
						// Attach clip
						if (audioSource.Clip)
						{
							m_AudioEngine->SetSourceClip(audioSource.RuntimeSourceID, audioSource.Clip.get());
						}

						// Play on awake
						if (audioSource.PlayOnAwake)
						{
							m_AudioEngine->PlaySource(audioSource.RuntimeSourceID);
							audioSource.IsPlaying = true;
						}
					}
				}

				// Update position for spatial audio
				if (audioSource.RuntimeSourceID != 0 && audioSource.Spatial)
				{
					m_AudioEngine->SetSourcePosition(audioSource.RuntimeSourceID, transform.Position);
				}

				// Update playing state
				if (audioSource.RuntimeSourceID != 0)
				{
					audioSource.IsPlaying = m_AudioEngine->IsSourcePlaying(audioSource.RuntimeSourceID);
				}
			}

			// Update audio engine
			m_AudioEngine->Update();
		}

		// Update physics debug drawing
		if (m_PhysicsWorld)
		{
			m_PhysicsWorld->DebugDraw();
		}
	}

	void Scene::RenderShadowMaps()
	{
		// Early return if no directional lights or no shadow shader
		size_t numDirLights = m_DirectionalLights.size();
		if (numDirLights == 0 || !m_ShadowShader)
			return;

		const uint32_t SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

		// Save current viewport to restore after shadow rendering
		int viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		
		// Save current framebuffer binding
		GLint currentFramebuffer;
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currentFramebuffer);

		// Ensure we have enough shadow map framebuffers
		if (m_ShadowMapFramebuffers.size() < numDirLights)
		{
			FramebufferSpecification shadowSpec;
			shadowSpec.Width = SHADOW_WIDTH;
			shadowSpec.Height = SHADOW_HEIGHT;
			shadowSpec.SwapChainTarget = false;

			while (m_ShadowMapFramebuffers.size() < numDirLights)
			{
				m_ShadowMapFramebuffers.push_back(std::shared_ptr<Framebuffer>(Framebuffer::Create(shadowSpec)));
			}
		}

		// Render shadow map for each directional light
		auto meshView = m_Registry.view<TransformComponent, MeshRendererComponent>();

		for (size_t i = 0; i < numDirLights && i < 4; ++i)
		{
			auto& light = m_DirectionalLights[i];
			auto& shadowFB = m_ShadowMapFramebuffers[i];

			// Calculate light space matrix
			float orthoSize = 20.0f;
			float nearPlane = 1.0f, farPlane = 50.0f;
			glm::mat4 lightProjection = glm::ortho(-orthoSize, orthoSize, -orthoSize, orthoSize, nearPlane, farPlane);

			// Position light looking at origin from direction
			glm::vec3 lightPos = -light.Direction * 25.0f;
			glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

			light.LightSpaceMatrix = lightProjection * lightView;

			// Render to shadow map
			shadowFB->Bind();
			RenderCommand::Clear();

			m_ShadowShader->Bind();
			m_ShadowShader->SetMat4("u_LightSpaceMatrix", light.LightSpaceMatrix);

			// Render all mesh renderers from light's perspective
			int meshCount = 0;
			for (auto entity : meshView)
			{
				auto [transform, meshRenderer] = meshView.get<TransformComponent, MeshRendererComponent>(entity);

				if (meshRenderer.Mesh)
				{
					m_ShadowShader->SetMat4("u_Transform", transform.GetTransform());
					meshRenderer.Mesh->GetVertexArray()->Bind();
					RenderCommand::DrawIndexed(meshRenderer.Mesh->GetVertexArray());
					meshCount++;
				}
			}

			// Store shadow map texture ID (don't unbind - let next framebuffer bind handle it)
			light.ShadowMapTexture = shadowFB->GetDepthAttachmentRendererID();
		}

		// Restore viewport to what it was before shadow rendering
		glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
		
		// Restore framebuffer binding to what it was before shadow rendering
		glBindFramebuffer(GL_FRAMEBUFFER, currentFramebuffer);
	}

	void Scene::OnRender()
	{
		// Begin the scene with the application camera
		Renderer::BeginScene(Application::Get().GetCamera());

		// Render skybox first (before all other geometry)
		auto skyboxView = m_Registry.view<SkyboxComponent>();
		for (auto entity : skyboxView)
		{
			auto& skybox = skyboxView.get<SkyboxComponent>(entity);
			
			if (skybox.SkyboxInstance)
			{
				// Get camera matrices
				auto& camera = Application::Get().GetCamera();
				glm::mat4 view = camera.GetViewMatrix();
				glm::mat4 projection = camera.GetProjectionMatrix();
				
				// Remove translation from view matrix (keep rotation only)
				view = glm::mat4(glm::mat3(view));
				
				// Set uniforms and render
				auto* skyboxPtr = skybox.SkyboxInstance.get();
				if (auto* openglSkybox = dynamic_cast<OpenGLSkybox*>(skyboxPtr))
				{
					uint32_t shaderProgram = openglSkybox->GetShaderProgram();
					glUseProgram(shaderProgram);
					
					GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
					GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
					
					if (viewLoc != -1)
						glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
					if (projLoc != -1)
						glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);
					
					openglSkybox->Render();
				}
			}
		}

		// Update scene-wide point lights list
		m_PointLights.clear();

		auto lightView = m_Registry.view<PointLightComponent, TransformComponent>();

		for (auto entity : lightView)
		{
			auto& light = lightView.get<PointLightComponent>(entity);
			auto& transform = lightView.get<TransformComponent>(entity);

			m_PointLights.push_back({
				transform.Position,  // use entity's world position
				light.Color,
				light.Intensity,
				light.Radius
				});
		}

		// Update scene-wide directional lights list
		m_DirectionalLights.clear();

		auto dirLightView = m_Registry.view<DirectionalLightComponent, TransformComponent>();

		for (auto entity : dirLightView)
		{
			auto& dirLight = dirLightView.get<DirectionalLightComponent>(entity);
			auto& transform = dirLightView.get<TransformComponent>(entity);

			// Calculate direction from entity's rotation (forward vector)
			glm::quat quat = glm::quat(glm::radians(transform.Rotation));
			glm::vec3 forward = glm::normalize(quat * glm::vec3(0.0f, 0.0f, -1.0f));

			m_DirectionalLights.push_back({
				forward,
				dirLight.Color,
				dirLight.Intensity,
				glm::mat4(1.0f), // LightSpaceMatrix (filled by RenderShadowMaps)
				0 // ShadowMapTexture (filled by RenderShadowMaps)
				});
		}

		// Render shadow maps for directional lights
		RenderShadowMaps();



		// Render all entities with mesh renderer components
		auto view = m_Registry.view<TransformComponent, MeshRendererComponent>();
		int numLights = (int)m_PointLights.size();
		int numDirLights = (int)m_DirectionalLights.size();
		
		for (auto entity : view)
		{
			auto [transform, meshRenderer] = view.get<TransformComponent, MeshRendererComponent>(entity);

			if (meshRenderer.Mesh && meshRenderer.Material)
			{
				std::shared_ptr<Nebula::Shader> shader = meshRenderer.Material->GetShader();
				shader->Bind();
				
				// Set global illumination
				shader->SetFloat3("u_GI", m_GlobalIllumination);
				
				// Set point lights
				shader->SetInt("u_NumPointLights", numLights);
				for (int i = 0; i < numLights && i < 4; ++i)
				{
					std::string idx = std::to_string(i);
					shader->SetFloat3("u_PointLights[" + idx + "].Position", m_PointLights[i].Position);
					shader->SetFloat3("u_PointLights[" + idx + "].Color", m_PointLights[i].Color);
					shader->SetFloat("u_PointLights[" + idx + "].Intensity", m_PointLights[i].Intensity);
					shader->SetFloat("u_PointLights[" + idx + "].Radius", m_PointLights[i].Radius);
				}

			// Set directional lights
			shader->SetInt("u_NumDirectionalLights", numDirLights);
			for (int i = 0; i < numDirLights && i < 4; ++i)
			{
				std::string idx = std::to_string(i);
				shader->SetFloat3("u_DirectionalLights[" + idx + "].Direction", m_DirectionalLights[i].Direction);
				shader->SetFloat3("u_DirectionalLights[" + idx + "].Color", m_DirectionalLights[i].Color);
				shader->SetFloat("u_DirectionalLights[" + idx + "].Intensity", m_DirectionalLights[i].Intensity);
				shader->SetMat4("u_DirectionalLights[" + idx + "].LightSpaceMatrix", m_DirectionalLights[i].LightSpaceMatrix);
				
				// Only bind shadow map if it was successfully created
				if (m_DirectionalLights[i].ShadowMapTexture != 0)
				{
					RenderCommand::BindTexture(1 + i, m_DirectionalLights[i].ShadowMapTexture);
					shader->SetInt("u_ShadowMaps[" + idx + "]", 1 + i);
				}
			}
			
			Renderer::Submit(meshRenderer.Material, meshRenderer.Mesh, transform.GetTransform());
		}
	}

		// End the scene
		Renderer::EndScene();
	}

	void Scene::SetPhysicsDebugDraw(bool enabled)
	{
		if (m_PhysicsWorld)
		{
			m_PhysicsWorld->SetDebugDrawEnabled(enabled);
		}
	}

	std::vector<Entity> Scene::GetAllEntities() const
	{
		std::vector<Entity> entities;

		auto view = m_Registry.view<entt::entity>();
		for (auto entityID : view)
		{
			entities.emplace_back(entityID, const_cast<Scene*>(this));
		}

		return entities;
	}

	bool Scene::ValidateAllScripts(std::string& errorMessage)
	{
		auto view = m_Registry.view<ScriptComponent, TagComponent>();
		for (auto entityID : view)
		{
			auto& scriptComp = view.get<ScriptComponent>(entityID);
			auto& tag = view.get<TagComponent>(entityID);

			if (!scriptComp.ClassName.empty())
			{
				// TODO: Re-implement script validation for C# scripts
				// if (!ScriptEngine::ValidateClass(scriptComp.ClassName))
				// {
				// 	errorMessage = "Script error in entity '" + tag.Tag + "' (" + scriptComp.ClassName + ")";
				// 	return false;
				// }
			}
		}
		return true;
	}

	void Scene::ClearScriptInitialization(const std::string& scriptPath)
	{
		// Remove all initialization flags for entities using this script
		std::vector<std::pair<entt::entity, std::string>> keysToRemove;
		
		for (const auto& [key, initialized] : m_LuaScriptInitialized)
		{
			if (key.second == scriptPath)
			{
				keysToRemove.push_back(key);
			}
		}

		for (const auto& key : keysToRemove)
		{
			m_LuaScriptInitialized.erase(key);
		}

		NB_CORE_INFO("Cleared initialization for {0} entity(ies) using script: {1}", keysToRemove.size(), scriptPath);
	}

}
