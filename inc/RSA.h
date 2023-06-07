#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <openssl/bn.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/bio.h>

using namespace std;

class rsa
{
private:
    BIGNUM* _privateKey;
    BIGNUM* _publicKey;
    BIGNUM* _n;
    size_t _keySize;
    vector<uint8_t> IV;
    //EVP_PKEY* _pkey;
public:
    rsa();
    rsa(size_t length);
    void keyGenerator();
    bool _gcd(const BIGNUM* a, const BIGNUM* b) const;
    void printBignum(const BIGNUM* num);
    void vectorToBIGNUM(const vector<uint8_t> data, BIGNUM* bn);
    vector<uint8_t> BIGNUMToVector(const BIGNUM* bn);
    void BIGNUMToVector(const BIGNUM* bn, vector<uint8_t>& data);
    void decrypt(const BIGNUM* ciphertext, BIGNUM* plaintext);
    void encrypt(const BIGNUM* plaintext, BIGNUM* ciphertext);
    vector<uint8_t> encryptECBmode(vector<uint8_t> data);
    vector<uint8_t> decryptECBmode(vector<uint8_t> data, size_t imageNumBytes);
    vector<uint8_t> encryptCBCmode(vector<uint8_t> data);
    vector<uint8_t> decryptCBCmode(vector<uint8_t> data, size_t imageNumBytes);
    vector<uint8_t> encryptLibrarymode(vector<uint8_t> data);
    vector<uint8_t> decryptLibrarymode(vector<uint8_t> data);
};



