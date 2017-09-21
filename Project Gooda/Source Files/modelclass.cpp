#include "modelclass.h"

ModelClass::ModelClass()
{

}

ModelClass::ModelClass(const ModelClass& other)
{
}

ModelClass::~ModelClass()
{
	
}

bool ModelClass::Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
	bool result;

	result = InitializeBuffers(device, commandList);
	if (!result)
	{
		return false;
	}

	return true;
}

void ModelClass::Render(ID3D12GraphicsCommandList* commandList)
{
	RenderBuffers(commandList);
}

void ModelClass::Shutdown()
{
	ShutdownBuffers();
}
