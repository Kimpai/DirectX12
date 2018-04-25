#include "Terrain.h"

Terrain::Terrain(char* file, int width, int height, float scale)
{
	m_height = height;
	m_width = width;
	m_scale = scale;
	m_file = file;
	m_heightMap = nullptr;
}

Terrain::~Terrain()
{
	if (m_vertexBuffer)
	{
		delete m_vertexBuffer;
		m_vertexBuffer = nullptr;
	}

	if (m_indexBuffer)
	{
		delete m_indexBuffer;
		m_indexBuffer = nullptr;
	}

	if (m_constantBuffer)
	{
		delete m_constantBuffer;
		m_constantBuffer = nullptr;
	}

	if (m_file)
	{
		delete[] m_file;
		m_file = nullptr;
	}

	if (m_heightMap)
	{
		delete[] m_heightMap;
		m_heightMap = nullptr;
	}
}

ConstantBuffer* Terrain::GetConstantBuffer()
{
	return m_constantBuffer;
}

void Terrain::InitializeBuffers(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
	//Load the bitmap file
	LoadBitmapHeightMap();

	//Setup the X and Z coordinates for the height map as well as scale the terrain height by the height scale value.
	SetCoordinates();

	//Determine the size of the vertex and index data
	//also used to set size of the vertex and index buffer
	const int size = (m_width - 1) * (m_height - 1) * 6;

	VertexPositionNormalColor* vertices = new VertexPositionNormalColor[size];
	unsigned int* indices = new unsigned int[size];

	//Build the 3D terrain
	Build(vertices);

	//Load the vertex and index arrays with data
	for (int i = 0; i < size; ++i)
		indices[i] = i;

	//Determine how many indices to draw
	m_indices = size;

	//Determine the size of the vertex and index buffers
	int vertexBufferSize = size * sizeof(VertexPositionNormalColor);
	int indexBufferSize = size * sizeof(DWORD);

	//Create a vertex buffer
	m_vertexBuffer = new VertexBuffer(vertices, vertexBufferSize, sizeof(VertexPositionNormalColor), device, commandList);
	
	//Create an index buffer
	m_indexBuffer = new IndexBuffer(indices, indexBufferSize, sizeof(DWORD), device, commandList);

	//Create a constant buffer for the world, view and projection matrices
	m_constantBuffer = new ConstantBuffer(&m_constantBufferData, sizeof(ConstantBufferData), device, commandList);

	//Release the vertex and index array now that we are done with them
	if (vertices)
	{
		delete[] vertices;
		vertices = nullptr;
	}

	if (indices)
	{
		delete[] indices;
		indices = nullptr;
	}
}

void Terrain::Render(ID3D12GraphicsCommandList* commandList, int currentFrame, CD3DX12_GPU_DESCRIPTOR_HANDLE handle)
{
	//Set the primitive topology
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Set the vertex buffer
	m_vertexBuffer->SetVertexBuffer();

	//Set the index buffer
	m_indexBuffer->SetIndexBuffer();

	commandList->SetGraphicsRootDescriptorTable(0, handle);

	//Draw
	commandList->DrawIndexedInstanced(m_indices, 1, 0, 0, 0);
}

void Terrain::LoadBitmapHeightMap()
{
	//Create the array structure to hold the height map data
	m_heightMap = new XMFLOAT3[m_width * m_height];
	assert(m_heightMap);

	//Open bitmap file in binary mode
	FILE* fileptr;
	assert(!fopen_s(&fileptr, m_file, "rb"));

	//Read in the bitmap header
	BITMAPFILEHEADER bitmapFileHeader;
	assert(std::fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, fileptr));

	//Read in the bitmap info header
	BITMAPINFOHEADER bitmapInfoHeader;
	assert(std::fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, fileptr));

	//Make sure the hight map dimensions are the same as the terrain dimensions
	m_height = bitmapInfoHeader.biHeight;
	m_width = bitmapInfoHeader.biWidth;

	//Calculate the size of the bitmap image data
	int imageSize = m_height * ((m_width * 3) + 1);
	unsigned char* bitmapImage = new unsigned char[imageSize];
	assert(bitmapImage);

	//Move to the beginning of the bitmap data
	std::fseek(fileptr, bitmapFileHeader.bfOffBits, SEEK_SET);

	//Read in the bitmap image data
	assert(std::fread(bitmapImage, 1, imageSize, fileptr));

	//Close the file
	assert(!fclose(fileptr));

	//Initialize the position in the image data buffer
	int k = 0;
	int index = 0;

	//Read the image data into the height map array
	for (int j = 0; j < m_height; ++j)
	{
		for (int i = 0; i < m_width; ++i)
		{
			//Bitmaps are upside down so load bottom to top into height map array
			index = (m_width * (m_height - 1 - j)) + i;

			//Get the grey scale pixel value from the bitmap image data at this location
			//unsigned char height = bitmapImage[k];

			float height = 1.0f;

			//Store the pixel value as the height at this point in the height map array
			m_heightMap[index].y = (float)height;

			//Increment the bitmap image data index
			k += 3;
		}

		++k;
	}

	//Release the bitmap image now that we are done with it
	if (bitmapImage)
	{
		delete[] bitmapImage;
		bitmapImage = nullptr;
	}
}

