#include "Console.h"

namespace GoodaCore
{
	Console::Console()
	{
		FILE* fp = nullptr;

		//Allocate a console
		AllocConsole();
		AttachConsole(GetCurrentProcessId());

		//Redirect unbuffered STDOUT to the console
		m_stdOutputHandle = GetStdHandle(STD_OUTPUT_HANDLE);

		freopen_s(&fp, "CONOUT$", "w", stdout);
		setvbuf(stdout, NULL, _IONBF, 0);

		//Redirect unbuffered STDIN to the console
		m_stdInputHandle = GetStdHandle(STD_INPUT_HANDLE);

		freopen_s(&fp, "CONIN$", "r", stdin);
		//setvbuf(stdin, NULL, _IONBF, 0);

		//Redirect unbuffered STDERR to the console
		m_stdErrorHandle = GetStdHandle(STD_ERROR_HANDLE);

		freopen_s(&fp, "CONOUT$", "w", stderr);
		setvbuf(stderr, NULL, _IONBF, 0);

		//Make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog 
		//point to console as well
		std::ios::sync_with_stdio();
	}

	Console* Console::Instance()
	{
		static Console s_console;
		return &s_console;
	}

	void Console::Frame()
	{
		for (auto message : m_messages)
		{
			std::cout << message << std::endl;
		}
	}

	void Console::LOG(std::string message)
	{
		m_messages.push_back(message);
	}

	bool Console::InputHandle(std::string& input)
	{
		DWORD result;
		char inputBuffer[512] = { 0 };
		INPUT_RECORD buffer[128];
		DWORD read;

		result = WaitForSingleObject(m_stdInputHandle, 0);

		if (result == WAIT_OBJECT_0)
		{
			PeekConsoleInput(m_stdInputHandle, buffer, 128, &read);

			switch (buffer->EventType)
			{
			case FOCUS_EVENT:
				FlushConsoleInputBuffer(m_stdInputHandle);
				break;

			case KEY_EVENT:
				ReadConsole(m_stdInputHandle, inputBuffer, 512, &read, NULL);
				input = inputBuffer;
				FlushConsoleInputBuffer(m_stdInputHandle);
				return true;

			case MOUSE_EVENT:
				FlushConsoleInputBuffer(m_stdInputHandle);
				break;

			case MENU_EVENT:
				FlushConsoleInputBuffer(m_stdInputHandle);
				break;

			default:
				break;
			}
		}

		FlushConsoleInputBuffer(m_stdInputHandle);

		return true;
	}
}