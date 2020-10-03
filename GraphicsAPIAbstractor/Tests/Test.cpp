#include "GAAPrecompiledHeader.h"
#include "Test.h"
#include "imgui/imgui.h"

namespace Test
{
	TestMenu::TestMenu(Test*& currentTestPointer) : m_CurrentActiveTest(currentTestPointer)
	{

	}

	TestMenu::~TestMenu()
	{
		for (auto& test : m_Tests)
		{

		}
	}

	void TestMenu::OnImGuiRender()
	{
		for (auto& test : m_Tests)
		{
			if (ImGui::Button(test.first.c_str()))
			{
				m_CurrentActiveTest = test.second();
			}
		}
	}
}
