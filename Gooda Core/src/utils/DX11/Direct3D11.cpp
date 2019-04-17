#include "Direct3D11.h"

namespace GoodaCore
{
	Direct3D11::Direct3D11()
	{
		CreateDirect3DDeviceAndSwapChain();

		CreateRenderTargets();

		CreateViewPortAndScissorRect();
	}

	Direct3D11::~Direct3D11()
	{
		//Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
		if (m_swapChain)
			m_swapChain->SetFullscreenState(false, nullptr);
	}

	Direct3D11* Direct3D11::Instance()
	{
		Direct3D11 s_direct3D11;
		return &s_direct3D11;
	}

	void Direct3D11::BeginScene()
	{
		//Swap the current render target view buffer index so drawing is don on the correct buffer
		m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

		//Setup the color to clear the buffer to
		float color[4] = { 0.5f, 0.5f, 0.5f, 1.0f };

		//Clear the back buffer
		m_deviceContext->ClearRenderTargetView(m_renderTargetView.Get(), color);

		//Clear the depth buffer
		m_deviceContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	void Direct3D11::EndScene()
	{
		//Present the back buffer to the screen since rendering is complete
		if (m_vsync)
		{
			m_swapChain->Present(1, 0);
		}
		else
		{
			m_swapChain->Present(0, 0);
		}
	}

	ID3D11Device* Direct3D11::GetDevice()
	{
		return m_device.Get();
	}

	ID3D11DeviceContext* Direct3D11::GetDeviceContext()
	{
		return m_deviceContext.Get();
	}

	int Direct3D11::GetCurrentFrame()
	{
		return m_frameIndex;
	}

	void Direct3D11::CreateDirect3DDeviceAndSwapChain()
	{
		//Create a DIrect3D graphics interface factory
		ComPtr<IDXGIFactory5> factory = nullptr;
		ComPtr<IDXGIAdapter1> adapter = nullptr;

		CreateDXGIFactory(__uuidof(IDXGIFactory5), (void**)factory.GetAddressOf());

		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_1;

		//Set device flags
		UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if _DEBUG
		// If the project is in a debug build, enable the debug layer.
		creationFlags |= D3D11_CREATE_DEVICE_DEBUG;

#endif // _DEBUG

		for (UINT adapterIndex = 0;; adapterIndex++)
		{
			adapter = nullptr;
			if (DXGI_ERROR_NOT_FOUND == factory->EnumAdapters1(adapterIndex, adapter.GetAddressOf()))
				break;

			//Check to see if the adapter supports Direct3D 12, but don't create the device yet
			if (SUCCEEDED(D3D11CreateDevice(adapter.Get(), D3D_DRIVER_TYPE_UNKNOWN, NULL, creationFlags, &featureLevel, 1, D3D11_SDK_VERSION, nullptr, nullptr, nullptr)))
				break;
		}

		if (adapter)
		{
			//Create the device
			assert(!D3D11CreateDevice(adapter.Get(), D3D_DRIVER_TYPE_UNKNOWN, NULL, creationFlags, &featureLevel, 1, D3D11_SDK_VERSION, m_device.GetAddressOf(), nullptr, m_deviceContext.GetAddressOf()));
		}
		else
		{
			//Create warp device if no adapter was found
			factory->EnumWarpAdapter(__uuidof(IDXGIAdapter1), (void**)& adapter);
			featureLevel = D3D_FEATURE_LEVEL_10_0;
			assert(!D3D11CreateDevice(adapter.Get(), D3D_DRIVER_TYPE_UNKNOWN, NULL, creationFlags, &featureLevel, 1, D3D11_SDK_VERSION, m_device.GetAddressOf(), nullptr, m_deviceContext.GetAddressOf()));
		}

		//Enumerate the primary monitor
		ComPtr<IDXGIOutput> adapterOutput = nullptr;
		assert(!adapter->EnumOutputs(0, adapterOutput.GetAddressOf()));

		//Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
		unsigned int numModes = 0;
		assert(!adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, nullptr));

		//Create a list to hold all the possible display modes for this monitor/video card combination.
		DXGI_MODE_DESC* displayModeList = new DXGI_MODE_DESC[numModes];

		//Now fill the display mode list structures.
		assert(!adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList));

		//Now go through all the display modes and find the one that matches the screen width and height.
		//When a match is found store the numerator and denominator of the refresh rate for that monitor.
		unsigned int numerator, denominator;
		for (unsigned int i = 0; i < numModes; i++)
		{
			if (displayModeList[i].Width == m_screenWidth)
			{
				if (displayModeList[i].Height == m_screenHeight)
				{
					numerator = displayModeList[i].RefreshRate.Numerator;
					denominator = displayModeList[i].RefreshRate.Denominator;
				}
			}
		}

		//Release the display mode list.
		if (displayModeList)
		{
			delete[] displayModeList;
			displayModeList = nullptr;
		}

		//Get the adapter (video card) description.
		DXGI_ADAPTER_DESC adapterDesc = {};
		assert(!adapter->GetDesc(&adapterDesc));

		//Store the dedicated video card memory in megabytes.
		unsigned long long videoCardMemory = 0;
		videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

		//Convert the name of the video card to a character array and store it.
		size_t stringLength;
		char videoCardDescription[128];
		wcstombs_s(&stringLength, videoCardDescription, 128, adapterDesc.Description, 128);

		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		ComPtr<IDXGISwapChain> swapChain = nullptr;

