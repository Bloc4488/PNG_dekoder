#pragma once
#include <inc/PNG.h>

class RSA
{
private:
	int _public_key;
	int _private_key;
	vector<int> _prime;
	int _n;
public:
	RSA();
	void primeFiller();
	int pickRandomPrime();
	void setKeys();
	~RSA();
};

