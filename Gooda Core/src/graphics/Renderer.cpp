#include "Renderer.h"

namespace GoodaCore
{
	Renderer::Renderer() : m_backend(Backend::NOACTIVE)
	{

	}

	Renderer::~Renderer()
	{
	}

	Renderer* Renderer::Instance()
	{
		static Renderer s_renderer;
		return &s_renderer;
	}

	void* Renderer::GetDevice()
	{
		void* device = nullptr;

		switch (m_backend)
		{
		case Backend::DX11:
			device = Direct3D11::Instance()->GetDevice();
			break;
		case Backend::DX12:
			device = Direct3D12::Instance()->GetDevice();
			break;
		default:
			break;
		}

		return device;
	}

	void* Renderer::GetDeviceContextOrCommandList()
	{
		void* device = nullptr;

		switch (m_backend)
		{
		case Backend::DX11:
			device = Direct3D11::Instance()->GetDeviceContext();
			break;
		case Backend::DX12:
			device = Direct3D12::Instance()->GetCommandList();
			break;
		default:
			break;
		}

		return device;
	}

	Backend Renderer::GetBackend()
	{
		return m_backend;
	}

	int Renderer::GetCurrentFrame()
	{
		int frameIndex = -1;

		switch (m_backend)
		{
		case Backend::DX11:
			frameIndex = Direct3D11::Instance()->GetCurrentFrame();
			break;
		case Backend::DX12:
			frameIndex = Direct3D12::Instance()->GetCurrentFrame();
			break;
		default:
			break;
		}

		return frameIndex;
	}

	void Renderer::Render()
	{
		switch (m_backend)
		{
		case Backend::DX11:
			Direct3D11::Instance()->BeginScene();

			Direct3D11::Instance()->EndScene();
			break;
		case Backend::DX12:
			Direct3D12::Instance()->BeginScene(NULL, NULL);

			Direct3D12::Instance()->EndScene();
			break;
		default:
			break;
		}
	}
}