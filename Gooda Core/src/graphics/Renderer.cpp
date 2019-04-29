#include "Renderer.h"

namespace GoodaCore
{
	Renderer::Renderer() : m_backend(Backend::NOACTIVE)
	{
		
	}

	bool Renderer::Init(Backend backend)
	{
		m_backend = backend;
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

	void Renderer::SetBackend(Backend backend)
	{
		switch (backend)
		{
		case GoodaCore::Backend::DX11:
			m_backend = Backend::DX11;

			std::cout << "Swapped backend renderer, Now DX 11" << std::endl;
			break;
		case GoodaCore::Backend::DX12:
			m_backend = Backend::DX12;

			std::cout << "Swapped backend renderer, Now DX 12" << std::endl;
			break;
		case GoodaCore::Backend::NOACTIVE:
			std::cout << "No active backend renderer set, defaulting to latest" << std::endl;
			SetBackend(m_backend);
			break;
		default:
			break;
		}
	}

	bool Renderer::Render(std::vector<Model*>& models)
	{
		for (auto model : models)
			model->Draw();

		return true;
	}
}