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
		if (!Direct3D12::Instance()->Init())
			return false;

		//Initialize Console
		if (!Console::Instance()->Init())
			return false;

		//Create a Window object used for rendering
		m_window = new Window();
		if (!m_window)
			return false;

		//Initiate Input object
		if (!Input::Instance()->Init(m_window->GetWindowHandle()))
			return false;

		//Create the Model object
		m_models.push_back(new Cube(XMFLOAT3(0.0f, 1.0f, 5.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)));
		m_models.push_back(new Cube(XMFLOAT3(5.0f, 1.0f, 5.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)));
		m_models.push_back(new Cube(XMFLOAT3(-5.0f, 1.0f, 5.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)));

		//Create the light object
		m_lights.push_back(new DirectionalLight(XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f), XMFLOAT4(0.15f, 0.15f, 0.15f, 1.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f)));

		//Create the camera object
		m_camera = new Camera();
		if (!m_camera)
			return false;

		//Initiate Shader Ojects
		if (!ShaderManager::Instance()->Init())
			return false;

		//Initiate Renderer to use Direct3D12 as backend
		if (!Renderer::Instance()->Init(Backend::DX12))
			return false;

		//Close the command list now that all the commands have been recorded
		if (!Direct3D12::Instance()->CloseCommandList())
			return false;

		//Execute the command list
		if (!Direct3D12::Instance()->ExecuteCommandList())
			return false;

		return true;
	}

	bool GoodaDriver::Frame()
	{
		if (!Direct3D12::Instance()->BeginFrame())
			return false;

		//Update the mouse and keyboard
		if (!Input::Instance()->Frame())
			return false;

		if (!m_window->Frame())
			return false;

		if (!ShaderManager::Instance()->Frame())
			return false;

		for (auto model : m_models)
			if (!model->Frame())
				return false;

		for (auto light : m_lights)
			if (!light->Frame())
				return false;

		if (!m_camera->Frame())
			return false;

		if (!Renderer::Instance()->Render(m_models))
			return false;

		if (!m_window->Present())
			return false;

		if (!Direct3D12::Instance()->EndFrame())
			return false;

		return true;
	}

	bool GoodaCore::GoodaDriver::Destroy()
	{
		//Flush the command queue to make sure the GPU is not using any objects
		Direct3D12::Instance()->FlushCommandQueue();

		//Release Gooda objects
		m_camera->Release();

		for (auto model : m_models)
			model->Release();

		for (auto light : m_lights)
			light->Release();

		m_window->Release();

		Renderer::Instance()->Destroy();
		ShaderManager::Instance()->Destroy();
		Input::Instance()->Destroy();
		Console::Instance()->Destroy();
		Direct3D12::Instance()->Destroy();

		return true;
	}
}