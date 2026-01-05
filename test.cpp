#include <iostream>
#include <string>
#include <NTL/ZZ.h>

using namespace std;
using namespace NTL;

/* ===================== KEY GENERATION ===================== */
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

/* ===================== ENCRYPT / DECRYPT ===================== */
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

/* ===================== TEXT CONVERSION ===================== */
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

/* ===================== MAIN ===================== */
int main() {
    ZZ n, e, d, cipher, decrypted;
    string msg;

    int password = 123;
    bool key = false;

    while (true) {
        cout << "\nRSA by Dhafer\n";
        cout << "1 : generate a key\n";
        cout << "2 : encryption using the key\n";
        cout << "3 : decryption using the key\n";
        cout << "4 : RSA cryptanalysis (msieve)\n";
        cout << "5 : leave\n";
        cout << "Choose an option: ";

        int x;
        cin >> x;
        cin.ignore();

        /* ===== OPTION 1 ===== */
        if (x == 1) {
            key_gen(n, e, d);
            key = true;

            cout << "n = " << n << endl;
            cout << "e = " << e << endl;

            int y = 3, pass;
            while (y != 0) {
                cout << "for private key type the password to see it: ";
                cin >> pass;
                if (pass == password) {
                    cout << "private key d = " << d << endl;
                    break;
                } else {
                    cout << "wrong password\n";
                    y--;
                }
            }
        }

        /* ===== OPTION 2 ===== */
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

        /* ===== OPTION 3 ===== */
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

        /* ===== OPTION 4 : RSA CRYPTANALYSIS ===== */
        else if (x == 4) {
            ZZ n_pub, e_pub, p, q, phi, d_recovered, c;

            cout << "\n--- RSA Cryptanalysis using msieve ---\n";
            cout << "Enter public modulus n: ";
            cin >> n_pub;

            cout << "Enter public exponent e: ";
            cin >> e_pub;

            cout << "Enter factor p (from msieve): ";
            cin >> p;

            cout << "Enter factor q (from msieve): ";
            cin >> q;

            if (p * q != n_pub) {
                cout << "Error: p * q != n\n";
                continue;
            }

            phi = (p - 1) * (q - 1);
            InvMod(d_recovered, e_pub, phi);

            cout << "Recovered private key d = " << d_recovered << endl;

            cout << "Enter ciphertext to decrypt: ";
            cin >> c;

            ZZ recovered_msg = dec_func(c, d_recovered, n_pub);
            cout << "Recovered plaintext: "
                 << number_to_text(recovered_msg) << endl;
        }

        /* ===== OPTION 5 ===== */
        else if (x == 5) {
            break;
        }

        else {
            cout << "wrong input\n";
        }
    }

    return 0;
}
