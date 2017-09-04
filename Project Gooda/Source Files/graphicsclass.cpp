#include "graphicsclass.h"

GraphicsClass::GraphicsClass()
{
	m_Direct3D = nullptr;
	m_ColorShader = nullptr;
	m_Model = nullptr;
}

GraphicsClass::GraphicsClass(const GraphicsClass& other)
{

}

GraphicsClass::~GraphicsClass()
{
	
}

bool GraphicsClass::Initialize(int screenHeight, int screenWidth, HWND hwnd)
{
	bool result;

	//Create the Direct3D object
	m_Direct3D = new Direct3DClass;
	if (!m_Direct3D)
	{
		return false;
	}

	//Create the Color Shader object
	m_ColorShader = new ColorShaderClass;
	if (!m_ColorShader)
	{
		return false;
	}

	//Create the Model object
	m_Model = new ModelClass;
	if (!m_Model)
	{
		return false;
	}

	//Initialize the Direct3D object
	result = m_Direct3D->Initialize(screenHeight, screenWidth, hwnd, VSYNC_ENABLED, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, (LPCSTR)L"Could not initialize Direct3D", (LPCSTR)L"Error", MB_OK);
		return false;
	}

	//Initialize the Color Shader object
	result = m_ColorShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, (LPCSTR)L"Could not initialize Color Shader", (LPCSTR)L"Error", MB_OK);
		return false;
	}

	//Initialize the model object
	result = m_Model->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetCommandList());
	if (!result)
	{
		MessageBox(hwnd, (LPCSTR)L"Could not initialize Model", (LPCSTR)L"Error", MB_OK);
		return false;
	}

	//Close the command list now that all the commands have been recorded
	result = m_Direct3D->CloseCommandList();
	if (!result)
	{
		return false;
	}

	//Execute the command list
	result = m_Direct3D->ExecuteCommandList();
	if (!result)
	{
		return false;
	}

	return true;
}

void GraphicsClass::Shutdown()
{
	//Release the Model object
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = nullptr;
	}

	//Release the Color Shader object
	if (m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = nullptr;
	}

	//Release the Direct3D object
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = nullptr;
	}

	return;
}

bool GraphicsClass::Frame()
{
	bool result;

	//Render the graphics scene
	result = Render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool GraphicsClass::Render()
{
	bool result;

	//Reset the command list and put it in a recording state
	result = m_Direct3D->BeginScene(m_ColorShader->GetPipelineState());
	if (!result)
	{
		return false;
	}

	m_Model->Render(m_Direct3D->GetCommandList());

	m_ColorShader->Render(m_Direct3D->GetCommandList());

	//Close the command list and execute the commands
	result = m_Direct3D->EndScene();
	if (!result)
	{
		return false;
	}

	return true;
}