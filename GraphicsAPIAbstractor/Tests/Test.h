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
}