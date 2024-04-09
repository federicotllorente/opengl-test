#include "TestClearColor.h"
#include "GLHandleError.h"
#include "imgui/imgui.h"

namespace test
{
	test::TestClearColor::TestClearColor()
		: m_Clear_Color { 0.2f, 0.3f, 0.8f, 1.0f }
	{
	}

	void test::TestClearColor::OnRender(Renderer renderer)
	{
		GL_CALL(glClearColor(
			m_Clear_Color[0],
			m_Clear_Color[1],
			m_Clear_Color[2],
			m_Clear_Color[3]
		));
		GL_CALL(glClear(GL_COLOR_BUFFER_BIT));
	}

	void test::TestClearColor::OnImGuiRender(ImGuiIO& io)
	{
		ImGui::ColorPicker4("Clear color", m_Clear_Color);
	}
}
