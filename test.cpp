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
    return blocks;
}

string decrypt_text(const vector<ZZ> &blocks, ZZ d, ZZ n) {
    string result;
    for (const ZZ &c : blocks) {
        ZZ m = dec_func(c, d, n);
        result += number_to_text(m);
    }
    return result;
}

/* ===================== MAIN ===================== */
int main() {
    ZZ n, e, d;
    bool key_ready = false;
    int password = 123;

    while (true) {
        cout << "\nRSA by Dhafer\n";
        cout << "1 : generate a key\n";
        cout << "2 : encrypt text\n";
        cout << "3 : decrypt text\n";
        cout << "4 : RSA cryptanalysis (msieve)\n";
        cout << "5 : exit\n";
        cout << "Choose: ";

        int choice;
        cin >> choice;
        cin.ignore();

        /* ===== OPTION 1 ===== */
        if (choice == 1) {
            key_gen(n, e, d);
            key_ready = true;

            cout << "Public key:\n";
            cout << "n = " << n << "\n";
            cout << "e = " << e << "\n";

            int tries = 3, pass;
            while (tries--) {
                cout << "Password to view private key: ";
                cin >> pass;
                if (pass == password) {
                    cout << "d = " << d << "\n";
                    break;
                } else {
                    cout << "Wrong password\n";
                }
            }
        }

        /* ===== OPTION 2 ===== */
        else if (choice == 2) {
            if (!key_ready) {
                cout << "Generate a key first\n";
                continue;
            }

            string msg;
            cout << "Enter message: ";
            getline(cin, msg);

            vector<ZZ> cipher = encrypt_text(msg, e, n);

            cout << "Cipher blocks:\n";
            for (const ZZ &c : cipher)
                cout << c << "\n";
        }

        /* ===== OPTION 3 ===== */
        else if (choice == 3) {
            if (!key_ready) {
                cout << "Generate a key first\n";
                continue;
            }

            int count;
            cout << "Number of cipher blocks: ";
            cin >> count;

            vector<ZZ> blocks(count);
            for (int i = 0; i < count; i++) {
                cout << "Block " << i + 1 << ": ";
                cin >> blocks[i];
            }

            cout << "Decrypted message:\n";
            cout << decrypt_text(blocks, d, n) << "\n";
        }

        /* ===== OPTION 4 (MSIEVE) ===== */
        else if (choice == 4) {
            ZZ n_pub, e_pub, p, q, phi, d_rec;
            int count;

            cout << "Enter public n: ";
            cin >> n_pub;
            cout << "Enter public e: ";
            cin >> e_pub;
            cout << "Enter factor p: ";
            cin >> p;
            cout << "Enter factor q: ";
            cin >> q;

            if (p * q != n_pub) {
                cout << "Error: p * q != n\n";
                continue;
            }

            phi = (p - 1) * (q - 1);
            InvMod(d_rec, e_pub, phi);

            cout << "Recovered d = " << d_rec << "\n";

            cout << "Number of cipher blocks: ";
            cin >> count;

            vector<ZZ> blocks(count);
            for (int i = 0; i < count; i++) {
                cout << "Block " << i + 1 << ": ";
                cin >> blocks[i];
            }

            cout << "Recovered plaintext:\n";
            cout << decrypt_text(blocks, d_rec, n_pub) << "\n";
        }

        else if (choice == 5) {
            break;
        }

        else {
            cout << "Invalid choice\n";
        }
    }

    return 0;
}
