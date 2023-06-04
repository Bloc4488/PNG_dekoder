#include <inc/RSA.h>

rsa::rsa()
{
    _keySize = 1024;
    _privateKey = BN_new();
    _publicKey = BN_new();
    _n = BN_new();
    BIGNUM* randIV = BN_new();
    BN_rand(randIV, _keySize, BN_RAND_TOP_TWO, BN_RAND_BOTTOM_ODD);
    IV = BIGNUMToVector(randIV);
    BN_clear_free(randIV);
    keyGenerator();
}

void rsa::keyGenerator()
{
    BIGNUM* p = BN_new();
    BIGNUM* q = BN_new();
    BIGNUM* phi = BN_new();
    BIGNUM* e = BN_new();
    BIGNUM* d = BN_new();
    BN_CTX* ctx = BN_CTX_new();

    BN_generate_prime_ex(p, _keySize / 2, 0, nullptr, nullptr, nullptr);
    BN_generate_prime_ex(q, _keySize / 2, 0, nullptr, nullptr, nullptr);
    BN_mul(_n, p, q, ctx);

    BN_sub_word(p, 1);
    BN_sub_word(q, 1);
    BN_mul(phi, p, q, ctx);

    BN_rand_range(e, phi);
    while (!_gcd(e, phi))
    {
        BN_add(e, e, BN_value_one());
    }
    BN_mod_inverse(d, e, phi, ctx);
    BN_copy(_publicKey, e);
    BN_copy(_privateKey, d);

    /*OSSL_PARAM params[] = {
        OSSL_PARAM_BN("n", _n, 0),
        OSSL_PARAM_BN("e", e, 0),
        OSSL_PARAM_BN("d", d, 0),
        OSSL_PARAM_END
    };
    EVP_PKEY_CTX* ctxEVP = EVP_PKEY_CTX_new_from_name(NULL, "RSA", NULL);
    _pkey = NULL;

    if (ctxEVP == NULL || EVP_PKEY_fromdata_init(ctxEVP) <= 0 || EVP_PKEY_fromdata(ctxEVP, &_pkey, EVP_PKEY_KEYPAIR, params) <= 0)
        cerr << "Invalid conversation from 'BIGNUM*' to 'EVP_PKEY''";*/

    BN_CTX_free(ctx);
    BN_clear_free(p);
    BN_clear_free(e);
    BN_clear_free(d);
    BN_clear_free(q);
}

void rsa::printBignum(const BIGNUM* num)
{
    char* str = BN_bn2dec(num);
    if (str)
    {
        std::cout << str << std::endl;
        OPENSSL_free(str);
    }
}

bool rsa::_gcd(const BIGNUM* a, const BIGNUM* b) const
{
    BIGNUM* gcd = BN_new();
    BN_CTX* ctx = BN_CTX_new();
    BN_gcd(gcd, a, b, ctx);
    bool isOne = (BN_cmp(gcd, BN_value_one()) == 0);
    BN_CTX_free(ctx);
    BN_clear_free(gcd);
    return isOne;
}

void rsa::vectorToBIGNUM(const vector<uint8_t> data, BIGNUM* bn)
{
    BN_bin2bn(data.data(), data.size(), bn);

}

vector<uint8_t> rsa::BIGNUMToVector(const BIGNUM* bn)
{
    vector<uint8_t> data(BN_num_bytes(bn));
    BN_bn2bin(bn, data.data());
    return data;
}

void rsa::BIGNUMToVector(const BIGNUM* bn, vector<uint8_t>& data)
{
    BN_bn2binpad(bn, data.data(), data.size());
}

void rsa::encrypt(const BIGNUM* plaintext, BIGNUM* ciphertext)
{
    BN_CTX* ctx = BN_CTX_new();

    // ciphertext = plaintext^e mod _n
    BN_mod_exp(ciphertext, plaintext, _publicKey, _n, ctx);

    BN_CTX_free(ctx);
}

void rsa::decrypt(const BIGNUM* ciphertext, BIGNUM* plaintext)
{
    BN_CTX* ctx = BN_CTX_new();

    // plaintext = ciphertext^d mod _n
    BN_mod_exp(plaintext, ciphertext, _privateKey, _n, ctx);

    BN_CTX_free(ctx);
}

vector<uint8_t> rsa::encryptECBmode(vector<uint8_t> data)
{
	vector<uint8_t> dataHash;
	size_t blockSize = _keySize / 8 - 1;
	for (size_t i = 0; i < data.size(); i += blockSize)
	{
		size_t blockLength = min(blockSize, data.size() - i);
		vector<uint8_t> block(data.begin() + i, data.begin() + i + blockLength);
		block.insert(block.begin(), 0x01);
        BIGNUM* m = BN_new(), *cipher = BN_new();  
        vectorToBIGNUM(block, m);
        encrypt(m, cipher);
		vector<uint8_t> encrypt;
        encrypt = BIGNUMToVector(cipher);
        if (encrypt.size() < blockSize + 1)
        {
            uint8_t numberOfElements = blockSize - encrypt.size() + 2;
            while (encrypt.size() != blockSize + 1)
            {
                encrypt.push_back(0x00);
            }
            encrypt.push_back(numberOfElements);
        }
        else if (encrypt.size() == blockSize + 1)
        {
            encrypt.push_back(0x01);
        }
        dataHash.insert(dataHash.end(), encrypt.begin(), encrypt.end());
        BN_clear_free(m);
        BN_clear_free(cipher);
	}
    return dataHash;
}

