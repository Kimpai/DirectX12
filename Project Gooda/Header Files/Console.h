#pragma once

#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <assert.h>
#include <Gooda.h>

class Console : public Gooda
{
public:
	Console();
	~Console();
	
	bool Frame(std::string&);

private:
	HANDLE m_stdOutputHandle;
	HANDLE m_stdInputHandle;
	HANDLE m_stdErrorHandle;

	bool InputHandle(std::string&);
};