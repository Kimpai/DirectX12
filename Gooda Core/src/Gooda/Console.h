#pragma once

#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <Gooda.h>

namespace GoodaCore
{
	class Console : public Gooda
	{
	public:
		virtual ~Console() = default;

		static Console* Instance();

		virtual bool Init();
		virtual bool Frame(std::string&);
		virtual bool Destroy();

	private:
		Console() = default;
		bool InputHandle(std::string&);

		HANDLE m_stdOutputHandle;
		HANDLE m_stdInputHandle;
		HANDLE m_stdErrorHandle;
	};
}