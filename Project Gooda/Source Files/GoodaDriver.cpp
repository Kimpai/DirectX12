#include "GoodaDriver.h"

GoodaDriver::GoodaDriver()
{
	m_Direct3D = nullptr;
	m_ColorShader = nullptr;
	m_Cube = nullptr;
}

GoodaDriver::GoodaDriver(const GoodaDriver& other)
{

}

GoodaDriver::~GoodaDriver()
{
	
}

bool GoodaDriver::Initialize(HWND hwnd, Camera* camera)
{
	bool result;

	//Create the Direct3D object
	m_Direct3D = new Direct3D();
	if (!m_Direct3D)
	{
		return false;
	}

	//Create the Color Shader object
	m_ColorShader = new ColorShader();
	if (!m_ColorShader)
	{
		return false;
	}

	//Create the Model object
	m_Cube = new Cube();
	if (!m_Cube)
	{
		return false;
	}

	//Initialize the Direct3D object
	result = m_Direct3D->Initialize(SCREEN_HEIGHT, SCREEN_HEIGHT, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, (LPCSTR)L"Could not initialize Direct3D", (LPCSTR)L"Error", MB_OK);
		return false;
	}

	//Initialize the Color Shader object
	result = m_ColorShader->Initialize(m_Direct3D->GetDevice(), hwnd, SCREEN_HEIGHT, SCREEN_WIDTH);
	if (!result)
	{
		MessageBox(hwnd, (LPCSTR)L"Could not initialize Color Shader", (LPCSTR)L"Error", MB_OK);
		return false;
	}

	//Initialize the model object
	result = m_Cube->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetCommandList(), camera, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_NEAR, SCREEN_DEPTH, XMFLOAT4(0.0f, 0.0f, 0.5f, 1.0f));
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

void GoodaDriver::Shutdown()
{
	//Release the Model object
	if (m_Cube)
	{
		delete m_Cube;
		m_Cube = nullptr;
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
		delete m_Direct3D;
		m_Direct3D = nullptr;
	}

	return;
}

bool GoodaDriver::Frame(Camera* camera)
{
	bool result;

	//Update constant buffer
	m_ColorShader->Frame(m_Direct3D->GetCurrentFrame());
	m_Cube->Frame(m_Direct3D->GetCurrentFrame(), camera);

	//Render the graphics scene
	result = Render();
	if (!result)
	{
		return false;
	}

	return true;
}

int GoodaDriver::RenderQuad(lua_State * L)
{	
	if (lua_isboolean(L, 1))
		renderQuad = lua_toboolean(L, 1);

	return 0;
}

bool GoodaDriver::Render()
{
	bool result;

	//Reset the command list and put it in a recording state
	result = m_Direct3D->BeginScene(m_ColorShader);
	if (!result)
		return false;

	if (renderQuad)
	{
		m_Cube->Render(m_Direct3D->GetCommandList(), m_Direct3D->GetCurrentFrame());
	}

	//Close the command list and execute the commands
	result = m_Direct3D->EndScene();
	if (!result)
		return false;

	return true;
}
