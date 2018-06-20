#include "GoodaDriver.h"

GoodaDriver::GoodaDriver()
{
	m_Direct3D = nullptr;
	m_Shader = nullptr;
	m_Sound = nullptr;
}

GoodaDriver::GoodaDriver(const GoodaDriver& other)
{

}

GoodaDriver::~GoodaDriver()
{
	
}

void GoodaDriver::Initialize(HWND hwnd, Camera* camera)
{
	//Create the Direct3D object
	m_Direct3D = new Direct3D(SCREEN_HEIGHT, SCREEN_WIDTH, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	assert(m_Direct3D);

	//Create the Shader manager object
	m_Shader = new ShaderManager();
	assert(&m_Shader);

	//Create the Sound manager object
	m_Sound = new SoundManager();
	assert(&m_Sound);

	//Create the Model object
	m_Models.push_back(new Cube(m_Direct3D->GetDevice(), m_Direct3D->GetCommandList(), XMFLOAT3(5.0f, 1.0f, 20.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)));
	m_Shader->AppendRootDescriptorToHeap(m_Models.back()->GetConstantBuffer());
	m_Models.push_back(new Cube(m_Direct3D->GetDevice(), m_Direct3D->GetCommandList(), XMFLOAT3(50.0f, 1.0f, 20.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)));
	m_Shader->AppendRootDescriptorToHeap(m_Models.back()->GetConstantBuffer());
	m_Models.push_back(new Cube(m_Direct3D->GetDevice(), m_Direct3D->GetCommandList(), XMFLOAT3(100.0f, 1.0f, 20.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)));
	m_Shader->AppendRootDescriptorToHeap(m_Models.back()->GetConstantBuffer());
	m_Models.push_back(new Terrain(m_Direct3D->GetDevice(), m_Direct3D->GetCommandList(), "Resource Files/heightmap.bmp", 257, 257, 12.0f, XMFLOAT3(0.0f, 0.0f, 0.0f)));
	m_Shader->AppendRootDescriptorToHeap(m_Models.back()->GetConstantBuffer());

	assert(&m_Models);

	//Create the light object
	m_Lights.push_back(new DirectionalLight(m_Direct3D->GetDevice(), m_Direct3D->GetCommandList(), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f)));
	m_Shader->AppendRootDescriptorToHeap(m_Lights.back()->GetConstantBuffer());
	assert(&m_Lights);


	m_Shader->CreateRootSignature(m_Direct3D->GetDevice());
	m_Shader->CreatPipelineState({ { ShaderType::VS, L"Shader Files/ColorVertexShader.hlsl" },{ ShaderType::PS, L"Shader Files/ColorPixelShader.hlsl" } }, 
		m_Direct3D->GetDevice(), SCREEN_WIDTH, SCREEN_HEIGHT, ShaderPipelineType::COLOR);

	//Close the command list now that all the commands have been recorded
	m_Direct3D->CloseCommandList();

	//Execute the command list
	m_Direct3D->ExecuteCommandList();
}

void GoodaDriver::Shutdown()
{
	//Release the Model object
	for (auto model : m_Models)
		if (model)
		{
			delete model;
			model = nullptr;
		}

	if (m_Sound)
	{
		delete m_Sound;
		m_Sound = nullptr;
	}

	//Release the Color Shader object
	if (m_Shader)
	{
		delete m_Shader;
		m_Shader = nullptr;
	}

	//Release the Direct3D object
	if (m_Direct3D)
	{
		delete m_Direct3D;
		m_Direct3D = nullptr;
	}

	for (auto light : m_Lights)
		if (light)
		{
			delete light;
			light = nullptr;
		}
}

void GoodaDriver::Frame(Camera* camera)
{
	//Update constant buffer
	m_Shader->Frame(m_Direct3D->GetCurrentFrame());

	for (auto model : m_Models)
		model->Frame(m_Direct3D->GetCurrentFrame(), camera->GetViewMatrix());

	for (auto light : m_Lights)
		light->Frame(m_Direct3D->GetCurrentFrame());


	//Render the graphics scene
	Render();
}

void GoodaDriver::Render()
{
	//Reset the command list and put it in a recording state
	m_Direct3D->BeginScene(m_Shader);

	for (int i = 0; i < m_Lights.size(); ++i)
	{
		CD3DX12_GPU_DESCRIPTOR_HANDLE handle(m_Shader->GetDescriptorHeap(m_Direct3D->GetCurrentFrame())->GetGPUDescriptorHandleForHeapStart(), (int)m_Models.size() + i, m_Direct3D->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
		m_Lights[i]->Render(m_Direct3D->GetCommandList(), m_Direct3D->GetCurrentFrame(), 0, handle);
	}

	for (int i = 0; i < m_Models.size(); ++i)
	{
		CD3DX12_GPU_DESCRIPTOR_HANDLE handle(m_Shader->GetDescriptorHeap(m_Direct3D->GetCurrentFrame())->GetGPUDescriptorHandleForHeapStart(), i, m_Direct3D->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
		m_Models[i]->Render(m_Direct3D->GetCommandList(), m_Direct3D->GetCurrentFrame(), 0, handle);
	}

	//Close the command list and execute the commands
	m_Direct3D->EndScene();
}
