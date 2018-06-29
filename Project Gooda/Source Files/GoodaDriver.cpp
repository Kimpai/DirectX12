#include "GoodaDriver.h"

GoodaDriver::GoodaDriver(HWND hwnd)
{
	//Create the Direct3D object
	m_direct3D = new Direct3D(SCREEN_HEIGHT, SCREEN_WIDTH, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	assert(m_direct3D);

	//Create the Shader manager object
	m_shaderManager = new ShaderManager();
	assert(&m_shaderManager);

	//Create the Model object
	m_models.push_back(new Cube(m_direct3D->GetDevice(), m_direct3D->GetCommandList(), XMFLOAT3(0.0f, 1.0f, 5.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)));
	m_models.push_back(new Cube(m_direct3D->GetDevice(), m_direct3D->GetCommandList(), XMFLOAT3(5.0f, 1.0f, 5.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)));

	//Pointer to the begining of the vector of models
	m_shaderManager->CreateDescriptor(m_models[0]->GetConstantBuffer());
	assert(&m_models);

	//Create the light object
	m_lights.push_back(new DirectionalLight(m_direct3D->GetDevice(), m_direct3D->GetCommandList(), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f)));

	//Pointer to the begining of the vector of lights
	m_shaderManager->CreateDescriptor(m_lights[0]->GetConstantBuffer());
	assert(&m_lights);


	m_shaderManager->CreateRootSignature(m_direct3D->GetDevice());
	m_shaderManager->CreatPipelineState({ { ShaderType::VS, L"Shader Files/ColorVertexShader.hlsl" },{ ShaderType::PS, L"Shader Files/ColorPixelShader.hlsl" } },
		m_direct3D->GetDevice(), SCREEN_WIDTH, SCREEN_HEIGHT, ShaderPipelineType::COLOR);

	//Close the command list now that all the commands have been recorded
	m_direct3D->CloseCommandList();

	//Execute the command list
	m_direct3D->ExecuteCommandList();
}

GoodaDriver::~GoodaDriver()
{
}

void GoodaDriver::Frame(Camera* camera)
{
	//Update constant buffer
	m_shaderManager->Frame(m_direct3D->GetCurrentFrame());

	for (int i = 0; i < m_models.size(); ++i)
		m_models[i]->Frame(m_direct3D->GetCurrentFrame(), camera->GetViewMatrix());

	for (int i = 0; i < m_lights.size(); ++i)
		m_lights[i]->Frame(m_direct3D->GetCurrentFrame());


	//Render the graphics scene
	Render();
}

void GoodaDriver::Render()
{
	//Reset the command list and put it in a recording state
	m_direct3D->BeginScene(m_shaderManager);

	UINT descriptorSize = m_direct3D->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	CD3DX12_GPU_DESCRIPTOR_HANDLE handle(m_shaderManager->GetDescriptorHeap(m_direct3D->GetCurrentFrame())->GetGPUDescriptorHandleForHeapStart());

	for (int i = 0; i < m_models.size(); ++i)
		m_models[i]->Render(m_direct3D->GetCommandList(), m_direct3D->GetCurrentFrame(), 0, handle);

	handle.Offset(descriptorSize);

	for (int i = 0; i < m_lights.size(); ++i)
		m_lights[i]->Render(m_direct3D->GetCommandList(), m_direct3D->GetCurrentFrame(), 0, handle);	

	//Close the command list and execute the commands
	m_direct3D->EndScene();
}