		//Initialize the swap chain description.
		ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

		//Set to a single back buffer
		swapChainDesc.BufferCount = frameBufferCount;

		//Set the width and height of the back buffer
		swapChainDesc.BufferDesc.Width = m_screenWidth;
		swapChainDesc.BufferDesc.Height = m_screenHeight;

		//Set regular 32-bit surface for the back buffer
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

		//Set the refresh rate of the back buffer
		if (m_vsync)
		{
			swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
			swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
		}
		else
		{
			swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
			swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		}

		//Set the usage of the back buffer
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

		//Discard the back buffer contents after presenting
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		//Set the handle for the window to render to
		swapChainDesc.OutputWindow = nullptr;

		//Turn multisampling off
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;

		//Set the scan line ordering and scaling to unspecified
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		//Don't set the advanced flags
		swapChainDesc.Flags = 0;

		//Set the full screen mode
		if (m_fullScreen)
			swapChainDesc.Windowed = false;
		else
			swapChainDesc.Windowed = true;

		//Finally create the swap chain using the swap chain description.	
		assert(!factory->CreateSwapChain(m_device.Get(), &swapChainDesc, swapChain.GetAddressOf()));

		//Next upgrade the IDXGISwapChain to a IDXGISwapChain3 interface and store it in a private member variable named m_swapChain.
		//This will allow us to use the newer functionality
		assert(!swapChain->QueryInterface(__uuidof(IDXGISwapChain3), (void**)m_swapChain.GetAddressOf()));
	}

	void Direct3D11::CreateViewPortAndScissorRect()
	{
		//Setup the viewport for rendering
		m_viewport.Width = (float)m_screenWidth;
		m_viewport.Height = (float)m_screenHeight;
		m_viewport.MinDepth = 0.0f;
		m_viewport.MaxDepth = 1.0f;
		m_viewport.TopLeftX = 0.0f;
		m_viewport.TopLeftY = 0.0f;

		//Create the viewport
		m_deviceContext->RSSetViewports(1, &m_viewport);

		//Setup the scissor rectangle for rendering
		m_rect.top = 0;
		m_rect.left = 0;
		m_rect.bottom = m_screenHeight;
		m_rect.right = m_screenWidth;

		//Create the scissor rectangle
		m_deviceContext->RSSetScissorRects(1, &m_rect);
	}

	void Direct3D11::CreateRenderTargets()
	{
		//Get the pointer to the back buffer
		ComPtr<ID3D11Texture2D> backBufferPtr;
		assert(!m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)backBufferPtr.GetAddressOf()));

		//Create the render target view with the back buffer pointer
		assert(!m_device->CreateRenderTargetView(backBufferPtr.Get(), nullptr, m_renderTargetView.GetAddressOf()));

		//Set up the description of the depth buffer
		D3D11_TEXTURE2D_DESC depthBufferDesc;
		depthBufferDesc.Width = m_screenWidth;
		depthBufferDesc.Height = m_screenHeight;
		depthBufferDesc.MipLevels = 1;
		depthBufferDesc.ArraySize = 1;
		depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthBufferDesc.SampleDesc.Count = 1;
		depthBufferDesc.SampleDesc.Quality = 0;
		depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthBufferDesc.CPUAccessFlags = 0;
		depthBufferDesc.MiscFlags = 0;

		//Create the texture for the depth buffer using the filled out description
		assert(!m_device->CreateTexture2D(&depthBufferDesc, nullptr, m_depthStencilBuffer.GetAddressOf()));

		//Set up the description of the stencil state
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

		depthStencilDesc.StencilEnable = true;
		depthStencilDesc.StencilReadMask = 0xFF;
		depthStencilDesc.StencilWriteMask = 0xFF;

		//Stencil operations if pixel is front-facing
		depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		//Stencil operations if pixel is back-facing
		depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		//Create the depth stencil state
		assert(!m_device->CreateDepthStencilState(&depthStencilDesc, m_depthStencilState.GetAddressOf()));

		//Set the depth stencil state
		m_deviceContext->OMSetDepthStencilState(m_depthStencilState.Get(), 1);

		//Set up the depth stencil view description
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;
		depthStencilViewDesc.Flags = 0;

		assert(!m_device->CreateDepthStencilView(m_depthStencilBuffer.Get(), &depthStencilViewDesc, m_depthStencilView.GetAddressOf()));

		//Bind the render target view and depth stencil buffer to the output render pipeline
		m_deviceContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());

		//Setup the raster description which will determine how and what polygons will be drawn
		D3D11_RASTERIZER_DESC rasterDesc;
		rasterDesc.AntialiasedLineEnable = false;
		rasterDesc.CullMode = D3D11_CULL_BACK;
		rasterDesc.DepthBias = 0;
		rasterDesc.DepthBiasClamp = 0.0f;
		rasterDesc.DepthClipEnable = true;
		rasterDesc.FillMode = D3D11_FILL_SOLID;
		rasterDesc.FrontCounterClockwise = false;
		rasterDesc.MultisampleEnable = false;
		rasterDesc.ScissorEnable = true;
		rasterDesc.SlopeScaledDepthBias = 0.0f;

		assert(!m_device->CreateRasterizerState(&rasterDesc, m_rasterState.GetAddressOf()));

		//Now set the rasterizer state
		m_deviceContext->RSSetState(m_rasterState.Get());
	}
}