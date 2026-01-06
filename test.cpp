#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <NTL/ZZ.h>

using namespace std;
using namespace NTL;

/* ---------- Utilities ---------- */
bool coprime(const ZZ& a, const ZZ& b)
{
    return GCD(a, b) == 1;
}

ZZ rsa_encrypt(const ZZ& m, const ZZ& e, const ZZ& n)
{
    return PowerMod(m, e, n);
}

ZZ rsa_decrypt(const ZZ& c, const ZZ& d, const ZZ& n)
{
    return PowerMod(c, d, n);
}

/* ---------- Parse space-separated ZZ ---------- */
vector<ZZ> parse_cipher_numbers_line(const string& line)
{
    vector<ZZ> out;
    istringstream iss(line);
    ZZ x;
    while (iss >> x) out.push_back(x);
    return out;
}

/* ---------- ZZ <-> HEX ---------- */
string zz_to_hex(const ZZ& x)
{
    long len = NumBytes(x);
    vector<unsigned char> buf(len);
    BytesFromZZ(buf.data(), x, len);

    ostringstream oss;
    oss << hex << setfill('0');
    for (unsigned char b : buf)
        oss << setw(2) << (int)b;
    return oss.str();
}

ZZ hex_to_zz(const string& s)
{
    ZZ x(0);
    vector<unsigned char> buf;

    for (size_t i = 0; i < s.size(); i += 2)
    {
        unsigned int byte;
        istringstream iss(s.substr(i, 2));
        iss >> hex >> byte;
        buf.push_back((unsigned char)byte);
    }

    ZZFromBytes(x, buf.data(), buf.size());
    return x;
}

string encode_hex(const vector<ZZ>& cipher)
{
    ostringstream oss;
    for (size_t i = 0; i < cipher.size(); i++)
    {
        if (i) oss << ":";
        oss << zz_to_hex(cipher[i]);
    }
    return oss.str();
}

vector<ZZ> decode_hex(const string& s)
{
    vector<ZZ> out;
    istringstream iss(s);
    string token;
    while (getline(iss, token, ':'))
        out.push_back(hex_to_zz(token));
    return out;
}

/* ---------- SYMBOL encoding (base95, reversible) ---------- */
string encode_symbols(const vector<ZZ>& cipher)
{
    string out;
    for (const ZZ& x : cipher)
    {
        long v = conv<long>(x % (95 * 95));
        char a = (char)(v / 95 + 32);
        char b = (char)(v % 95 + 32);
        out += a;
        out += b;
    }
    return out;
}

vector<ZZ> decode_symbols(const string& s)
{
    vector<ZZ> out;
    for (size_t i = 0; i + 1 < s.size(); i += 2)
    {
        long v =
            (long)(s[i] - 32) * 95 +
            (long)(s[i + 1] - 32);
        out.push_back(ZZ(v));
    }
    return out;
}

/* ---------- Main ---------- */
int main()
{
    ZZ p, q, n, phi, e, d;
    bool key_ready = false;

    while (true)
    {
        cout << "\n===== RSA MENU =====\n";
        cout << "1. Generate RSA keys\n";
        cout << "2. Encrypt integer\n";
        cout << "3. Decrypt integer\n";
        cout << "4. Encrypt sentence (numbers + HEX + symbols)\n";
        cout << "5. Decrypt sentence (numbers / HEX / symbols)\n";
        cout << "6. Exit\n";
        cout << "Choose an option: ";

        int choice;
        cin >> choice;
        cin.ignore();

        if (choice == 1)
        {
            cout << "Enter prime p: ";
            cin >> p;
            cout << "Enter prime q: ";
            cin >> q;

            n = p * q;
            phi = (p - 1) * (q - 1);

            cout << "\nn = " << n << endl;
            cout << "phi = " << phi << endl;

            cout << "\nCandidates for e: ";
            for (long i = 3; i < 300; i += 2)
                if (coprime(ZZ(i), phi)) cout << i << " ";
            cout << "\nSelect e: ";
            cin >> e;

            InvMod(d, e, phi);
            cout << "Candidate d = " << d << endl;
            cout << "Select d: ";
            cin >> d;

            key_ready = true;
        }

        else if (choice == 4)
        {
            if (!key_ready) continue;

            string msg;
            cout << "Enter sentence:\n";
            getline(cin, msg);

            vector<ZZ> cipher;
            for (char c : msg)
                cipher.push_back(rsa_encrypt(ZZ((int)c), e, n));

            cout << "\nCiphertext as numbers:\n";
            for (auto& x : cipher) cout << x << " ";
            cout << "\n\n";

            cout << "Ciphertext as HEX text:\n";
            cout << encode_hex(cipher) << "\n\n";

            cout << "Ciphertext as symbols:\n";
            cout << encode_symbols(cipher) << endl;
        }

        else if (choice == 5)
        {
            if (!key_ready) continue;

            cout << "Decrypt input type:\n";
            cout << "1. Numbers\n";
            cout << "2. HEX text\n";
            cout << "3. Symbols\n";
            cout << "Choose: ";

            int t;
            cin >> t;
            cin.ignore();

            vector<ZZ> cipher;
            string line;

            if (t == 1)
            {
                getline(cin, line);
                cipher = parse_cipher_numbers_line(line);
            }
            else if (t == 2)
            {
                getline(cin, line);
                cipher = decode_hex(line);
            }
            else if (t == 3)
            {
                getline(cin, line);
                cipher = decode_symbols(line);
            }
            else continue;

            string plain;
            for (auto& x : cipher)
                plain += (char)conv<int>(rsa_decrypt(x, d, n));

            cout << "\nDecrypted plaintext:\n";
            cout << plain << endl;
        }

        else if (choice == 6)
        {
            break;
        }
    }
    return 0;
}
