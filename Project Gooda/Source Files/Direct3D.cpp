#include "Direct3D.h"

Direct3D::Direct3D(int screenHeight, int screenWidth, HWND hwnd, bool fullscreen, float screenDepth, float screenNear)
{
	CreateDirect3DDevice(hwnd);

	CreateCommandInterfaceAndSwapChain(hwnd, screenWidth, screenHeight, fullscreen);

	//Initialize the frameIndex to the current back buffer index
	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

	CreateFenceAndEventHandle();

	CreateRenderTargets();

	CreateViewPortAndScissorRect(screenWidth, screenHeight);
}

Direct3D::~Direct3D()
{
	//Wait of GPU to finish before releasing com objects
	for (int i = 0; i < frameBufferCount; i++)
	{
		m_frameIndex = i;
		DeviceSynchronize();
	}

	//Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
	if (m_swapChain)
		m_swapChain->SetFullscreenState(false, NULL);

	//Close the object handle to the fence event.
	CloseHandle(m_fenceEvent);

	for (int i = 0; i < frameBufferCount; i++)
	{
		//Release the fence.
		if (m_fence[i])
			m_fence[i] = nullptr;
	}

	//Release the command list.
	if (m_commandList)
		m_commandList = nullptr;

	for (int i = 0; i < frameBufferCount; i++)
	{
		//Release the command allocator.
		if (m_commandAllocator[i])
			m_commandAllocator[i] = nullptr;
	}

	//Release the back buffer render target views.
	for (int i = 0; i < frameBufferCount; i++)
	{
		if (m_backBufferRenderTarget[i])
			m_backBufferRenderTarget[0] = nullptr;
	}

	//Release the render target view heap.
	if (m_renderTargetViewDescHeap)
		m_renderTargetViewDescHeap = nullptr;

	//Release the swap chain.
	if (m_swapChain)
		m_swapChain = nullptr;

	//Release the command queue.
	if (m_commandQueue)
		m_commandQueue = nullptr;

	//Release the device.
	if (m_device)
		m_device = nullptr;
}

void Direct3D::BeginScene(ShaderManager* shader)
{
	//Swap the current render target view buffer index so drawing is don on the correct buffer
	m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

	//If the current fence value is still less than the "m_fenceValue", then GPU has not finished
	//the command queue since it has not reached the "m_commandQueue->Signal" command
	if (m_fence[m_frameIndex]->GetCompletedValue() < m_fenceValue[m_frameIndex])
	{
		assert(!m_fence[m_frameIndex]->SetEventOnCompletion(m_fenceValue[m_frameIndex], m_fenceEvent));

		//Wait until the fence has triggered the correct fence event
		WaitForSingleObject(m_fenceEvent, INFINITE);
	}

	//Increment m_fenceValue for next frame
	m_fenceValue[m_frameIndex]++;

	//Can only reset an allocator once the GPU is done with it.
	//Resetting it will free the memory that the command list was stored in
	assert(!m_commandAllocator[m_frameIndex]->Reset());

	//Reset the command list so that it can start recording again.
	//Only one command list can record at a given time.
	assert(!m_commandList->Reset(m_commandAllocator[m_frameIndex].Get(), shader->GetPipelineState(ShaderPipelineType::COLOR)));

	//Record commands in the command list now.
	//Start by setting the resource barrier.
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource = m_backBufferRenderTarget[m_frameIndex].Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

	m_commandList->ResourceBarrier(1, &barrier);

	//Get the render target view handle for the current back buffer.
	D3D12_CPU_DESCRIPTOR_HANDLE renderTargetViewHandle = m_renderTargetViewDescHeap->GetCPUDescriptorHandleForHeapStart();
	renderTargetViewHandle.ptr += m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV) * m_frameIndex;

	//Get the depth stencil view handle
	D3D12_CPU_DESCRIPTOR_HANDLE depthStencilViewHandle(shader->GetDepthStencilViewHandle());

	//Set the back buffer as the render target.
	m_commandList->OMSetRenderTargets(1, &renderTargetViewHandle, FALSE, &depthStencilViewHandle);

	//Then set the color to clear the window to.
	float color[4] = { 0.5f, 0.5f, 0.5f, 1.0f };

	m_commandList->ClearRenderTargetView(renderTargetViewHandle, color, 0, NULL);
	m_commandList->ClearDepthStencilView(shader->GetDepthStencilViewHandle(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, NULL);
	m_commandList->SetGraphicsRootSignature(shader->GetRootSignature());
	
	ID3D12DescriptorHeap* descriptorHeaps[] = { shader->GetDescriptorHeap(m_frameIndex) };
	m_commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	m_commandList->RSSetViewports(1, &m_viewport);
	m_commandList->RSSetScissorRects(1, &m_rect);
}

