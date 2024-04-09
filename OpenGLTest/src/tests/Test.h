#pragma once

#include <vector>
#include <string>
#include <functional>

#include "Renderer.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

namespace test
{
	class Test
	{
	public:
		Test() {};
		virtual ~Test() {};

		virtual void OnUpdate(float deltaTime) {}
		virtual void OnRender(Renderer renderer) {}
		virtual void OnImGuiRender(ImGuiIO& io) {}
	};

	class TestMenu : public Test
	{
	public:
		TestMenu(Test*& currentTestPointer);
		
		void OnImGuiRender(ImGuiIO& io) override;

		template <typename T>
		void RegisterTest(const std::string& name)
		{
			Log("Registering test " + name);
			m_Tests.push_back(std::make_pair(name, std::function<Test*()>([]() -> Test* { return new T(); })));
		}

	private:
		Test*& m_CurrentTest;
		std::vector<std::pair<std::string, std::function<Test*()>>> m_Tests;
	};
}
