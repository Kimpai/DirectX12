#include "d3dclass.h"

Direct3DClass::Direct3DClass()
{
	m_device = nullptr;
	m_commandQueue = nullptr;
	m_swapChain = nullptr;
	m_renderTargetViewDescHeap = nullptr;
	m_backBufferRenderTarget[0] = nullptr;
	m_backBufferRenderTarget[1] = nullptr;
	m_backBufferRenderTarget[2] = nullptr;
	m_commandAllocator[0] = nullptr;
	m_commandAllocator[1] = nullptr;
	m_commandAllocator[2] = nullptr;
	m_commandList = nullptr;
	m_fence[0] = nullptr;
	m_fence[1] = nullptr;
	m_fence[2] = nullptr;
	m_fenceEvent = nullptr;
}


Direct3DClass::Direct3DClass(const Direct3DClass& other)
{

}


Direct3DClass::~Direct3DClass()
{

}


bool Direct3DClass::Initialize(int screenHeight, int screenWidth, HWND hwnd, bool vsync, bool fullscreen, float screenDepth, float screenNear)
{
	D3D_FEATURE_LEVEL featureLevel;
	HRESULT result;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc;
	ComPtr<IDXGIFactory4> factory = nullptr;
	ComPtr<IDXGIAdapter> adapter = nullptr;
	ComPtr<IDXGIOutput> adapterOutput = nullptr;
	unsigned int numModes, numerator, denominator, renderTargetViewDescriptorSize;
	unsigned long long stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	IDXGISwapChain* swapChain = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC renderTargetViewHeapDesc;
	CD3DX12_CPU_DESCRIPTOR_HANDLE renderTargetViewHandle;


	//Store the vsync setting.
	m_vsync = vsync;

	//Set the feature level to DirectX 12.1 to enable using all the DirectX 12 features.
	//Note: Not all cards support full DirectX 12, this feature level may need to be reduced on some cards to 12.0.
	featureLevel = D3D_FEATURE_LEVEL_12_1;

#if _DEBUG
	//Get the interface to DirectX 12 debugger
	ComPtr<ID3D12Debug1> debugController = nullptr;
	result = D3D12GetDebugInterface(__uuidof(ID3D12Debug), (void**)&debugController);
	if (FAILED(result))
	{
		return false;
	}

	//Enable debug layer
	debugController->EnableDebugLayer();
	//debugController->SetEnableGPUBasedValidation(true);
	//debugController->SetEnableSynchronizedCommandQueueValidation(true);

	//Release the debug controller now that the debug layer has been enabled
	debugController = nullptr;
#endif

	//Create the Direct3D 12 device.
	result = D3D12CreateDevice(NULL, featureLevel, __uuidof(ID3D12Device), (void**)&m_device);
	if (FAILED(result))
	{
		MessageBox(hwnd, (LPCSTR)L"Could not create a DirectX 12 device.  The default video card does not support DirectX 12.", (LPCSTR)L"DirectX Device Failure", MB_OK);
		return false;
	}

	//Initialize the description of the command queue.
	ZeroMemory(&commandQueueDesc, sizeof(commandQueueDesc));

	//Set up the description of the command queue.
	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	commandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	commandQueueDesc.NodeMask = 0;

	//Create the command queue.
	result = m_device->CreateCommandQueue(&commandQueueDesc, __uuidof(ID3D12CommandQueue), (void**)&m_commandQueue);
	if (FAILED(result))
	{
		return false;
	}

	//Create a DirectX graphics interface factory.
	result = CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void**)&factory);
	if (FAILED(result))
	{
		return false;
	}

	//Use the factory to create an adapter for the primary graphics interface (video card).
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
	{
		return false;
	}

	//Enumerate the primary adapter output (monitor).
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
	{
		return false;
	}

	//Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// Create a list to hold all the possible display modes for this monitor/video card combination.
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		return false;
	}

	//Now fill the display mode list structures.
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result))
	{
		return false;
	}

	//Now go through all the display modes and find the one that matches the screen height and width.
	//When a match is found store the numerator and denominator of the refresh rate for that monitor.
	for (unsigned int i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Height == (unsigned int)screenHeight)
		{
			if (displayModeList[i].Width == (unsigned int)screenWidth)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	//Get the adapter (video card) description.
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result))
	{
		return false;
	}

	//Store the dedicated video card memory in megabytes.
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	//Convert the name of the video card to a character array and store it.
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		return false;
	}

	// Release the display mode list.
	delete[] displayModeList;
	displayModeList = nullptr;

	// Release the adapter output.
	adapterOutput = nullptr;

	// Release the adapter.
	adapter = nullptr;

	//Initialize the swap chain description.
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	//Set the swap chain to use double buffering.
	swapChainDesc.BufferCount = frameBufferCount;

	//Set the height and width of the back buffers in the swap chain.
	swapChainDesc.BufferDesc.Height = screenHeight;
	swapChainDesc.BufferDesc.Width = screenWidth;

	//Set a regular 32-bit surface for the back buffers.
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	//Set the usage of the back buffers to be render target outputs.
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	//Set the swap effect to discard the previous buffer contents after swapping.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	//Set the handle for the window to render to.
	swapChainDesc.OutputWindow = hwnd;

	//Set to full screen or windowed mode.
	if (fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	//Set the refresh rate of the back buffer.
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

	//Turn multisampling off.
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// Set the scan line ordering and scaling to unspecified.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//Don't set the advanced flags.
	swapChainDesc.Flags = 0;

	//Finally create the swap chain using the swap chain description.	
	result = factory->CreateSwapChain(m_commandQueue.Get(), &swapChainDesc, &swapChain);
	if (FAILED(result))
	{
		return false;
	}

	//Next upgrade the IDXGISwapChain to a IDXGISwapChain3 interface and store it in a private member variable named m_swapChain.
	//This will allow us to use the newer functionality such as getting the current back buffer index.
	result = swapChain->QueryInterface(__uuidof(IDXGISwapChain3), (void**)&m_swapChain);
	if (FAILED(result))
	{
		return false;
	}

	//Clear pointer to original swap chain interface since we are using version 3 instead (m_swapChain).
	swapChain = nullptr;

	//Release the factory now that the swap chain has been created.
	factory = nullptr;

	//Initialize the frameIndex to the current back buffer index
	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

	//Initialize the render target view heap description for the two back buffers.
	ZeroMemory(&renderTargetViewHeapDesc, sizeof(renderTargetViewHeapDesc));

	//Set the number of descriptors to two for our two back buffers.  Also set the heap type to render target views.
	renderTargetViewHeapDesc.NumDescriptors = frameBufferCount;
	renderTargetViewHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	renderTargetViewHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	//Create the render target view heap for the back buffers.
	result = m_device->CreateDescriptorHeap(&renderTargetViewHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_renderTargetViewDescHeap);
	if (FAILED(result))
	{
		return false;
	}

	//Get a handle to the starting memory location in the render target view heap to identify where the render target views will be located for the two back buffers.
	renderTargetViewHandle = m_renderTargetViewDescHeap->GetCPUDescriptorHandleForHeapStart();

	//Get the size of the memory location for the render target view descriptors.
	renderTargetViewDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	for (int i = 0; i < frameBufferCount; i++)
	{
		//Get a pointer to the first back buffer from the swap chain.
		result = m_swapChain->GetBuffer(i, __uuidof(ID3D12Resource), (void**)&m_backBufferRenderTarget[i]);
		if (FAILED(result))
		{
			return false;
		}

		//Create a render target view for the first back buffer.
		m_device->CreateRenderTargetView(m_backBufferRenderTarget[i].Get(), NULL, renderTargetViewHandle);

		//Increment the render target view handle by the render target view desc size
		renderTargetViewHandle.Offset(1, renderTargetViewDescriptorSize);
	}

	for (int i = 0; i < frameBufferCount; i++)
	{
		//Create a command allocator.
		result = m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**)&m_commandAllocator[i]);
		if (FAILED(result))
		{
			return false;
		}
	}

	//Create a basic command list.
	result = m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator[0].Get(), NULL, __uuidof(ID3D12GraphicsCommandList), (void**)&m_commandList);
	if (FAILED(result))
	{
		return false;
	}

	for (int i = 0; i < frameBufferCount; i++)
	{
		//Create a fence for GPU synchronization.
		result = m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&m_fence[i]);
		if (FAILED(result))
		{
			return false;
		}

		m_fenceValue[i] = 0;
	}

	//Create an event object for the fence.
	m_fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (m_fenceEvent == NULL)
	{
		return false;
	}

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


