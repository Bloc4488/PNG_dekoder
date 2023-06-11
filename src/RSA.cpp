#include <inc/RSA.h>

rsa::rsa()
{
    _keySize = 1024;
    _privateKey = BN_new();
    _publicKey = BN_new();
    _n = BN_new();
    BIGNUM* randIV = BN_new();
    BN_rand(randIV, _keySize, BN_RAND_TOP_ONE, BN_RAND_BOTTOM_ODD);
    IV = BIGNUMToVector(randIV);
    BN_clear_free(randIV);
    keyGenerator();
}

rsa::rsa(size_t length)
{
    _keySize = length;
    _privateKey = BN_new();
    _publicKey = BN_new();
    _n = BN_new();
    BIGNUM* randIV = BN_new();
    BN_rand(randIV, _keySize, BN_RAND_TOP_ONE, BN_RAND_BOTTOM_ODD);
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

    //BN_rand_range(e, phi);
    BN_set_word(e, 65537);
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
    size_t blockSize = floor(_keySize / 8) - 1;
    for (size_t i = 0; i < data.size(); i += blockSize)
    {
        vector<uint8_t> encryptVector;
        encryptVector.resize(blockSize + 1);
        BIGNUM* m = BN_new(), *cipher = BN_new();
        size_t blocklength = min(blockSize, data.size() - i);
        vector<uint8_t> block(data.begin() + i, data.begin() + i + blocklength);
        vectorToBIGNUM(block, m);
        encrypt(m, cipher);
        BIGNUMToVector(cipher, encryptVector);
        dataHash.insert(dataHash.end(), encryptVector.begin(), encryptVector.end());
        BN_clear_free(m);
        BN_clear_free(cipher);
    }
    return dataHash;
}

vector<uint8_t> rsa::decryptECBmode(vector<uint8_t> data, size_t imageNumBytes)
{
    vector<uint8_t> dataClean;
    size_t blockSize = floor(_keySize / 8);
    for (size_t i = 0; i < data.size(); i += blockSize)
    {
        vector<uint8_t> decryptVector;
        decryptVector.resize(blockSize - 1);
        size_t blockLength = min(blockSize, data.size() - i);
        vector<uint8_t> block(data.begin() + i, data.begin() + i + blockLength);
        BIGNUM* m = BN_new(), * plain = BN_new();
        vectorToBIGNUM(block, m);
        decrypt(m, plain);
        BIGNUMToVector(plain, decryptVector);
        dataClean.insert(dataClean.end(), decryptVector.begin(), decryptVector.end());
        BN_clear_free(m);
        BN_clear_free(plain);
    }
    if (dataClean.size() > imageNumBytes)
    {
        size_t bytesToErase = dataClean.size() - imageNumBytes;
        dataClean.erase(dataClean.end() - blockSize + 1, dataClean.end() - blockSize + 1 + bytesToErase);
    }
    return dataClean;
}

vector<uint8_t> rsa::encryptCBCmode(vector<uint8_t> data)
{
    vector<uint8_t> dataHash;
    size_t blockSize = floor(_keySize / 8) - 1;
    vector<uint8_t> IV_encrypt = IV;
    for (size_t i = 0; i < data.size(); i += blockSize)
    {
        vector<uint8_t> encryptVector;
        BIGNUM* m = BN_new(), * cipher = BN_new();
        encryptVector.resize(blockSize + 1);
        size_t blockLength = min(blockSize, data.size() - i);
        vector<uint8_t> block(data.begin() + i, data.begin() + i + blockLength);
        while (block.size() != blockSize)
        {
            block.insert(block.begin(), 0x00);
        }
        while (IV_encrypt.size() != block.size())
        {
            IV_encrypt.pop_back();
        }
        transform(block.begin(), block.end(), IV_encrypt.begin(), block.begin(), [](auto a, auto b) {return a ^ b;});
        vectorToBIGNUM(block, m);
        encrypt(m, cipher);
        BIGNUMToVector(cipher, encryptVector);
        IV_encrypt = encryptVector;
        dataHash.insert(dataHash.end(), encryptVector.begin(), encryptVector.end());
        BN_clear_free(m);
        BN_clear_free(cipher);
    }
    return dataHash;
}

