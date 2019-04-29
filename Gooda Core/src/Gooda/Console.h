#pragma once

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <Windows.h>

namespace GoodaCore
{
	class Console
	{
	public:
		virtual ~Console() = default;

		static Console* Instance();
		void Frame();
		void LOG(std::string);

	private:
		Console();
		bool InputHandle(std::string&);

		HANDLE m_stdOutputHandle;
		HANDLE m_stdInputHandle;
		HANDLE m_stdErrorHandle;

		std::vector<std::string> m_messages;
	};
}