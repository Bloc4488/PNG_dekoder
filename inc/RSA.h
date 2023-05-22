#pragma once
#include <cmath>
#include <iostream>
#include <random>
#include <ctime>
#include <openssl/bn.h>

using namespace std;

class rsa
{
private:
    BIGNUM* _public_key;
    BIGNUM* _private_key;
    BIGNUM* _n;
    int _keySize;
public:
    rsa();
    void keyGenerator();
    void printBignum(const BIGNUM* num);
    bool _gcd(const BIGNUM* a, const BIGNUM* b) const;
    void decrypt(const BIGNUM* ciphertext, BIGNUM* plaintext);
    void encrypt(const BIGNUM* plaintext, BIGNUM* ciphertext);
};



