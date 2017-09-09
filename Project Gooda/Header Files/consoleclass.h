#pragma once

#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <string>

class ConsoleClass
{
public:
	ConsoleClass();
	ConsoleClass(const ConsoleClass&);
	~ConsoleClass();
	
	bool Initialize();
	bool Frame(std::string&);

private:
	HANDLE m_stdOutputHandle;
	HANDLE m_stdInputHandle;
	HANDLE m_stdErrorHandle;

	bool InputHandle(std::string&);
};