vector<uint8_t> rsa::decryptECBmode(vector<uint8_t> data)
{
    vector<uint8_t> dataClean;
    size_t blockSize = _keySize / 8 + 1;

    for (size_t i = 0; i < data.size(); i += blockSize)
    {
        size_t blockLength = min(blockSize, data.size() - i);
        vector<uint8_t> block(data.begin() + i, data.begin() + i + blockLength);
        uint8_t numberOfElements = block.back();
        for (size_t j = 0; j < numberOfElements; j++)
        {
            block.erase(block.end() - 1);
        }
        BIGNUM* m = BN_new(), *plain = BN_new();
        vectorToBIGNUM(block, m);
        decrypt(m, plain);
        vector<uint8_t> decrypt;
        decrypt.resize(blockSize - 1);
        decrypt = BIGNUMToVector(plain);
        decrypt.erase(decrypt.begin());
        dataClean.insert(dataClean.end(), decrypt.begin(), decrypt.end());
        BN_clear_free(m);
        BN_clear_free(plain);
    }
    return dataClean;
}

vector<uint8_t> rsa::encryptCBCmode(vector<uint8_t> data)
{
    vector<uint8_t> dataHash;
    size_t blockSize = _keySize / 8 - 1;
    vector<uint8_t> IV_crypt = IV;
    for (size_t i = 0; i < data.size(); i += blockSize)
    {
        size_t blockLength = min(blockSize, data.size() - i);
        vector<uint8_t> block(data.begin() + i, data.begin() + i + blockLength);
        block.insert(block.begin(), 0x01);
        transform(block.begin(), block.end(), IV_crypt.begin(), block.begin(), [](auto e1, auto e2) {return e1 ^ e2;});        
        BIGNUM* m = BN_new(), * cipher = BN_new();
        vectorToBIGNUM(block, m);
        encrypt(m, cipher);
        vector<uint8_t> encrypt;
        encrypt = BIGNUMToVector(cipher);
        IV_crypt = encrypt;
        if (encrypt.size() < blockSize + 1)
        {
            uint8_t numberOfElements = blockSize - encrypt.size() + 2;
            while (encrypt.size() != blockSize + 1)
            {
                encrypt.push_back(0x00);
            }
            encrypt.push_back(numberOfElements);
        }
        else if (encrypt.size() == blockSize + 1)
        {
            encrypt.push_back(0x01);
        }
        while (IV_crypt.size() < blockSize + 1)
        {
            IV_crypt.push_back(0x00);
        }
        dataHash.insert(dataHash.end(), encrypt.begin(), encrypt.end());
        BN_clear_free(m);
        BN_clear_free(cipher);
    }
    return dataHash;
}

vector<uint8_t> rsa::decryptCBCmode(vector<uint8_t> data)
{
    vector<uint8_t> dataClean;
    size_t blockSize = _keySize / 8 + 1;
    vector<uint8_t> IV_crypt = IV;
    for (size_t i = 0; i < data.size(); i += blockSize)
    {
        size_t blockLength = min(blockSize, data.size() - i);
        vector<uint8_t> block(data.begin() + i, data.begin() + i + blockLength);
        uint8_t numberOfElements = block.back();
        for (size_t j = 0; j < numberOfElements; j++)
        {
            block.erase(block.end() - 1);
        }
        BIGNUM* m = BN_new(), * plain = BN_new();
        vectorToBIGNUM(block, m);
        decrypt(m, plain);
        vector<uint8_t> decrypt;
        decrypt = BIGNUMToVector(plain);
        transform(decrypt.begin(), decrypt.end(), IV_crypt.begin(), decrypt.begin(), [](auto e1, auto e2) {return e2 ^ e1;});
        decrypt.erase(decrypt.begin());
        IV_crypt = block;
        while (IV_crypt.size() < blockSize - 1)
        {
            IV_crypt.push_back(0x00);
        }
        dataClean.insert(dataClean.end(), decrypt.begin(), decrypt.end());
        BN_clear_free(m);
        BN_clear_free(plain);
    }
    return dataClean;
}

vector<uint8_t> rsa::encryptLibrarymode(vector<uint8_t> data)
{
    OpenSSL_add_all_algorithms();
    RSA* public_key;
    public_key = RSA_new();
    RSA_set0_key(public_key, _n, _publicKey, NULL);
    size_t encryptedSize = RSA_size(public_key);
    vector<uint8_t> cipher;
    for (size_t i = 0; i < data.size(); i += encryptedSize)
    {
        size_t blockLength = min(encryptedSize, data.size() - i);
        vector<uint8_t> block(data.begin() + i, data.begin() + i + blockLength);
        unsigned char* bloc = reinterpret_cast<unsigned char*>(block.data()), *encryp = (unsigned char*)malloc(blockLength);
        vector<uint8_t> encrypt(blockLength);
        if (RSA_public_encrypt(blockLength, bloc, encryp, public_key, RSA_NO_PADDING) < 0)
        {
            char buf[128];
            cerr << "RSA_public_encrypt: " << ERR_error_string(ERR_get_error(), buf) << endl;
        }
        cipher.insert(cipher.end(), encrypt.begin(), encrypt.end());
    }
    return cipher;
}

vector<uint8_t> rsa::decryptLibrarymode(vector<uint8_t> data)
{
    /*vector<uint8_t> plain;
    EVP_PKEY_CTX* ctxEVP = EVP_PKEY_CTX_new(_pkey, NULL);
    size_t plainLength;
    EVP_PKEY_decrypt_init(ctxEVP);
    EVP_PKEY_CTX_set_rsa_padding(ctxEVP, RSA_PKCS1_OAEP_PADDING);
    EVP_PKEY_decrypt(ctxEVP, NULL, &plainLength, data.data(), data.size());

    plain.resize(plainLength);
    EVP_PKEY_decrypt(ctxEVP, plain.data(), &plainLength, data.data(), data.size());

    EVP_PKEY_CTX_free(ctxEVP);
    return plain;*/
    return vector<uint8_t>();
}



