#pragma once

#include "Test.h"

namespace test
{
	class TestClearColor : public Test
	{
	public:
		TestClearColor();

		void OnRender(Renderer renderer) override;
		void OnImGuiRender(ImGuiIO& io) override;

	private:
		float m_Clear_Color[4];
	};
}
