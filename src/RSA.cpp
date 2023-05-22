#include <inc/RSA.h>


rsa::rsa()
{
    _keySize = 1024;
    _private_key = BN_new();
    _public_key = BN_new();
    _n = BN_new();
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
    BN_copy(_public_key, e);
    BN_copy(_private_key, d);
    //printBignum(_private_key);
    //printBignum(_public_key);
    BN_CTX_free(ctx);
    BN_clear_free(p);
    BN_clear_free(e);
    BN_clear_free(d);
    BN_clear_free(q);

    BIGNUM* plaintext = BN_new();
    BIGNUM* ciphertext = BN_new();
    BN_set_word(plaintext, 123123);

    encrypt(plaintext, ciphertext);
    cout << BN_bn2dec(ciphertext) << endl;
    BIGNUM* answer = BN_new();

    decrypt(ciphertext, answer);


    cout << "Decrypted plaintext: " << BN_bn2dec(answer) << endl;

    BN_free(plaintext);
    BN_free(ciphertext);
    BN_free(answer);

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

void rsa::encrypt(const BIGNUM* plaintext, BIGNUM* ciphertext)
{
    BN_CTX* ctx = BN_CTX_new();

    // ciphertext = plaintext^e mod _n
    BN_mod_exp(ciphertext, plaintext, _public_key, _n, ctx);

    BN_CTX_free(ctx);
}

void rsa::decrypt(const BIGNUM* ciphertext, BIGNUM* plaintext)
{
    BN_CTX* ctx = BN_CTX_new();

    // plaintext = ciphertext^d mod _n
    BN_mod_exp(plaintext, ciphertext, _private_key, _n, ctx);

    BN_CTX_free(ctx);
}