void Direct3D::EndScene()
{
	ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };

	//Indicate that the back buffer will now be used to present.
	m_commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(m_backBufferRenderTarget[m_frameIndex].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	//Close the list of commands.
	assert(!m_commandList->Close());

	//Load the command list array (only one command list for now).
	ppCommandLists[0] = m_commandList.Get();

	//Execute the list of commands.
	m_fenceValue[m_frameIndex]++;
	m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	//Signal and increment the fence value.
	assert(!m_commandQueue->Signal(m_fence[m_frameIndex].Get(), m_fenceValue[m_frameIndex]));

	//Finally present the back buffer to the screen since rendering is complete.
	if (m_vsync)
	{
		//Lock to screen refresh rate.
		assert(!m_swapChain->Present(1, 0));
	}
	else
	{
		//Present as fast as possible.
		assert(!m_swapChain->Present(0, 0));
	}
}

void Direct3D::CloseCommandList()
{
	assert(!m_commandList->Close());
}

void Direct3D::ResetCommandList(ID3D12PipelineState* pipelinestate)
{
	assert(!m_commandList->Reset(m_commandAllocator[m_frameIndex].Get(), pipelinestate));
}

void Direct3D::ExecuteCommandList()
{
	ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };

	//Load the command list array (only one command list for now).
	ppCommandLists[0] = m_commandList.Get();

	//Execute the list of commands.
	m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	//Signal and increment the fence value.
	m_fenceValue[m_frameIndex]++;
	assert(!m_commandQueue->Signal(m_fence[m_frameIndex].Get(), m_fenceValue[m_frameIndex]));
}

ID3D12Device* Direct3D::GetDevice()
{
	return m_device.Get();
}

ID3D12GraphicsCommandList* Direct3D::GetCommandList()
{
	return m_commandList.Get();
}

int Direct3D::GetCurrentFrame()
{
	return m_frameIndex;
}

void Direct3D::DeviceSynchronize()
{
	//If the current fence value is still less than the "m_fenceValue", then GPU has not finished
	//the command queue since it has not reached the "m_commandQueue->Signal" command
	if (m_fence[m_frameIndex]->GetCompletedValue() < m_fenceValue[m_frameIndex])
	{
		assert(!m_fence[m_frameIndex]->SetEventOnCompletion(m_fenceValue[m_frameIndex], m_fenceEvent));

		//Wait until the fence has triggered the correct fence event
		WaitForSingleObject(m_fenceEvent, INFINITE);
	}

	//Increment m_fenceValue for next frame
	m_fenceValue[m_frameIndex]++;
}

void Direct3D::CreateDirect3DDevice(HWND hwnd)
{

#if _DEBUG
	//Get the interface to DirectX 12 debugger
	ComPtr<ID3D12Debug> debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(__uuidof(ID3D12Debug), (void**)&debugController)))
		debugController->EnableDebugLayer();

	//Release the debug controller now that the debug layer has been enabled
	debugController = nullptr;
#endif

	ComPtr<IDXGIFactory5> factory = nullptr;
	ComPtr<IDXGIAdapter1> adapter = nullptr;

	CreateDXGIFactory(__uuidof(IDXGIFactory5), (void**)&factory);
	for (UINT adapterIndex = 0;; ++adapterIndex)
	{
		adapter = nullptr;
		if (DXGI_ERROR_NOT_FOUND == factory->EnumAdapters1(adapterIndex, &adapter))
			break;

		//Check to see if the adapter supports Direct3D 12, but don't create the device yet
		if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_1, __uuidof(ID3D12Device), NULL)))
			break;
	}

	if (adapter)
	{
		//Create the device
		assert(!D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_1, __uuidof(ID3D12Device), (void**)&m_device));
	}
	else
	{
		//Create warp device if no adapter was found
		factory->EnumWarpAdapter(__uuidof(IDXGIAdapter1), (void**)&adapter);
		assert(!D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), (void**)&m_device));
	}

	factory = nullptr;
}

void Direct3D::CreateViewPortAndScissorRect(int screenWidth, int screenHeight)
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
}

void Direct3D::CreateFenceAndEventHandle()
{
	for (int i = 0; i < frameBufferCount; i++)
	{
		//Create a fence for GPU synchronization.
		assert(!m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&m_fence[i]));

		m_fenceValue[i] = 0;
	}

	//Create an event object for the fence.
	m_fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
}

