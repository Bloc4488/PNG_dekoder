#include <inc/RSA.h>

RSA::RSA()
{
	primeFiller();
	setKeys();
}

void RSA::primeFiller()
{
	vector<bool> seive(250, true);
	seive[0] = false;
	seive[1] = false;
	for (int i = 2; i < 250; i++)
	{
		for (int j = i * 2; j < 250; j += i)
		{
			seive[j] = false;
		}
	}
	for (int i = 0; i < seive.size(); i++)
	{
		if (seive[i]) _prime.push_back(i);
	}
}

void RSA::setKeys()
{
	int prime1 = pickRandomPrime();
	int prime2 = pickRandomPrime();
	cout << prime1 << " " << prime2 << endl;
	n = prime1
}

int RSA::pickRandomPrime()
{
	int k = rand() % _prime.size();
	auto it = _prime.begin();
	while (k--)
	{
		it++;
	}
	int ret = *it;
	_prime.erase(it);
	return ret;
}