vector<uint8_t> rsa::decryptCBCmode(vector<uint8_t> data, size_t imageNumBytes)
{
    vector<uint8_t> dataClean;
    size_t blockSize = floor(_keySize / 8);
    vector<uint8_t> IV_decrypt = IV;
    for (size_t i = 0; i < data.size(); i += blockSize)
    {
        vector<uint8_t> decryptVector;
        decryptVector.resize(blockSize - 1);
        size_t blockLength = min(blockSize, data.size() - i);
        vector<uint8_t> block(data.begin() + i, data.begin() + i + blockLength);
        BIGNUM* m = BN_new(), * plain = BN_new();
        vectorToBIGNUM(block, m);
        decrypt(m, plain);
        BIGNUMToVector(plain, decryptVector);
        while (IV_decrypt.size() != decryptVector.size())
        {
            IV_decrypt.pop_back();
        }
        transform(decryptVector.begin(), decryptVector.end(), IV_decrypt.begin(), decryptVector.begin(), [](auto a, auto b) {return a ^ b;});
        dataClean.insert(dataClean.end(), decryptVector.begin(), decryptVector.end());
        IV_decrypt = block;
        BN_clear_free(m);
        BN_clear_free(plain);
    }
    if (dataClean.size() > imageNumBytes)
    {
        size_t bytesToErase = dataClean.size() - imageNumBytes;
        dataClean.erase(dataClean.end() - blockSize + 1, dataClean.end() - blockSize + 1 + bytesToErase);
    }
    return dataClean;
}

vector<uint8_t> rsa::encryptLibrarymode(vector<uint8_t> data)
{
    RSA* publicKey = RSA_new();
    RSA_set0_key(publicKey, _n, _publicKey, NULL);
    const uint8_t* inputData = data.data();
    size_t inputSize = data.size();
    const int encryptedDataSize = RSA_size(publicKey);
    std::vector<uint8_t> encryptedData;
    const int blockSize = RSA_size(publicKey) - 11;
    for (size_t i = 0; i < inputSize; i += blockSize) {
        int remainingBytes = inputSize - i;
        int currentBlockSize = remainingBytes > blockSize ? blockSize : remainingBytes;
        std::vector<uint8_t> currentBlock(currentBlockSize);
        memcpy(currentBlock.data(), inputData + i, currentBlockSize);
        std::vector<uint8_t> encryptedBlock(encryptedDataSize);
        int encryptedSize = RSA_public_encrypt(currentBlockSize, currentBlock.data(), encryptedBlock.data(), publicKey, RSA_PKCS1_PADDING);
        if (encryptedSize == -1) {
            char buf[128];
            cerr << "RSA_public_encrypt: " << ERR_error_string(ERR_get_error(), buf) << endl;
        }
        encryptedData.insert(encryptedData.end(), encryptedBlock.begin(), encryptedBlock.begin() + encryptedSize);
    }
    //RSA_free(publicKey);
    return encryptedData;
}

vector<uint8_t> rsa::decryptLibrarymode(vector<uint8_t> data)
{
    RSA* privateKey = RSA_new();
    RSA_set0_key(privateKey, _n, _publicKey, _privateKey);
    const uint8_t* inputData = data.data();
    size_t inputSize = data.size();
    const int decryptedDataSize = RSA_size(privateKey);
    std::vector<uint8_t> decryptedData;
    const int blockSize = RSA_size(privateKey);
    for (size_t i = 0; i < inputSize; i += blockSize) {
        int remainingBytes = inputSize - i;
        int currentBlockSize = remainingBytes > blockSize ? blockSize : remainingBytes;
        std::vector<uint8_t> currentBlock(currentBlockSize);
        memcpy(currentBlock.data(), inputData + i, currentBlockSize);
        std::vector<uint8_t> decryptedBlock(decryptedDataSize);
        int decryptedSize = RSA_private_decrypt(currentBlockSize, currentBlock.data(), decryptedBlock.data(), privateKey, RSA_PKCS1_PADDING);
        if (decryptedSize == -1) {
            char buf[128];
            cerr << "RSA_public_encrypt: " << ERR_error_string(ERR_get_error(), buf) << endl;
        }
        decryptedData.insert(decryptedData.end(), decryptedBlock.begin(), decryptedBlock.begin() + decryptedSize);
    }
    //RSA_free(privateKey);
    return decryptedData;
}

size_t rsa::getKeyLength()
{
    return _keySize;
}



