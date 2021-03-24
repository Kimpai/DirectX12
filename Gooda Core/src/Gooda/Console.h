#pragma once

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <Windows.h>
#include <shellapi.h>

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
		void ParseCommandLineArgument();

		HANDLE m_stdOutputHandle;
		HANDLE m_stdInputHandle;
		HANDLE m_stdErrorHandle;

		std::vector<std::string> m_messages;
	};
}