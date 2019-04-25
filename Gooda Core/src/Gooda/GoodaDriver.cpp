#include "GoodaDriver.h"

namespace GoodaCore
{
	GoodaDriver* GoodaDriver::Instance()
	{
		static GoodaDriver s_goodaDriver;
		return &s_goodaDriver;
	}

	bool GoodaCore::GoodaDriver::Init()
	{
		//Initiate Direct3D
		Direct3D12::Instance()->Init();

		//Create a Window object used for rendering
		m_window = new Window();
		m_window->Init();

		//Initiate Input object
		Input::Instance()->Init(m_window->GetWindowHandle());

		//Create the Model object
		m_models.push_back(new Cube(XMFLOAT3(0.0f, 1.0f, 5.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)));
		m_models.push_back(new Cube(XMFLOAT3(5.0f, 1.0f, 5.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)));
		m_models.push_back(new Cube(XMFLOAT3(-5.0f, 1.0f, 5.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)));
		assert(&m_models);

		//Create the light object
		m_lights.push_back(new DirectionalLight(XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f), XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f)));
		assert(&m_lights);

		//Create the camera object
		m_camera = new Camera();
		assert(m_camera);

		//Initiate Shader Ojects
		ShaderManager::Instance()->Init();

		//Initiate Renderer to use Direct3D12 as backend
		Renderer::Instance()->Init(Backend::DX12);

		//Close the command list now that all the commands have been recorded
		Direct3D12::Instance()->CloseCommandList();

		//Execute the command list
		Direct3D12::Instance()->ExecuteCommandList();

		return true;
	}

	bool GoodaDriver::Frame()
	{
		Direct3D12::Instance()->BeginFrame();

		//Update the mouse and keyboard
		if (!Input::Instance()->Frame())
			return false;

		m_window->Frame();

		ShaderManager::Instance()->Frame();

		for (auto model : m_models)
			model->Frame();

		for (auto light : m_lights)
			light->Frame();

		m_camera->Frame();

		Renderer::Instance()->Render(m_models);

		m_window->Present();

		Direct3D12::Instance()->EndFrame();

		return true;
	}

	bool GoodaCore::GoodaDriver::Destroy()
	{
		Direct3D12::Instance()->FlushCommandQueue();

		//Release Gooda objects
		m_camera->Release();

		for (auto model : m_models)
			model->Release();

		for (auto light : m_lights)
			light->Release();

		m_window->Destroy();
		m_window->Release();

		return true;
	}
}