#pragma once
#include <inc/PNG.h>

class Menu
{
private:
	PNG png;
	string getFileName();
	void showApplicationInfo();
	int getOptionMain();
	int getOptionRSA();
	void showRSAInfo();
	void chooseRSA();
public:
	void Init_application();
};