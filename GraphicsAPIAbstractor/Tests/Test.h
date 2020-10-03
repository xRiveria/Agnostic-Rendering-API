#pragma once

namespace Test
{
	class Test
	{
	public:  //Initialization and destruction will all be done within the lifetime of the class so we don't have to worry about memory management. Thus, they will be confined to the lifetime of the test scope. 
		Test() {}
		virtual ~Test() {}

		virtual void OnUpdate(float deltaTime) {}
		virtual void OnRender() {}
		virtual void OnImGuiRender() {}
	};

	class TestMenu : public Test
	{
	public:
		TestMenu(Test*& currentTestPointer);
		~TestMenu();

		void OnImGuiRender() override;
		template<typename T>
		void RegisterTest(const std::string& name)
		{
			std::cout << "Registering Test" << name << "\n";
			m_Tests.push_back(std::make_pair(name, []() { return new T(); }));
		}
		
	private: //Label & Function
		Test*& m_CurrentActiveTest;
		std::vector<std::pair<std::string, std::function<Test* ()>>> m_Tests;
	};
}