#include "Window.h"
#include "Input.h"

namespace GoodaCore
{
	bool Window::BeginScene()
	{
		UINT frameIndex = m_swapChain->GetCurrentBackBufferIndex();

		//Record commands in the command list now.
		//Start by setting the resource barrier.
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Transition.pResource = m_resources.m_backBufferRenderTargetView[frameIndex];
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

		Direct3D12::Instance()->GetCommandList()->ResourceBarrier(1, &barrier);

		//Set the back buffer as the render target.
		D3D12_CPU_DESCRIPTOR_HANDLE renderTargetViewHandle = m_resources.m_renderTargetViewDescHeap->GetCPUDescriptorHandleForHeapStart();
		renderTargetViewHandle.ptr += Direct3D12::Instance()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV) * (UINT64)frameIndex;

		CD3DX12_CPU_DESCRIPTOR_HANDLE depthStencilHandle(m_resources.m_depthStencilDescHeap->GetCPUDescriptorHandleForHeapStart());

		Direct3D12::Instance()->GetCommandList()->OMSetRenderTargets(1, &renderTargetViewHandle, FALSE, &depthStencilHandle);

		//Then set the color to clear the window to.
		float color[4] = { 0.5f, 0.5f, 0.5f, 1.0f };

		Direct3D12::Instance()->GetCommandList()->RSSetViewports(1, &m_viewport);
		Direct3D12::Instance()->GetCommandList()->RSSetScissorRects(1, &m_rect);

		Direct3D12::Instance()->GetCommandList()->ClearRenderTargetView(renderTargetViewHandle, color, 1, &m_rect);
		Direct3D12::Instance()->GetCommandList()->ClearDepthStencilView(depthStencilHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 1, &m_rect);

		return true;
	}

	Window* Window::Instance()
	{
		static Window s_window;
		return &s_window;
	}

	bool Window::Init()
	{
		UINT screenHeight, screenWidth;
		FLOAT screenNear, screenDepth;

		//Set the width and height of the screen to zero
		screenHeight = 0;
		screenWidth = 0;
		screenNear = m_screenNear;
		screenDepth = m_screenDepth;

		DEVMODE dmScreenSettings;
		int posX, posY;

		//Setup the windows class with the default settings
		ZeroMemory(&m_wc, sizeof(WNDCLASSEX));
		m_wc.style = CS_HREDRAW | CS_VREDRAW;
		m_wc.lpfnWndProc = &WindowProc;
		m_wc.cbClsExtra = 0;
		m_wc.cbWndExtra = 0;
		m_wc.hInstance = m_hinstance;
		m_wc.hIcon = LoadIcon(m_hinstance, IDI_APPLICATION);
		m_wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		m_wc.hbrBackground = (HBRUSH)COLOR_WINDOW;;
		m_wc.lpszClassName = L"Gooda Engine";
		m_wc.cbSize = sizeof(WNDCLASSEX);

		//Register the window class
		RegisterClassEx(&m_wc);

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
		m_windowHandle = CreateWindowEx(NULL,
			m_wc.lpszClassName,
			L"Game Window",
			WS_OVERLAPPEDWINDOW,
			posX,
			posY,
			screenWidth,
			screenHeight,
			NULL,
			NULL,
			m_hinstance,
			NULL);

		//Bring the window up on the screen and set it as main focus
		ShowWindow(m_windowHandle, SW_SHOW);

		//Hide the mouse cursor
		ShowCursor(false);

		if (!Direct3D12::Instance()->CreateSwapChainForHWND(screenWidth, screenHeight, m_windowHandle, &m_swapChain))
			return false;

		if (!Direct3D12::Instance()->CreateBackBufferRenderTarget(m_swapChain, &m_resources.m_backBufferRenderTargetView[0], &m_resources.m_renderTargetViewDescHeap, m_windowHandle))
			return false;

		if (!Direct3D12::Instance()->CreateDepthStencil(&m_resources.m_depthStencilBuffer, &m_resources.m_depthStencilDescHeap))
			return false;

		CreateViewPortAndScissorRect(screenWidth, screenHeight, screenNear, screenDepth);

		return true;
	}

	bool Window::Destroy()
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
		UnregisterClassA((LPCSTR)m_wc.lpszClassName, m_hinstance);

		m_hinstance = NULL;

		//Before shutting down set to windowed mode or when you release the swap chain it will throw an exception
		if (m_swapChain)
			if (FAILED(m_swapChain->SetFullscreenState(false, nullptr)))
				return false;

		m_resources.m_depthStencilBuffer->Release();
		m_resources.m_depthStencilDescHeap->Release();
		m_resources.m_renderTargetViewDescHeap->Release();

		for (int i = 0; i < frameBufferCount; i++)
			m_resources.m_backBufferRenderTargetView[i]->Release();

		m_swapChain->Release();

		return true;
	}

	bool Window::CreateViewPortAndScissorRect(UINT screenWidth, UINT screenHeight, FLOAT screenNear, FLOAT screenFar)
	{
		//Fill out the Viewport
		m_viewport.TopLeftX = 0;
		m_viewport.TopLeftY = 0;
		m_viewport.Width = (float)screenWidth;
		m_viewport.Height = (float)screenHeight;
		m_viewport.MinDepth = 0.0f;
		m_viewport.MaxDepth = 1.0f;

		//Fill out a scissor rect
		m_rect.left = 0;
		m_rect.top = 0;
		m_rect.right = screenWidth;
		m_rect.bottom = screenHeight;

		return true;
	}

	UINT Window::GetCurrentFrameIndex()
	{
		return m_swapChain->GetCurrentBackBufferIndex();
	}

	HWND Window::GetWindowHandle()
	{
		return m_windowHandle;
	}

	bool Window::EndScene()
	{
		UINT frameIndex = m_swapChain->GetCurrentBackBufferIndex();

		//Indicate that the back buffer will now be used to present.
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Transition.pResource = m_resources.m_backBufferRenderTargetView[frameIndex];
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		Direct3D12::Instance()->GetCommandList()->ResourceBarrier(1, &barrier);

		//Close and Send the command list to the GPU
		Direct3D12::Instance()->CloseCommandList(frameIndex);
		Direct3D12::Instance()->ExecuteCommandList(m_swapChain->GetCurrentBackBufferIndex());

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
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
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

	case WM_STYLECHANGED:
	{
		ShowWindow(hwnd, SW_SHOW);
	}

	//Check if a key has been pressed on the keyboard
	case WM_KEYDOWN:
	{
		//If a key is pressed send it to input object so it can record that state
		GoodaCore::Input::Instance()->ProcessKeyboardMessage(umessage, wparam, lparam);

		break;
	}

	//Check if a key has been released on the keyboard
	case WM_KEYUP:
	{
		//If a key has been released then send it to the input object so it can unset the state for that key
		GoodaCore::Input::Instance()->ProcessKeyboardMessage(umessage, wparam, lparam);
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
		//If mouse was moved send to input object to update the mouse
		GoodaCore::Input::Instance()->ProcessMouseMessage(umessage, wparam, lparam);
		break;
	}

	//Any other message send to the default message handler as our application won't use them
	default:
	{
		return DefWindowProc(hwnd, umessage, wparam, lparam);
	}
	}

	return 0;
}