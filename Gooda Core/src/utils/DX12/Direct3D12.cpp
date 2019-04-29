#include "Direct3D12.h"

namespace GoodaCore
{
	Direct3D12::Direct3D12()
	{
		
	}

	bool Direct3D12::CreateFenceAndEventHandle()
	{
		for (int i = 0; i < frameBufferCount; i++)
		{
			//Create a fence for GPU synchronization.
			if (FAILED(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)& m_fence[i])))
				return false;

			m_fenceValue[i] = 0;
		}

		//Create an event object for the fence.
		m_fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

		return true;
	}

	bool Direct3D12::CreateCommandInterface()
	{
		D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};

		//Initialize the description of the command queue.
		ZeroMemory(&commandQueueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));

		//Set up the description of the command queue.
		commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		commandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		commandQueueDesc.NodeMask = 0;

		//Create the command queue.
		if (FAILED(m_device->CreateCommandQueue(&commandQueueDesc, __uuidof(ID3D12CommandQueue), (void**)m_commandQueue.GetAddressOf())))
			return false;

		for (int i = 0; i < frameBufferCount; ++i)
		{
			//Create a command allocator.
			if (FAILED(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**)m_commandAllocator[i].GetAddressOf())))
				return false;
		}

		//Create a basic command list.
		if (FAILED(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator[0].Get(), NULL, __uuidof(ID3D12GraphicsCommandList), (void**)& m_commandList)))
			return false;

		//Set inital frame index to zero
		m_frameIndex = 0;

		return true;
	}

	bool Direct3D12::CreateSwapChainForHWND(UINT screenWidth, UINT screenHeight, HWND hwnd, IDXGISwapChain3** swapChain3)
	{
		//Enumerate the primary monitor
		IDXGIOutput* output = nullptr;
		if (FAILED(m_adapter->EnumOutputs(0, &output)))
			return false;

		//Get the number of modes
		unsigned int numModes = 0;

		if (FAILED(output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, nullptr)))
			return false;

		//Create and hold all the possible display modes for this monitor/graphcis card combination
		DXGI_MODE_DESC* displayModeList = new DXGI_MODE_DESC[numModes];

		//Now fill thr display mode list struct
		if (FAILED(output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList)))
			return false;

		//Now go through all the display modes and find one that matches the screen height and width
		//When a match is found  store the numerator and denominator of the refresh rate for thr monitor
		unsigned int numerator = 0, denominator = 0;
		for (unsigned int i = 0; i < numModes; i++)
		{
			if (displayModeList[i].Height == screenHeight)
			{
				if (displayModeList[i].Width == screenWidth)
				{
					numerator = displayModeList[i].RefreshRate.Numerator;
					denominator = displayModeList[i].RefreshRate.Denominator;
				}
			}
		}

		if (displayModeList)
		{
			delete[] displayModeList;
			displayModeList = nullptr;
		}

		//Get the adapter desc
		DXGI_ADAPTER_DESC adapterDesc = {};
		ZeroMemory(&adapterDesc, sizeof(DXGI_ADAPTER_DESC));
		if (FAILED(m_adapter->GetDesc(&adapterDesc)))
			return false;

		//Store the dedicated video card memory in megabytes
		unsigned long long videoCardMemory = 0;
		videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

		//Convert the name of the video card to a char array to store it
		size_t stringLength = 0;
		char videoCardDescription[128];
		wcstombs_s(&stringLength, videoCardDescription, 128, adapterDesc.Description, 128);
	
		//Initialize the swap chain description.
		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

		//Set the swap chain to use double buffering.
		swapChainDesc.BufferCount = frameBufferCount;

		//Set the height and width of the back buffers in the swap chain.
		swapChainDesc.BufferDesc.Height = m_screenHeight;
		swapChainDesc.BufferDesc.Width = m_screenWidth;

		//Set a regular 32-bit surface for the back buffers.
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

		//Set the usage of the back buffers to be render target outputs.
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

		//Set the swap effect to discard the previous buffer contents after swapping.
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

		//Set the handle for the window to render to.
		swapChainDesc.OutputWindow = hwnd;

		//Turn multisampling off.
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;

		// Set the scan line ordering and scaling to unspecified.
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		//Don't set the advanced flags.
		swapChainDesc.Flags = 0;

		//Set to full screen or windowed mode.
		if (m_fullScreen)
			swapChainDesc.Windowed = false;
		else
			swapChainDesc.Windowed = true;

		//Finally create the swap chain using the swap chain description.	
		IDXGIFactory5* factory = nullptr;
		m_adapter->GetParent(__uuidof(IDXGIFactory5), (void**)& factory);

		IDXGISwapChain* swapChain = nullptr;
		if (FAILED(factory->CreateSwapChain(m_commandQueue.Get(), &swapChainDesc, &swapChain)))
			return false;

		//Next upgrade the IDXGISwapChain to a IDXGISwapChain3 interface and store it in a private member variable named m_swapChain.
		//This will allow us to use the newer functionality such as getting the current back buffer index.
		if (FAILED(swapChain->QueryInterface(__uuidof(IDXGISwapChain3), (void**)swapChain3)))
			return false;

		return true;
	}

	bool Direct3D12::CreateRenderTargets(IDXGISwapChain3* swapChain3, ID3D12Resource** renderTargetView, ID3D12DescriptorHeap** descriptorHeap)
	{
		return true;
	}

	bool Direct3D12::CreateBackBufferRenderTarget(IDXGISwapChain3* swapChain3, ID3D12Resource** renderTargetView, ID3D12DescriptorHeap** descriptorHeap, HWND hwnd)
	{
		unsigned int renderTargetViewDescriptorSize;
		D3D12_DESCRIPTOR_HEAP_DESC renderTargetViewHeapDesc = {};
		D3D12_CPU_DESCRIPTOR_HANDLE renderTargetViewHandle = {};

		//Initialize the render target view heap description for the two back buffers.
		ZeroMemory(&renderTargetViewHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));

		//Set the number of descriptors to two for our two back buffers.  Also set the heap type to render target views.
		renderTargetViewHeapDesc.NumDescriptors = frameBufferCount;
		renderTargetViewHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		renderTargetViewHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		//Create the render target view heap for the back buffers.
		if (FAILED(m_device->CreateDescriptorHeap(&renderTargetViewHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**) & *descriptorHeap)))
			return false;

		//Get a handle to the starting memory location in the render target view heap to identify where the render target views will be located for the two back buffers.
		renderTargetViewHandle = descriptorHeap[0]->GetCPUDescriptorHandleForHeapStart();

		//Get the size of the memory location for the render target view descriptors.
		renderTargetViewDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		for (int i = 0; i < frameBufferCount; i++)
		{
			//Get a pointer to the back buffer from the swap chain.
			if (FAILED(swapChain3->GetBuffer(i, __uuidof(ID3D12Resource), (void**)& renderTargetView[i])))
				return false;

			//Create a render target view for the back buffer.
			m_device->CreateRenderTargetView(renderTargetView[i], NULL, renderTargetViewHandle);

			//Increment the render target view handle by the render target view desc size
			renderTargetViewHandle.ptr += renderTargetViewDescriptorSize;
		}

		return true;
	}

	bool Direct3D12::CreateDepthStencil(ID3D12Resource** depthStencilBuffer, ID3D12DescriptorHeap** depthStencilDescHeap)
	{
		//Create a depth stencil descriptor heap so we can get a pointer to the depth stencil buffer
		D3D12_DESCRIPTOR_HEAP_DESC depthStencilViewHeapDesc = {};
		depthStencilViewHeapDesc.NumDescriptors = 1;
		depthStencilViewHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		depthStencilViewHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		if (FAILED(m_device->CreateDescriptorHeap(&depthStencilViewHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)depthStencilDescHeap)))
			return false;

		//Fill out a depth stencil desc structure
		D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
		ZeroMemory(&depthStencilViewDesc, sizeof(D3D12_DEPTH_STENCIL_VIEW_DESC));

		depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
		depthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Flags = D3D12_DSV_FLAG_NONE;

		//Fil out a depth clear value sturcture
		D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
		ZeroMemory(&depthOptimizedClearValue, sizeof(D3D12_CLEAR_VALUE));

		depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
		depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
		depthOptimizedClearValue.DepthStencil.Stencil = 0;

		////Create a resource and the resource heap to store the resource 
		//D3D12_HEAP_PROPERTIES properties = {};
		//ZeroMemory(&properties, sizeof(properties));
		//properties.Type = D3D12_HEAP_TYPE_DEFAULT;

		//D3D12_RESOURCE_DESC resourceDesc = {};
		//ZeroMemory(&resourceDesc, sizeof(resourceDesc));
		//resourceDesc.Alignment = 0;
		//resourceDesc.DepthOrArraySize = 1;
		//resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
		//resourceDesc.Format = DXGI_FORMAT_D32_FLOAT;
		//resourceDesc.Height = m_screenHeight;
		//resourceDesc.Width = m_screenWidth;
		//resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		//resourceDesc.MipLevels = 0;
		//resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		//resourceDesc.SampleDesc.Count = 1;
		//resourceDesc.SampleDesc.Quality = 0;

		//if (FAILED(m_device->CreateCommittedResource(&properties, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE,
		//	&depthOptimizedClearValue, __uuidof(ID3D12Resource), (void**)depthStencilBuffer)))
		//	return false;

		m_device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, m_screenWidth, m_screenHeight, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
			D3D12_RESOURCE_STATE_DEPTH_WRITE,
			&depthOptimizedClearValue,
			IID_PPV_ARGS(depthStencilBuffer)
		);

		//Give the resource a name for debugging purposes
		depthStencilDescHeap[0]->SetName(L"Depth/Stencil Resource Heap");

		//Create the depth stencil view
		m_device->CreateDepthStencilView(depthStencilBuffer[0], &depthStencilViewDesc, depthStencilDescHeap[0]->GetCPUDescriptorHandleForHeapStart());

		return true;
	}

	bool Direct3D12::CloseCommandList()
	{
		if (FAILED(m_commandList->Close()))
			return false;

		return true;
	}

	bool Direct3D12::ResetCommandList()
	{
		m_frameIndex++;
		if (m_frameIndex > (frameBufferCount - 1))
			m_frameIndex = 0;

		if (FAILED(m_commandAllocator[m_frameIndex]->Reset()))
			return false;

		if (FAILED(m_commandList->Reset(m_commandAllocator[m_frameIndex].Get(), nullptr)))
			return false;

		return true;
	}

	bool Direct3D12::ExecuteCommandList()
	{
		ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };

		//Load the command list array (only one command list for now).
		ppCommandLists[0] = m_commandList.Get();

		//Execute the list of commands.
		m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

		//Signal and increment the fence value.
		m_fenceValue[m_frameIndex]++;
		if (FAILED(m_commandQueue->Signal(m_fence[m_frameIndex].Get(), m_fenceValue[m_frameIndex])))
			return false;

		return true;
	}

	bool Direct3D12::DeviceSynchronize()
	{
		//If the current fence value is still less than the "m_fenceValue", then GPU has not finished
		//the command queue since it has not reached the "m_commandQueue->Signal" command
		if (m_fence[m_frameIndex]->GetCompletedValue() < m_fenceValue[m_frameIndex])
		{
			if (FAILED(m_fence[m_frameIndex]->SetEventOnCompletion(m_fenceValue[m_frameIndex], m_fenceEvent)))
				return false;

			//Wait until the fence has triggered the correct fence event
			WaitForSingleObject(m_fenceEvent, INFINITE);
		}

		//Increment m_fenceValue for next frame
		m_fenceValue[m_frameIndex]++;

		return true;
	}

	bool Direct3D12::FlushCommandQueue()
	{
		for (UINT i = 0; i < frameBufferCount; i++)
		{
			//Signal and increment the fence value one last time to flush the command queue
			m_fenceValue[i]++;
			if (FAILED(m_commandQueue->Signal(m_fence[i].Get(), m_fenceValue[i])))
				return false;

			if (m_fence[i]->GetCompletedValue() < m_fenceValue[i])
			{
				if (FAILED(m_fence[i]->SetEventOnCompletion(m_fenceValue[i], m_fenceEvent)))
					return false;

				//Wait until the fence has triggered the event
				WaitForSingleObject(m_fenceEvent, INFINITE);
			}
		}
		return true;
	}

	Direct3D12* Direct3D12::Instance()
	{
		static Direct3D12 s_direct3D12;
		return &s_direct3D12;
	}

	bool Direct3D12::Init()
	{
		CreateDirect3DDevice();
		CreateCommandInterface();
		CreateFenceAndEventHandle();

		m_frameIndex = 0;

		return true;
	}

	bool Direct3D12::Destroy()
	{
		if (m_dxgiDebug)
			m_dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);

		return true;
	}

	bool Direct3D12::BeginFrame()
	{
		DeviceSynchronize();
		ResetCommandList();

		return true;
	}

	bool Direct3D12::EndFrame()
	{
		switch (m_frameIndex)
		{
		case 0:
			m_frameIndex++;
			break;

		case 1:
			m_frameIndex++;
			break;

		case frameBufferCount - 1:
			m_frameIndex = 0;
			break;

		default:
			break;
		}

		return true;
	}

	ID3D12Device* Direct3D12::GetDevice()
	{
		return m_device.Get();
	}

	ID3D12GraphicsCommandList* Direct3D12::GetCommandList()
	{
		return m_commandList.Get();
	}

	UINT Direct3D12::GetCurrentFrame()
	{
		return m_frameIndex;
	}

	bool Direct3D12::CreateDirect3DDevice()
	{

#ifdef _DEBUG
		//Get the interface to DirectX 12 debugger
		ID3D12Debug* debugController = nullptr;
		ID3D12Debug1* debugController1 = nullptr;
		if (SUCCEEDED(D3D12GetDebugInterface(__uuidof(ID3D12Debug), (void**)&debugController)))
		{
			debugController->QueryInterface(__uuidof(ID3D12Debug1), (void**)&debugController1);
			debugController1->EnableDebugLayer();
			debugController1->SetEnableGPUBasedValidation(TRUE);

			typedef HRESULT(WINAPI * LPDXGIGETDEBUGINTERFACE)(REFIID, void**);
			HMODULE dxgidebug = LoadLibraryEx(L"DXGIDEBUG.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
			if (dxgidebug)
			{
				LPDXGIGETDEBUGINTERFACE DxgiGetDebugInterface = (LPDXGIGETDEBUGINTERFACE)GetProcAddress(dxgidebug, "DXGIGetDebugInterface");
				HRESULT hr = DxgiGetDebugInterface(__uuidof(IDXGIDebug), (void**)m_dxgiDebug.GetAddressOf());
				if (FAILED(hr))
					return false;
			}
			debugController->Release();
			debugController1->Release();
		}
#endif

		//Create temporary factory to use when creating Device
		IDXGIFactory5* factory = nullptr;
		CreateDXGIFactory(__uuidof(IDXGIFactory5), (void**)& factory);

		//Use the factory to create an adapter for the primary graphics card
		IDXGIAdapter1* adapter = nullptr;
		for (UINT adapterIndex = 0;; adapterIndex++)
		{
			adapter = nullptr;
			if (DXGI_ERROR_NOT_FOUND == factory->EnumAdapters1(adapterIndex, &adapter))
				break;

			//Check to see if the adapter supports Direct3D 12, but don't create the device yet
			if (SUCCEEDED(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_1, __uuidof(ID3D12Device), nullptr)))
				break;
		}

		if (adapter)
		{
			//Create the device
			if (FAILED(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_1, __uuidof(ID3D12Device), (void**)m_device.GetAddressOf())))
				return false;
		}
		else
		{
			//Create warp device if no adapter was found
			factory->EnumWarpAdapter(__uuidof(IDXGIAdapter1), (void**)&adapter);
			if (FAILED(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), (void**)m_device.GetAddressOf())))
				return false;
		}

		adapter->QueryInterface(__uuidof(IDXGIAdapter3), (void**)m_adapter.GetAddressOf());
		adapter->Release();

		return true;
	}
}