void Direct3DClass::Shutdown()
{
	//Wait of GPU to finish before releasing com objects
	for (int i = 0; i < frameBufferCount; i++)
	{
		m_frameIndex = i;
		WaitforFrameToFinish();
	}

	//Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
	if (m_swapChain)
	{
		m_swapChain->SetFullscreenState(false, NULL);
	}

	//Close the object handle to the fence event.
	CloseHandle(m_fenceEvent);

	for (int i = 0; i < frameBufferCount; i++)
	{
		//Release the fence.
		if (m_fence[i])
		{
			m_fence[i] = nullptr;
		}
	}

	//Release the command list.
	if (m_commandList)
	{
		m_commandList = nullptr;
	}

	for (int i = 0; i < frameBufferCount; i++)
	{
		//Release the command allocator.
		if (m_commandAllocator[i])
		{
			m_commandAllocator[i] = nullptr;
		}
	}

	//Release the back buffer render target views.
	if (m_backBufferRenderTarget[0])
	{
		m_backBufferRenderTarget[0] = nullptr;
	}
	if (m_backBufferRenderTarget[1])
	{
		m_backBufferRenderTarget[1] = nullptr;
	}
	if (m_backBufferRenderTarget[2])
	{
		m_backBufferRenderTarget[2] = nullptr;
	}

	//Release the render target view heap.
	if (m_renderTargetViewDescHeap)
	{
		m_renderTargetViewDescHeap = nullptr;
	}

	//Release the swap chain.
	if (m_swapChain)
	{
		m_swapChain = nullptr;
	}

	//Release the command queue.
	if (m_commandQueue)
	{
		m_commandQueue = nullptr;
	}

	//Release the device.
	if (m_device)
	{
		m_device = nullptr;
	}

	return;
}