void Direct3D::CreateCommandInterfaceAndSwapChain(HWND hwnd, int screenWidth, int screenHeight, bool fullscreen)
{
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc = {};

	//Initialize the description of the command queue.
	ZeroMemory(&commandQueueDesc, sizeof(commandQueueDesc));

	//Set up the description of the command queue.
	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	commandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	commandQueueDesc.NodeMask = 0;

	//Create the command queue.
	assert(!m_device->CreateCommandQueue(&commandQueueDesc, __uuidof(ID3D12CommandQueue), (void**)&m_commandQueue));

	for (int i = 0; i < frameBufferCount; ++i)
	{
		//Create a command allocator.
		assert(!m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**)&m_commandAllocator[i]));
	}

	//Create a basic command list.
	assert(!m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator[0].Get(), NULL, __uuidof(ID3D12GraphicsCommandList), (void**)&m_commandList));

	//Create temporary factory to use when creating swapchain
	ComPtr<IDXGIFactory5> factory = nullptr;
	CreateDXGIFactory(__uuidof(IDXGIFactory5), (void**)&factory);

	//Use the factory to create an adapter for the primary graphics card
	ComPtr<IDXGIAdapter1> adapter = nullptr;
	assert(!factory->EnumAdapters1(0, &adapter));

	//Enumerate the primary monitor
	ComPtr<IDXGIOutput> adapterOutput = nullptr;
	assert(!adapter->EnumOutputs(0, &adapterOutput));

	//Get the number of modes
	unsigned int numModes = 0;
	assert(!adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL));

	//Create and hold all the possible display modes for this monitor/graphcis card combination
	DXGI_MODE_DESC* displayModeList = new DXGI_MODE_DESC[numModes];

	//Now fill thr display mode list struct
	assert(!adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList));

	//Now go through all the display modes and find one that matches the screen height and width
	//When a match is found  store the numerator and denominator of the refresh rate for thr monitor
	unsigned int numerator, denominator;
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

	if (displayModeList)
	{
		delete[] displayModeList;
		displayModeList = nullptr;
	}

	//Get the adapter desc
	DXGI_ADAPTER_DESC adapterDesc = {};
	assert(!adapter->GetDesc(&adapterDesc));

	//Store the dedicated video card memory in megabytes
	unsigned long long videoCardMemory = 0;
	videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);
	
	//Convert the name of the video card to a char array to store it
	size_t stringLength = 0;
	wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	ComPtr<IDXGISwapChain> swapChain = nullptr;

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

	//Set the refreshrate of the swapChain
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;

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
	if (fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

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

	//Finally create the swap chain using the swap chain description.	
	
	assert(!factory->CreateSwapChain(m_commandQueue.Get(), &swapChainDesc, &swapChain));

	//Next upgrade the IDXGISwapChain to a IDXGISwapChain3 interface and store it in a private member variable named m_swapChain.
	//This will allow us to use the newer functionality such as getting the current back buffer index.
	assert(!swapChain->QueryInterface(__uuidof(IDXGISwapChain3), (void**)&m_swapChain));

	//Clear pointer to original swap chain interface since we are using version 3 instead (m_swapChain).
	swapChain = nullptr;

	//Release the factory now that the swap chain has been created.
	factory = nullptr;
}

void Direct3D::CreateRenderTargets()
{
	unsigned int renderTargetViewDescriptorSize;
	D3D12_DESCRIPTOR_HEAP_DESC renderTargetViewHeapDesc = {};
	D3D12_CPU_DESCRIPTOR_HANDLE renderTargetViewHandle = {};

	//Initialize the render target view heap description for the two back buffers.
	ZeroMemory(&renderTargetViewHeapDesc, sizeof(renderTargetViewHeapDesc));

	//Set the number of descriptors to two for our two back buffers.  Also set the heap type to render target views.
	renderTargetViewHeapDesc.NumDescriptors = frameBufferCount;
	renderTargetViewHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	renderTargetViewHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	//Create the render target view heap for the back buffers.
	assert(!m_device->CreateDescriptorHeap(&renderTargetViewHeapDesc, __uuidof(ID3D12DescriptorHeap), (void**)&m_renderTargetViewDescHeap));

	//Get a handle to the starting memory location in the render target view heap to identify where the render target views will be located for the two back buffers.
	renderTargetViewHandle = m_renderTargetViewDescHeap->GetCPUDescriptorHandleForHeapStart();

	//Get the size of the memory location for the render target view descriptors.
	renderTargetViewDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	for (int i = 0; i < frameBufferCount; i++)
	{
		//Get a pointer to the first back buffer from the swap chain.
		assert(!m_swapChain->GetBuffer(i, __uuidof(ID3D12Resource), (void**)&m_backBufferRenderTarget[i]));

		//Create a render target view for the first back buffer.
		m_device->CreateRenderTargetView(m_backBufferRenderTarget[i].Get(), NULL, renderTargetViewHandle);

		//Increment the render target view handle by the render target view desc size
		renderTargetViewHandle.ptr += renderTargetViewDescriptorSize;
	}

}
