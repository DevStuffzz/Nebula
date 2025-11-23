#pragma once

#include "Entity.h"

namespace Nebula {

	class Script
	{
	public:
		virtual ~Script() = default;

		template<typename T>
		T& GetComponent()
		{
			return m_Entity.GetComponent<T>();
		}

	protected:
		virtual void OnCreate() {}
		virtual void OnUpdate(float dt) {}
		virtual void OnDestroy() {}

	protected:
		Entity m_Entity;
		friend class Scene;
	};

}