void Terrain::SetCoordinates()
{
	int index = 0;

	//Loop through all the elements in the height map array and adjust the coordinates correctly
	for (int j = 0; j < m_height; ++j)
	{
		for (int i = 0; i < m_width; ++i)
		{
			index = (m_width * j) + i;

			//Set the x and z coordinates
			m_heightMap[index].x = (float)i;
			m_heightMap[index].z = -(float)j;

			//Move the terrain depth into the positive rangez
			m_heightMap[index].z += (float)(m_height - 1);

			//Scale the height
			m_heightMap[index].y /= m_scale;
		}
	}
}

void Terrain::Build(VertexPositionNormalColor* vertices)
{
	//Initialize the index
	int index = 0;

	//Load the 3D terrain with the height map terrain data
	//We will be creating 2 triangles for each of the four points in a quad
	int index1, index2, index3, index4;
	for (int j = 0; j < (m_height - 1); ++j)
	{
		for (int i = 0; i < (m_width - 1); ++i)
		{
			//Get the indexes to the four points of the quad
			index1 = (m_width * j) + i;				//Upper left
			index2 = (m_width * j) + (i + 1);		//Upper right
			index3 = (m_width * (j + 1)) + i;		//Bottom left
			index4 = (m_width * (j + 1)) + (i + 1);	//Bottom right

			//Now create the triangles for that quad
			//Triangle 1 - Upper left
			vertices[index].position.x = m_heightMap[index1].x;
			vertices[index].position.y = m_heightMap[index1].y;
			vertices[index].position.z = m_heightMap[index1].z;
			vertices[index].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			vertices[index].color =	XMFLOAT4(0.0f, 0.5f, 0.0f, 1.0f);
			index++;

			//Triangle 1 - Upper right
			vertices[index].position.x = m_heightMap[index2].x;
			vertices[index].position.y = m_heightMap[index2].y;
			vertices[index].position.z = m_heightMap[index2].z;
			vertices[index].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			vertices[index].color = XMFLOAT4(0.0f, 0.5f, 0.0f, 1.0f);
			index++;

			//Triangle 1 - Bottom left
			vertices[index].position.x = m_heightMap[index3].x;
			vertices[index].position.y = m_heightMap[index3].y;
			vertices[index].position.z = m_heightMap[index3].z;
			vertices[index].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			vertices[index].color = XMFLOAT4(0.0f, 0.5f, 0.0f, 1.0f);
			index++;

			//Triangle 2 - Bottom left
			vertices[index].position.x = m_heightMap[index3].x;
			vertices[index].position.y = m_heightMap[index3].y;
			vertices[index].position.z = m_heightMap[index3].z;
			vertices[index].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			vertices[index].color = XMFLOAT4(0.0f, 0.5f, 0.0f, 1.0f);
			index++;

			//Triangle 2 - Upper right
			vertices[index].position.x = m_heightMap[index2].x;
			vertices[index].position.y = m_heightMap[index2].y;
			vertices[index].position.z = m_heightMap[index2].z;
			vertices[index].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			vertices[index].color = XMFLOAT4(0.0f, 0.5f, 0.0f, 1.0f);
			index++;

			//Triangle 2 - Bottom right
			vertices[index].position.x = m_heightMap[index4].x;
			vertices[index].position.y = m_heightMap[index4].y;
			vertices[index].position.z = m_heightMap[index4].z;
			vertices[index].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			vertices[index].color = XMFLOAT4(0.0f, 0.5f, 0.0f, 1.0f);
			index++;
		}
	}
}
