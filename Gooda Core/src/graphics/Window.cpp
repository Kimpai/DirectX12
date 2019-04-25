#include "Window.h"

namespace GoodaCore
{
	Window::Window()
	{
		m_name = "Game Window";

		UINT screenHeight, screenWidth;
		FLOAT screenNear, screenDepth;

		//Set the width and height of the screen to zero
		screenHeight = 0;
		screenWidth = 0;
		screenNear = m_screenNear;
		screenDepth = m_screenDepth;

		WNDCLASSEX wc;
		DEVMODE dmScreenSettings;
		int posX, posY;

		//Setup the windows class with the default settings
		wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wc.lpfnWndProc = WndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = m_hinstance;
		wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		wc.hIconSm = wc.hIcon;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wc.lpszMenuName = NULL;
		wc.lpszClassName = m_name;
		wc.cbSize = sizeof(WNDCLASSEX);

		//Register the window class
		RegisterClassEx(&wc);

		//Determine the resolution of the clients desktop screen
		screenHeight = GetSystemMetrics(SM_CYSCREEN);
		screenWidth = GetSystemMetrics(SM_CXSCREEN);

		//Setup the screen settings depending on whether it is running in full screen or in window mode
		if (m_fullScreen)
		{
			//If full screen set the screen maximum size of the users desktop and 32bit
			memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
			dmScreenSettings.dmSize = sizeof(dmScreenSettings);
			dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
			dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
			dmScreenSettings.dmBitsPerPel = 32;
			dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

			//Change the display settings to full screen
			ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

			//Set the position of window to the top left corner
			posX = posY = 0;
		}
		else
		{
			//If windowed then set it 800x600 resolution
			screenWidth = m_screenWidth;
			screenHeight = m_screenHeight;

			//Place the window in the middle of the screen
			posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
			posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
		}

		//Create the window with the screen settings and get the handle to it
		m_windowHandle = CreateWindowEx(WS_EX_APPWINDOW, m_name, m_name,
			WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP, posX, posY, screenWidth,
			screenHeight, NULL, NULL, m_hinstance, NULL);

		//Bring the window up on the screen and set it as main focus
		ShowWindow(m_windowHandle, SW_SHOW);
		SetForegroundWindow(m_windowHandle);
		SetFocus(m_windowHandle);

		//Hide the mouse cursor
		ShowCursor(false);

		Direct3D12::Instance()->CreateSwapChainForHWND(screenWidth, screenHeight, m_windowHandle, m_swapChain.GetAddressOf());

		Direct3D12::Instance()->CreateBackBufferRenderTarget(m_swapChain.Get(), m_resources.m_backBufferRenderTargetView[0].GetAddressOf(), m_resources.m_renderTargetViewDescHeap.GetAddressOf(), m_windowHandle);

		//Fill out a depth stencil desc structure
		D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
		ZeroMemory(&depthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));

		depthStencilDesc.DepthEnable = TRUE;
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		depthStencilDesc.StencilEnable = FALSE;
		depthStencilDesc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
		depthStencilDesc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;

		// Fill out a stencil operation structure
		D3D12_DEPTH_STENCILOP_DESC depthStencilOPDesc = {};
		ZeroMemory(&depthStencilOPDesc, sizeof(D3D12_DEPTH_STENCILOP_DESC));

		depthStencilOPDesc.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		depthStencilOPDesc.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		depthStencilOPDesc.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		depthStencilOPDesc.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

		depthStencilDesc.FrontFace = depthStencilOPDesc;
		depthStencilDesc.BackFace = depthStencilOPDesc;

		Direct3D12::Instance()->CreateDepthStencil(depthStencilDesc, m_resources.m_depthStencilBuffer.GetAddressOf(), m_resources.m_depthStencilDescHeap.GetAddressOf());

