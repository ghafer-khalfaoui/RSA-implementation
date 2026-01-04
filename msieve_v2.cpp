#include <iostream>
#include <string>
#include <vector>
#include <limits> // Required for input clearing
#include <NTL/ZZ.h>

using namespace std;
using namespace NTL;

// ===== KEY GENERATION =====
void key_gen(ZZ &n, ZZ &e, ZZ &d) {
    ZZ p, q, phi;
    
    cout << "Generating primes... please wait." << endl;
    GenPrime(p, 512); 
    GenPrime(q, 512);

    n = p * q;
    phi = (p - 1) * (q - 1);

    cout << "Enter public exponent e (enter 0 for default 65537): ";
    cin >> e;

    if (e == 0) e = 65537;

    while (e < 3 || GCD(e, phi) != 1) {
        cout << "Error: e must be coprime to phi (GCD must be 1).\n";
        cout << "Current GCD(e, phi) = " << GCD(e, phi) << endl;
        cout << "Please enter a different e: ";
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
    for (size_t i = 0; i < s.size(); ++i) {
        res *= 256;
        res += ZZ((unsigned char)s[i]);
    }
    return res;
}

// ===== NUMBER TO STRING (FIXED) =====
string number_to_text(const ZZ &num) {
    ZZ temp = num;
    vector<unsigned char> bytes;
    if (temp == 0) return "";

    while (temp > 0) {
        // Using % operator is safer than Rem() function
        long byte_val = (long)(temp % 256); 
        bytes.push_back((unsigned char)byte_val);
        temp /= 256;
    }

    string s;
    for (int i = bytes.size() - 1; i >= 0; --i) {
        s += bytes[i];
    }
    return s;
}

// ===== RECOVER PRIVATE KEY =====
bool recover_private_key(const ZZ &p, const ZZ &q, const ZZ &e, ZZ &d, ZZ &n) {
    ZZ phi;
    n = p * q;
    phi = (p - 1) * (q - 1);
    
    if (GCD(e, phi) != 1) {
        cout << "\n[CRITICAL ERROR] Mathematical mismatch in Part 4!\n";
        cout << "The 'e' you entered is not compatible with 'p' and 'q'.\n";
        cout << "GCD(e, phi) is " << GCD(e, phi) << ", but it must be 1.\n";
        return false;
    }
    
    InvMod(d, e, phi);
    return true;
}

// ===== MAIN =====
int main() {
    ZZ n, e, d, cipher, decrypted;
    string msg;
    int password = 123;
    bool key = false;

    while (true) {
        cout << "\n=== RSA Perfected ===\n";
        cout << "1 : Generate Key\n";
        cout << "2 : Encrypt Message\n";
        cout << "3 : Decrypt (Standard)\n";
        cout << "4 : Recover Private Key (Cryptanalysis)\n";
        cout << "5 : Exit\n";
        cout << "Choose an option: ";

        int x;
        if (!(cin >> x)) {
            cout << "Invalid input! Please enter a number.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); 

        if (x == 1) {
            key_gen(n, e, d);
            key = true;
            cout << "\n[!] KEYS GENERATED [!]\n";
            cout << "n = " << n << endl;
            cout << "e = " << e << endl;

            int pass, tries = 3;
            while (tries--) {
                cout << "Enter password to see private key d: ";
                if(cin >> pass) {
                     if (pass == password) {
                        cout << "Private key d = " << d << endl;
                        break;
                    } else {
                        cout << "Wrong password.\n";
                    }
                } else {
                    cout << "Invalid number.\n";
                    cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
            }
        }
        else if (x == 2) {
            if (!key) {
                cout << "Error: Generate a key first (Option 1).\n";
                continue;
            }
            cout << "Type the message to encrypt: ";
            getline(cin, msg);
            if (msg.empty()) continue;

            ZZ msgZZ = text_to_number(msg);
            if (msgZZ >= n) {
                cout << "Error: Message too long for this key size.\n";
                continue;
            }
            cipher = enc_func(msgZZ, e, n);
            cout << "Cipher text: " << cipher << endl;
        }
        else if (x == 3) {
            if (!key) {
                cout << "Error: Generate a key first (Option 1).\n";
                continue;
            }
            ZZ user_cipher;
            cout << "Paste the cipher to decrypt: ";
            cin >> user_cipher;
            decrypted = dec_func(user_cipher, d, n);
            cout << "Decrypted message: " << number_to_text(decrypted) << endl;
        }
        else if (x == 4) {
            ZZ p_in, q_in, e_in, d_attack, n_attack, user_cipher;
            cout << "\n--- CRYPTANALYSIS MODE ---\n";
            cout << "Enter p: "; cin >> p_in;
            cout << "Enter q: "; cin >> q_in;
            cout << "Enter e: "; cin >> e_in;
            cout << "Enter cipher text: "; cin >> user_cipher;

            if (recover_private_key(p_in, q_in, e_in, d_attack, n_attack)) {
                ZZ recovered = dec_func(user_cipher, d_attack, n_attack);
                cout << "--------------------------------\n";
                cout << "Recovered Number: " << recovered << endl;
                cout << "Recovered Message: " << number_to_text(recovered) << endl;
                cout << "--------------------------------\n";
            }
        }
        else if (x == 5) break;
        else cout << "Unknown option.\n";
    }
    return 0;
}