bool Direct3DClass::BeginScene(ColorShaderClass* shader)
{
	HRESULT result;
	CD3DX12_CPU_DESCRIPTOR_HANDLE renderTargetViewHandle;
	float color[4];

	//Swap the current render target view buffer index so drawing is don on the correct buffer
	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

	//If the current fence value is still less than the "m_fenceValue", then GPU has not finished
	//the command queue since it has not reached the "m_commandQueue->Signal" command
	if (m_fence[m_frameIndex]->GetCompletedValue() < m_fenceValue[m_frameIndex])
	{
		result = m_fence[m_frameIndex]->SetEventOnCompletion(m_fenceValue[m_frameIndex], m_fenceEvent);
		if (FAILED(result))
		{
			return false;
		}

		//Wait until the fence has triggered the correct fence event
		WaitForSingleObject(m_fenceEvent, INFINITE);
	}

	//Increment m_fenceValue for next frame
	m_fenceValue[m_frameIndex]++;

	//Can only reset an allocator once the GPU is done with it.
	//Resetting it will free the memory that the command list was stored in
	result = m_commandAllocator[m_frameIndex]->Reset();
	if (FAILED(result))
	{
		return false;
	}

	//Reset the command list so that it can start recording again.
	//Only one command list can record at a given time.
	result = m_commandList->Reset(m_commandAllocator[m_frameIndex].Get(), shader->GetPipelineState());
	if (FAILED(result))
	{
		return false;
	}

	//Record commands in the command list now.
	//Start by setting the resource barrier.
	m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_backBufferRenderTarget[m_frameIndex].Get(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));

	//Get the render target view handle for the current back buffer.
	renderTargetViewHandle.InitOffsetted(m_renderTargetViewDescHeap->GetCPUDescriptorHandleForHeapStart(),
		m_frameIndex, m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));

	//Get the depth stencil view handle
	CD3DX12_CPU_DESCRIPTOR_HANDLE depthStencilViewHandle(shader->GetDepthStencilViewHandle());

	//Set the back buffer as the render target.
	m_commandList->OMSetRenderTargets(1, &renderTargetViewHandle, FALSE, &depthStencilViewHandle);

	//Then set the color to clear the window to.
	color[0] = 0.5;
	color[1] = 0.5;
	color[2] = 0.5;
	color[3] = 1.0;

	m_commandList->ClearRenderTargetView(renderTargetViewHandle, color, 0, NULL);
	m_commandList->ClearDepthStencilView(shader->GetDepthStencilViewHandle(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, NULL);
	m_commandList->SetGraphicsRootSignature(shader->GetRootSignature());
	m_commandList->RSSetViewports(1, &m_viewport);
	m_commandList->RSSetScissorRects(1, &m_rect);

	return true;
}

