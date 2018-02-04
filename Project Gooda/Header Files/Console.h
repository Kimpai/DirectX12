#pragma once

#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <string>

class Console
{
public:
	Console();
	Console(const Console&);
	~Console();
	
	bool Initialize();
	bool Frame(std::string&);

private:
	HANDLE m_stdOutputHandle;
	HANDLE m_stdInputHandle;
	HANDLE m_stdErrorHandle;

	bool InputHandle(std::string&);
};