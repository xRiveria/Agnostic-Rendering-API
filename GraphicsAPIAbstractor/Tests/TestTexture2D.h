#pragma once
#include "Test.h"
#include "VertexArray.h"
#include "OpenGLRenderer.h"
#include "VertexBufferLayout.h"
#include "Texture.h"

namespace Test
{
	class TestTexture2D : public Test
	{
	public:
		TestTexture2D();
		~TestTexture2D();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
 
	private:
		std::unique_ptr<VertexArray> m_VertexArrayObject;
		std::unique_ptr<VertexBuffer> m_VertexBuffer;
		std::unique_ptr<IndexBuffer> m_IndexBuffer;
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Texture> m_Texture;
		std::unique_ptr<Texture> m_SecondTexture;
		glm::mat4 m_ProjectionMatrix, m_ViewMatrix;
		glm::vec3 m_TranslationA, m_TranslationB;
		float m_ClearColor[4];
	};
}