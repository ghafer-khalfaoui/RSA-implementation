#include <iostream>
#include <string>
#include <NTL/ZZ.h>

using namespace std;
using namespace NTL;

// ===== KEY GENERATION =====
void key_gen(ZZ &n, ZZ &e, ZZ &d) {
    ZZ p, q, phi;

    GenPrime(p, 512); // smaller for testing, can increase
    GenPrime(q, 512);

    n = p * q;
    phi = (p - 1) * (q - 1);

    cout << "Enter an odd number greater than 2 (default 65537): ";
    cin >> e;

    while (e < 3 || GCD(e, phi) != 1) {
        cout << "e must be coprime to phi and greater than 2. GCD = " << GCD(e, phi) << endl;
        cin >> e;
    }

    InvMod(d, e, phi);
}

// ===== ENCRYPTION =====
ZZ enc_func(const ZZ &text, const ZZ &e, const ZZ &n) {
    ZZ c;
    PowerMod(c, text, e, n);
    return c;
}

// ===== DECRYPTION =====
ZZ dec_func(const ZZ &c, const ZZ &d, const ZZ &n) {
    ZZ text;
    PowerMod(text, c, d, n);
    return text;
}

// ===== STRING TO NUMBER =====
ZZ text_to_number(const string &s) {
    ZZ res(0);
    for (unsigned char c : s) {
        res <<= 8;
        res += ZZ(c);
    }
    return res;
}

// ===== NUMBER TO STRING =====
string number_to_text(const ZZ &num) {
    ZZ temp = num;
    string s;
    while (temp != 0) {
        unsigned char c = conv<unsigned char>(temp % 256);
        s = c + s;
        temp /= 256;
    }
    return s;
}

// ===== RECOVER PRIVATE KEY =====
void recover_private_key(const ZZ &p, const ZZ &q, const ZZ &e, ZZ &d, ZZ &n) {
    ZZ phi;
    n = p * q;
    phi = (p - 1) * (q - 1);
    InvMod(d, e, phi);
}

// ===== MAIN =====
int main() {
    ZZ n, e, d, cipher, decrypted;
    string msg;
    int password = 123;
    bool key = false;

    while (true) {
        cout << "\nRSA by Dhafer\n";
        cout << "1 : generate a key\n";
        cout << "2 : encryption using the key\n";
        cout << "3 : decryption using private key\n";
        cout << "4 : decryption using msieve results (cryptanalysis)\n";
        cout << "5 : leave\n";
        cout << "Choose an option: ";

        int x;
        cin >> x;
        cin.ignore();

        if (x == 1) {
            key_gen(n, e, d);
            key = true;

            cout << "n = " << n << endl;
            cout << "e = " << e << endl;

            int pass, tries = 3;
            while (tries--) {
                cout << "Enter password to see private key: ";
                cin >> pass;
                if (pass == password) {
                    cout << "Private key d = " << d << endl;
                    break;
                } else {
                    cout << "Wrong password\n";
                }
            }
        }

        else if (x == 2) {
            if (!key) {
                cout << "Generate a key first\n";
                continue;
            }

            cout << "Type the message to encrypt: ";
            getline(cin, msg);

            ZZ msgZZ = text_to_number(msg);
            if (msgZZ >= n) {
                cout << "Error: message too long for this key. Generate a bigger key.\n";
                continue;
            }

            cipher = enc_func(msgZZ, e, n);
            cout << "Cipher text: " << cipher << endl;
        }

        else if (x == 3) {
            if (!key) {
                cout << "Generate a key first\n";
                continue;
            }

            ZZ user_cipher;
            cout << "Type the cipher to decrypt: ";
            cin >> user_cipher;

            decrypted = dec_func(user_cipher, d, n);
            string decrypted_msg = number_to_text(decrypted);
            cout << "Decrypted message: " << decrypted_msg << endl;
        }

        else if (x == 4) {
            ZZ p, q, e_attack, d_attack, n_attack, user_cipher;

            cout << "Enter p (from msieve): ";
            cin >> p;

            cout << "Enter q (from msieve): ";
            cin >> q;

            cout << "Enter public exponent e: ";
            cin >> e_attack;

            cout << "Enter cipher text: ";
            cin >> user_cipher;

            recover_private_key(p, q, e_attack, d_attack, n_attack);

            ZZ recovered = dec_func(user_cipher, d_attack, n_attack);
            cout << "Recovered number: " << recovered << endl;

            string recovered_msg = number_to_text(recovered);
            if (recovered_msg.empty()) {
                cout << "Recovered message could not be converted. Check your inputs.\n";
            } else {
                cout << "Recovered message: " << recovered_msg << endl;
            }
        }

        else if (x == 5) {
            break;
        }

        else {
            cout << "Wrong input\n";
        }
    }

    return 0;
}
