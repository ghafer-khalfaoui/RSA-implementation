#include <iostream>
#include <string>
#include <vector>
#include <NTL/ZZ.h>

using namespace std;
using namespace NTL;

/* ===================== KEY GENERATION ===================== */
void key_gen(ZZ &n, ZZ &e, ZZ &d) {
    ZZ p, q, phi;

    // SMALL PRIMES FOR MSIEVE COMPATIBILITY
    GenPrime(p, 128);
    GenPrime(q, 128);

    n = p * q;
    phi = (p - 1) * (q - 1);

    cout << "enter an odd number greater than 2:\n";
    cout << "default 65537\n";
    cin >> e;

    while (e < 3 || GCD(e, phi) != 1) {
        cout << "e must be coprime to phi and greater than 2\n";
        cin >> e;
    }

    InvMod(d, e, phi);
}

/* ===================== RSA CORE ===================== */
ZZ enc_func(ZZ text, ZZ e, ZZ n) {
    ZZ c;
    PowerMod(c, text, e, n);
    return c;
}

ZZ dec_func(ZZ c, ZZ d, ZZ n) {
    ZZ text;
    PowerMod(text, c, d, n);
    return text;
}

/* ===================== TEXT <-> NUMBER ===================== */
ZZ text_to_number(const string &s) {
    ZZ res;
    ZZFromBytes(res, (const unsigned char*)s.data(), s.size());
    return res;
}

string number_to_text(const ZZ &z) {
    long len = NumBytes(z);
    vector<unsigned char> buf(len);
    BytesFromZZ(buf.data(), z, len);
    return string(buf.begin(), buf.end());
}

/* ===================== BLOCK HANDLING ===================== */
long max_block_size(const ZZ &n) {
    ZZ limit = conv<ZZ>(1);  // <--- FIXED
    long k = 0;
    while (limit * 256 < n) {
        limit *= 256;
        k++;
    }
    return k;
}

vector<ZZ> encrypt_text(const string &msg, ZZ e, ZZ n) {
    vector<ZZ> blocks;
    long k = max_block_size(n);

    for (size_t i = 0; i < msg.size(); i += k) {
        string part = msg.substr(i, k);
        ZZ m = text_to_number(part);
        blocks.push_back(enc_func(m, e, n));
    }
    return blo
