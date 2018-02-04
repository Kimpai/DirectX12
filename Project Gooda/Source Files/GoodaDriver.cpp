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

void GoodaDriver::Initialize(HWND hwnd, Camera* camera)
{
	//Create the Direct3D object
	m_Direct3D = new Direct3D();
	assert(m_Direct3D);

	//Create the Color Shader object
	m_ColorShader = new ColorShader();
	assert(m_ColorShader);

	//Create the Model object
	m_Cube = new Cube();
	assert(m_Cube);

	//Initialize the Direct3D object
	m_Direct3D->Initialize(SCREEN_HEIGHT, SCREEN_HEIGHT, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);

	//Initialize the Color Shader object
	m_ColorShader->Initialize(m_Direct3D->GetDevice(), hwnd, SCREEN_HEIGHT, SCREEN_WIDTH);

	//Initialize the model object
	m_Cube->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetCommandList(), camera, SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_NEAR, SCREEN_DEPTH, XMFLOAT4(0.0f, 0.0f, 0.5f, 1.0f));

	//Close the command list now that all the commands have been recorded
	m_Direct3D->CloseCommandList();

	//Execute the command list
	m_Direct3D->ExecuteCommandList();
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

void GoodaDriver::Frame(Camera* camera)
{
	//Update constant buffer
	m_ColorShader->Frame(m_Direct3D->GetCurrentFrame());
	m_Cube->Frame(m_Direct3D->GetCurrentFrame(), camera);

	//Render the graphics scene
	Render();
}

int GoodaDriver::RenderQuad(lua_State * L)
{	
	if (lua_isboolean(L, 1))
		renderQuad = lua_toboolean(L, 1);

	return 0;
}

void GoodaDriver::Render()
{
	//Reset the command list and put it in a recording state
	m_Direct3D->BeginScene(m_ColorShader);

	if (true)
	{
		m_Cube->Render(m_Direct3D->GetCommandList(), m_Direct3D->GetCurrentFrame());
	}

	//Close the command list and execute the commands
	m_Direct3D->EndScene();
}
