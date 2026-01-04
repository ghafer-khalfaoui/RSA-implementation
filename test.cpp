
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <NTL/ZZ.h>

using namespace std;
using namespace NTL;

string ZZ_to_ascii(const ZZ &m_in) {
    if (m_in == 0) return string(); // empty string for zero
    ZZ m = m_in;
    vector<unsigned char> bytes;
    ZZ base = ZZ(256);
    while (m > 0) {
        ZZ rem = m % base;
        long r = conv<long>(rem); // rem < 256
        bytes.push_back(static_cast<unsigned char>(r));
        m /= base;
    }
    reverse(bytes.begin(), bytes.end());
    string s;
    s.reserve(bytes.size());
    for (unsigned char b : bytes) s.push_back(static_cast<char>(b));
    return s;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string sp, sq, se, sc;
    if (!(cin >> sp >> sq >> se >> sc)) {
        cerr << "Usage: provide p q e ciphertext as decimal strings\n";
        return 1;
    }

    // Convert inputs to NTL big integers using to_ZZ
    ZZ p = to_ZZ(sp);
    ZZ q = to_ZZ(sq);
    ZZ e = to_ZZ(se);
    ZZ c = to_ZZ(sc);

    if (p <= 1 || q <= 1) {
        cerr << "Error: p and q must be > 1\n";
        return 1;
    }

    ZZ n = p * q;
    ZZ phi = (p - 1) * (q - 1);

    ZZ g = GCD(e, phi);
    if (g != 1) {
        cerr << "Error: gcd(e, phi) != 1; gcd = " << g << "\n";
        return 1;
    }

    ZZ d = InvMod(e, phi);
    ZZ m = PowerMod(c, d, n);

    cout << "n = " << n << "\n";
    cout << "phi = " << phi << "\n";
    cout << "d = " << d << "\n";
    cout << "Plaintext (integer) = " << m << "\n";

    string ascii = ZZ_to_ascii(m);
    cout << "Plaintext (ASCII interpretation):\n";
    bool all_printable = true;
    for (unsigned char ch : ascii) {
        if (ch < 32 || ch > 126) { all_printable = false; break; }
    }
    if (!ascii.empty() && all_printable) {
        cout << ascii << "\n";
    } else if (!ascii.empty()) {
        cout << "0x";
        static const char *hex = "0123456789ABCDEF";
        for (unsigned char ch : ascii) {
            cout << hex[(ch >> 4) & 0xF] << hex[ch & 0xF];
        }
        cout << "\n";
    } else {
        cout << "(empty string or zero)\n";
    }

    return 0;
}
