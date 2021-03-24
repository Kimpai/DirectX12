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
			//std::cout << message << std::endl;
		}
	}

	void Console::LOG(std::string message)
	{
		m_messages.push_back(message);
	}

	void Console::ParseCommandLineArgument()
	{
		int argc;
		wchar_t** argv = ::CommandLineToArgvW(::GetCommandLineW(), &argc);

		for (size_t i = 0; i < argc; i++)
		{
			if (::wcscmp(argv[i], L"-w") == 0 || ::wcscmp(argv[i], L"--width") == 0)
			{
				UINT32 width = ::wcstol(argv[++i], nullptr, 10);
				LOG("--width :" + std::to_string(width));
			}
			if (::wcscmp(argv[i], L"-h") == 0 || ::wcscmp(argv[i], L"--height") == 0)
			{
				UINT32 height = ::wcstol(argv[++i], nullptr, 10);
				LOG("--height :" + std::to_string(height));
			}
			if (::wcscmp(argv[i], L"-warp") == 0 || ::wcscmp(argv[i], L"--warp") == 0)
			{
				LOG("--warp :" + std::to_string(true));
			}
		}
	}
}