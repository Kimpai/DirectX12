#include "Renderer.h"

namespace GoodaCore
{
	Renderer::Renderer() : m_backend(Backend::NOACTIVE)
	{
		
	}

	bool Renderer::Init()
	{
		m_backend = Backend::DX12;
		return true;
	}

	bool Renderer::Destroy()
	{
		return true;
	}

	Renderer* Renderer::Instance()
	{
		static Renderer s_renderer;
		return &s_renderer;
	}

	Backend Renderer::GetBackend()
	{
		return m_backend;
	}

	bool Renderer::Render(std::vector<Model*>& models)
	{
		for (auto model : models)
			model->Draw();

		return true;
	}
	bool Renderer::BeginScene(Window* window)
	{
		return false;
	}

	bool Renderer::EndScene(Window* window)
	{
		return false;
	}
}