		CreateViewPortAndScissorRect(screenWidth, screenHeight, screenNear, screenDepth);
	}

	Window::~Window()
	{
		//Show the mouse cursor
		ShowCursor(true);

		//Fix the display settings if leaving full screen
		if (m_fullScreen)
		{
			ChangeDisplaySettings(NULL, 0);
		}

		//Remove the window
		DestroyWindow(m_windowHandle);
		m_windowHandle = NULL;

		//Remove the application instance
		UnregisterClassA(m_name, m_hinstance);

		m_hinstance = NULL;

		//Before shutting down set to windowed mode or when you release the swap chain it will throw an exception
		if (m_swapChain)
			m_swapChain->SetFullscreenState(false, nullptr);
	}

	bool Window::Frame()
	{
		UINT frameIndex = m_swapChain->GetCurrentBackBufferIndex();

		//Record commands in the command list now.
		//Start by setting the resource barrier.
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Transition.pResource = m_resources.m_backBufferRenderTargetView[frameIndex].Get();
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

		Direct3D12::Instance()->GetCommandList()->ResourceBarrier(1, &barrier);

		//Set the back buffer as the render target.
		D3D12_CPU_DESCRIPTOR_HANDLE renderTargetViewHandle = m_resources.m_renderTargetViewDescHeap->GetCPUDescriptorHandleForHeapStart();
		renderTargetViewHandle.ptr += Direct3D12::Instance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV) * (UINT64)frameIndex;

		D3D12_CPU_DESCRIPTOR_HANDLE depthStencilHandle = m_resources.m_depthStencilDescHeap->GetCPUDescriptorHandleForHeapStart();

		Direct3D12::Instance()->GetCommandList()->OMSetRenderTargets(1, &renderTargetViewHandle, FALSE, &depthStencilHandle);

		//Then set the color to clear the window to.
		float color[4] = { 0.5f, 0.5f, 0.5f, 1.0f };

		Direct3D12::Instance()->GetCommandList()->RSSetViewports(1, &m_viewport);
		Direct3D12::Instance()->GetCommandList()->RSSetScissorRects(1, &m_rect);

		Direct3D12::Instance()->GetCommandList()->ClearRenderTargetView(renderTargetViewHandle, color, 1, &m_rect);
		Direct3D12::Instance()->GetCommandList()->ClearDepthStencilView(depthStencilHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 1, &m_rect);

		return true;
	}

	bool Window::CreateViewPortAndScissorRect(UINT screenWidth, UINT screenHeight, FLOAT screenNear, FLOAT screenFar)
	{
		//Fill out the Viewport
		m_viewport.TopLeftX = 0;
		m_viewport.TopLeftY = 0;
		m_viewport.Width = (float)screenWidth;
		m_viewport.Height = (float)screenHeight;
		m_viewport.MinDepth = screenNear;
		m_viewport.MaxDepth = screenFar;

		//Fill out a scissor rect
		m_rect.left = 0;
		m_rect.top = 0;
		m_rect.right = screenWidth;
		m_rect.bottom = screenHeight;

		return true;
	}

	HWND Window::GetWindowHandle()
	{
		return m_windowHandle;
	}

	bool Window::Present()
	{
		UINT frameIndex = m_swapChain->GetCurrentBackBufferIndex();

		//Indicate that the back buffer will now be used to present.
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Transition.pResource = m_resources.m_backBufferRenderTargetView[frameIndex].Get();
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		Direct3D12::Instance()->GetCommandList()->ResourceBarrier(1, &barrier);

		//Close and Send the command list to the GPU
		Direct3D12::Instance()->CloseCommandList();
		Direct3D12::Instance()->ExecuteCommandList();

		//Present the back buffer to the screen since rendering is complete.
		if (m_vsync)
		{
			//Lock to screen refresh rate.
			if (FAILED(m_swapChain->Present(1, 0)))
				return false;
		}
		else
		{
			//Present as fast as possible.
			if (FAILED(m_swapChain->Present(0, 0)))
				return false;
		}

		return true;
	}

	LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
	{
		switch (umessage)
		{
			//Check if the window is being destroyed
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			break;
		}

		//Check if the window is being closed
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			break;
		}

		case WM_ACTIVATEAPP:
		case WM_INPUT:
		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_MOUSEWHEEL:
		case WM_XBUTTONDOWN:
		case WM_XBUTTONUP:
		case WM_MOUSEHOVER:
		{
			Mouse::ProcessMessage(umessage, wparam, lparam);
			break;
		}

		//All other messages pass to the message handler
		default:
		{
			return Gooda::MessageHandler(hwnd, umessage, wparam, lparam);
		}
		}

		return 0;
	}
}