#include "GAAPrecompiledHeader.h"
#include "TestClearColor.h"
#include "OpenGLRenderer.h"
#include "imgui/imgui.h"

Test::TestClearColor::TestClearColor() : m_ClearColor { 0.2f, 0.3f, 0.8f, 1.0f }
{
}

Test::TestClearColor::~TestClearColor()
{
}

void Test::TestClearColor::OnUpdate(float deltaTime)
{
}

void Test::TestClearColor::OnRender()
{
	glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]);
	glClear(GL_COLOR_BUFFER_BIT);
}

void Test::TestClearColor::OnImGuiRender()
{
	ImGui::ColorPicker4("Clear Color", m_ClearColor);
}
