#pragma once
#include <inc/PNG.h>

class Menu
{
private:
	PNG png;
	string getFileName();
	void showApplicationInfo();
	int getOption();
public:
	void Init_application();
};