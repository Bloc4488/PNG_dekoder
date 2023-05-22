#pragma once
#include <inc/PNG.h>
#include <inc/RSA.h>

class Menu
{
private:
	PNG png;
	rsa rsa;
	string getFileName();
	void showApplicationInfo();
	int getOption();
public:
	void Init_application();
};