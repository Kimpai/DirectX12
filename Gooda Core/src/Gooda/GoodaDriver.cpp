#include "GoodaDriver.h"

namespace GoodaCore
{
	GoodaDriver::GoodaDriver(HWND hwnd, Input* inputDevice)
	{
		Direct3D12::Instance()->Init(hwnd);

		//Create the Model object
		m_models.push_back(new Cube(Direct3D12::Instance()->GetDevice(), Direct3D12::Instance()->GetCommandList(), XMFLOAT3(0.0f, 1.0f, 5.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)));
		m_models.push_back(new Cube(Direct3D12::Instance()->GetDevice(), Direct3D12::Instance()->GetCommandList(), XMFLOAT3(5.0f, 1.0f, 5.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)));
		m_models.push_back(new Cube(Direct3D12::Instance()->GetDevice(), Direct3D12::Instance()->GetCommandList(), XMFLOAT3(-5.0f, 1.0f, 5.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)));

		//Pointer to the begining of the vector of models
		ShaderManager::Instance()->CreateDescriptor(m_models[0]->GetConstantBuffer());
		assert(&m_models);

		//Create the light object
		m_lights.push_back(new DirectionalLight(Direct3D12::Instance()->GetDevice(), Direct3D12::Instance()->GetCommandList(),
			XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f), XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f)));

		//Pointer to the begining of the vector of lights
		ShaderManager::Instance()->CreateDescriptor(m_lights[0]->GetConstantBuffer());
		assert(&m_lights);

		//Create the camera object
		m_camera = new Camera(inputDevice, Direct3D12::Instance()->GetDevice(), Direct3D12::Instance()->GetCommandList());

		ShaderManager::Instance()->CreateDescriptor(m_camera->GetConstanBuffer());
		assert(m_camera);

		ShaderManager::Instance()->CreateRootSignature();
		ShaderManager::Instance()->CreatePipelineState({ { ShaderType::VS, L"shaders/ColorVertexShader.hlsl" }, { ShaderType::PS, L"shaders/ColorPixelShader.hlsl" } },
			ShaderPipelineType::COLOR);

		//Close the command list now that all the commands have been recorded
		Direct3D12::Instance()->CloseCommandList();

		//Execute the command list
		Direct3D12::Instance()->ExecuteCommandList();
	}

	GoodaDriver::~GoodaDriver()
	{
		//Release Gooda objects
		m_camera->Release();

		for (auto model : m_models)
			model->Release();

		for (auto light : m_lights)
			light->Release();
	}

	void GoodaDriver::Frame()
	{
		m_camera->Frame(Direct3D12::Instance()->GetCurrentFrame());

		for (auto model : m_models)
			model->Frame(Direct3D12::Instance()->GetCurrentFrame(), m_camera->GetViewMatrix());

		for (auto light : m_lights)
			light->Frame(Direct3D12::Instance()->GetCurrentFrame());

		Render();
	}

	void GoodaDriver::Render()
	{
		//Reset the command list and put it in a recording state
		Direct3D12::Instance()->BeginScene(&ShaderManager::Instance()->GetDepthStencilViewHandle(), nullptr);

		ShaderManager::Instance()->Frame(Direct3D12::Instance()->GetCurrentFrame());

		UINT descriptorSize = Direct3D12::Instance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		CD3DX12_GPU_DESCRIPTOR_HANDLE handle(ShaderManager::Instance()->GetDescriptorHeap(Direct3D12::Instance()->GetCurrentFrame())->GetGPUDescriptorHandleForHeapStart());

		for (auto model : m_models)
			model->Render(Direct3D12::Instance()->GetCommandList(), Direct3D12::Instance()->GetCurrentFrame(), 0, handle);

		handle.Offset(descriptorSize);

		for (auto light : m_lights)
			light->Render(Direct3D12::Instance()->GetCommandList(), Direct3D12::Instance()->GetCurrentFrame(), 0, handle);

		handle.Offset(descriptorSize);

		m_camera->Render(0, handle);

		//Close the command list and execute the commands
		Direct3D12::Instance()->EndScene();

		/*m_direct3D11->BeginScene(m_shaderManager);

		m_direct3D11->EndScene();*/
	}
}