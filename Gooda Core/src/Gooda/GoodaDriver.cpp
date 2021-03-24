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

		//Create a Window object used for rendering
		if (!Window::Instance()->Init())
			return false;

		UINT currentFrame = Window::Instance()->GetCurrentFrameIndex();

		//Initiate Input object
		if (!Input::Instance()->Init(Window::Instance()->GetWindowHandle()))
			return false;

		//Create the Model object
		m_models.push_back(new Cube(XMFLOAT3(0.0f, 0.0f, 5.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)));
		m_models.push_back(new Cube(XMFLOAT3(5.0f, 0.0f, 5.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)));
		m_models.push_back(new Cube(XMFLOAT3(-5.0f, 0.0f, 5.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)));
		m_models.push_back(new Plane(20, 20, XMFLOAT3(0.0f, -0.5f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)));

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
		if (!Direct3D12::Instance()->CloseCommandList(currentFrame))
			return false;

		//Execute the command list
		if (!Direct3D12::Instance()->ExecuteCommandList(currentFrame))
			return false;

		return true;
	}

	bool GoodaDriver::Frame()
	{
		UINT currentFrame = Window::Instance()->GetCurrentFrameIndex();

		if (!Direct3D12::Instance()->BeginFrame(currentFrame))
			return false;

		Console::Instance()->Frame();

		if (!Input::Instance()->Frame())
			return false;

		if (!Window::Instance()->BeginScene())
			return false;

		if (!ShaderManager::Instance()->Frame(currentFrame))
			return false;

		for (auto model : m_models)
			if (!model->Frame(currentFrame))
				return false;

		for (auto light : m_lights)
			if (!light->Frame(currentFrame))
				return false;

		if (!m_camera->Frame(currentFrame))
			return false;

		if (!Renderer::Instance()->Render(m_models))
			return false;

		if (!Window::Instance()->EndScene())
			return false;

		if (!Direct3D12::Instance()->EndFrame(currentFrame))
			return false;

		return true;
	}

	bool GoodaDriver::Run()
	{
		//Initialize the message structure
		MSG msg;
		ZeroMemory(&msg, sizeof(MSG));

		//Loop until there is a quit message from the window or the user
		bool done = false;
		while (!done)
		{
			//Handle the application messages
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			switch (msg.message)
			{
			case WM_QUIT:
				done = true;
				break;

			default:
				if (!Frame())
				{
					done = true;
				}
				break;
			}
		}

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

		Renderer::Instance()->Destroy();
		ShaderManager::Instance()->Destroy();
		Input::Instance()->Destroy();
		Window::Instance()->Destroy();
		Direct3D12::Instance()->Destroy();

		return true;
	}
}