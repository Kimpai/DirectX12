#pragma once

#include <vector>

#include "../Gooda/Console.h"
#include "../resources/Model.h"
#include "../utils/DX11/Direct3D11.h"
#include "../utils/DX12/Direct3D12.h"

using namespace Microsoft::WRL;

namespace GoodaCore
{
	enum class Backend { DX11, DX12, NOACTIVE };

	class Renderer : public Gooda
	{
	public:
		virtual ~Renderer() = default;

		virtual bool Init(Backend);
		virtual bool Destroy();

		static Renderer* Instance();
		Backend GetBackend();
		void SetBackend(Backend);

		bool Render(std::vector<Model*>&);

	private:
		Renderer();

		Backend m_backend;
	};
}