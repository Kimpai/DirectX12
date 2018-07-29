#pragma once

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

#include <assert.h>
#include <frame.h>
#include <Gooda.h>
#include <memory>

#include "Direct3D11.h"
#include "Direct3D12.h"

enum class Backend { DX11, DX12, NOACTIVE };

class RenderManager : public Gooda
{
public:
	RenderManager(HWND);
	~RenderManager();

	void SetBackendRenderer(Backend);
	Backend GetBackendRenderer();
	int GetCurrentFrame();

	void BeginScene(D3D12_CPU_DESCRIPTOR_HANDLE*, D3D12_GPU_DESCRIPTOR_HANDLE*);
	void BeginScene();

	void EndScene();
	void Render();

private:
	void CreateDirect3DRenderer();

	HWND m_hwnd;
	Backend m_activeRenderer;

	std::unique_ptr<Direct3D11> m_direct3D11;
	std::unique_ptr<Direct3D12> m_direct3D12;
};