bool Direct3DClass::EndScene()
{
	HRESULT result;
	ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };

	//Indicate that the back buffer will now be used to present.
	m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_backBufferRenderTarget[m_frameIndex].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	//Close the list of commands.
	result = m_commandList->Close();
	if (FAILED(result))
	{
		return false;
	}

	//Load the command list array (only one command list for now).
	ppCommandLists[0] = m_commandList.Get();

	//Execute the list of commands.
	m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	//Signal and increment the fence value.
	result = m_commandQueue->Signal(m_fence[m_frameIndex].Get(), m_fenceValue[m_frameIndex]);
	if (FAILED(result))
	{
		return false;
	}

	//Finally present the back buffer to the screen since rendering is complete.
	if (m_vsync)
	{
		//Lock to screen refresh rate.
		result = m_swapChain->Present(1, 0);
		if (FAILED(result))
		{
			return false;
		}
	}
	else
	{
		//Present as fast as possible.
		result = m_swapChain->Present(0, 0);
		if (FAILED(result))
		{
			return false;
		}
	}

	return true;
}

bool Direct3DClass::CloseCommandList()
{
	HRESULT result;

	result = m_commandList->Close();
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

bool Direct3DClass::ResetCommandList(ID3D12PipelineState* pipelinestate)
{
	HRESULT result;

	result = m_commandList->Reset(m_commandAllocator[m_frameIndex].Get(), pipelinestate);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

bool Direct3DClass::ExecuteCommandList()
{
	HRESULT result;
	ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };

	//Load the command list array (only one command list for now).
	ppCommandLists[0] = m_commandList.Get();

	//Execute the list of commands.
	m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	//Signal and increment the fence value.
	m_fenceValue[m_frameIndex]++;
	result = m_commandQueue->Signal(m_fence[m_frameIndex].Get(), m_fenceValue[m_frameIndex]);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

ID3D12Device* Direct3DClass::GetDevice()
{
	return m_device.Get();
}

ID3D12GraphicsCommandList* Direct3DClass::GetCommandList()
{
	return m_commandList.Get();
}

int Direct3DClass::GetCurrentFrame()
{
	return m_frameIndex;
}

bool Direct3DClass::WaitforFrameToFinish()
{
	HRESULT result;

	//Swap the current render target view buffer index so drawing is don on the correct buffer
	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

	//If the current fence value is still less than the "m_fenceValue", then GPU has not finished
	//the command queue since it has not reached the "m_commandQueue->Signal" command
	if (m_fence[m_frameIndex]->GetCompletedValue() < m_fenceValue[m_frameIndex])
	{
		result = m_fence[m_frameIndex]->SetEventOnCompletion(m_fenceValue[m_frameIndex], m_fenceEvent);
		if (FAILED(result))
		{
			return false;
		}

		//Wait until the fence has triggered the correct fence event
		WaitForSingleObject(m_fenceEvent, INFINITE);
	}

	//Increment m_fenceValue for next frame
	m_fenceValue[m_frameIndex]++;

	return true;
}
