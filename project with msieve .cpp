#include <iostream>
#include <string>
#include <NTL/ZZ.h>

using namespace std;
using namespace NTL;

void key_gen(ZZ &n, ZZ &e, ZZ &d) {         
    ZZ p, q, phi;

    GenPrime(p, 1024);
    GenPrime(q, 1024);

    n = p * q;
    phi = (p - 1) * (q - 1);

    cout << "enter an odd number greater than 2 :" << endl;
    cout << "default 65537" << endl;
    cin >> e;

    while (e < 3 || GCD(e, phi) != 1) {
        cout << "e must be coprime to phi and greater than 2." << endl;
        cout << "the gcd was: " << GCD(e, phi) << endl;
        cin >> e;
    }

    InvMod(d, e, phi);
}

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

ZZ text_to_number(string i) {
    const unsigned char* p = (const unsigned char*)i.c_str();
    long length = i.length();
    ZZ res;
    ZZFromBytes(res, p, length);
    return res;
}

string number_to_text(ZZ i) {
    long length = NumBytes(i);
    unsigned char* buffer = new unsigned char[length];
    BytesFromZZ(buffer, i, length);
    string s((char*)buffer, length);
    delete[] buffer;
    return s;
}

/* ===== NEW FUNCTION FOR PART 2 (CRYPTANALYSIS) ===== */
void recover_private_key(ZZ p, ZZ q, ZZ e, ZZ &d, ZZ &n) {
    ZZ phi;
    n = p * q;
    phi = (p - 1) * (q - 1);
    InvMod(d, e, phi);
}

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
                cout << "for private key type the password to see it: ";
                cin >> pass;
                if (pass == password) {
                    cout << "private key d = " << d << endl;
                    break;
                } else {
                    cout << "wrong password\n";
                }
            }
        }

        else if (x == 2) {
            if (!key) {
                cout << "generate a key first\n";
                continue;
            }

            cout << "type the message to encrypt: ";
            getline(cin, msg);

            ZZ msgZZ = text_to_number(msg);
            cipher = enc_func(msgZZ, e, n);
            cout << "Cipher text: " << cipher << endl;
        }

        else if (x == 3) {
            if (!key) {
                cout << "generate a key first\n";
                continue;
            }

            ZZ user_cipher;
            cout << "type the cipher to decrypt: ";
            cin >> user_cipher;

            decrypted = dec_func(user_cipher, d, n);
            cout << "Decrypted message: " << number_to_text(decrypted) << endl;
        }

        /* ===== OPTION 4: RSA CRYPTANALYSIS USING MSIEVE ===== */
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
            cout << "Recovered message: " << number_to_text(recovered) << endl;
        }

        else if (x == 5) {
            break;
        }

        else {
            cout << "wrong input\n";
        }
    }

    return 0;
}
