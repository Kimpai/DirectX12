#include "RenderManager.h"

RenderManager::RenderManager(HWND hwnd) : m_hwnd(hwnd)
{
	m_activeRenderer = Backend::NOACTIVE;

	SetBackendRenderer(Backend::DX12);
}

RenderManager::~RenderManager()
{
}

void RenderManager::SetBackendRenderer(Backend backendRenderer)
{
	m_activeRenderer = backendRenderer;

	switch (m_activeRenderer)
	{
	case Backend::DX11:
		if (!m_direct3D11)
			CreateDirect3DRenderer();
		break;

	case Backend::DX12:
		if (!m_direct3D12)
			CreateDirect3DRenderer();
		break;

	default:
		break;
	}
}

Backend RenderManager::GetBackendRenderer()
{
	return m_activeRenderer;
}

int RenderManager::GetCurrentFrame()
{
	int frameIndex = -1;

	switch (m_activeRenderer)
	{
	case Backend::DX11:
		frameIndex = m_direct3D11->GetCurrentFrame();
		break;
	case Backend::DX12:
		frameIndex = m_direct3D12->GetCurrentFrame();
		break;
	default:
		break;
	}

	return frameIndex;
}

void RenderManager::BeginScene(D3D12_CPU_DESCRIPTOR_HANDLE* cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE* gpuHandle)
{
	if (m_activeRenderer == Backend::DX11)
		BeginScene();

	m_direct3D12->BeginScene(cpuHandle, gpuHandle);
}

void RenderManager::BeginScene()
{
	m_direct3D11->BeginScene();
}

void RenderManager::EndScene()
{
	switch (m_activeRenderer)
	{
	case Backend::DX11:
		m_direct3D11->EndScene();
		break;
	case Backend::DX12:
		m_direct3D12->EndScene();
		break;
	default:
		break;
	}
}

void RenderManager::Render()
{

}

void RenderManager::CreateDirect3DRenderer()
{	
	switch (m_activeRenderer)
	{
	case Backend::DX11:
		m_direct3D11 = std::make_unique<Direct3D11>(m_hwnd);
		break;

	case Backend::DX12:
		m_direct3D12 = std::make_unique<Direct3D12>(m_hwnd);
		break;

	default:
		break;
	}
}