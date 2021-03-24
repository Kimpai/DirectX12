#include "Gooda.h"
#include "Console.h"
#include "Input.h"

void Gooda::Release()
{
	if (this)
		delete this;
}

bool Gooda::Frame()
{
	return true;
}
