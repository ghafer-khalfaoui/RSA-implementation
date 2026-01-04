#include <iostream>
#include <string>
#include <NTL/ZZ.h>
using namespace std;
using namespace NTL;

void key_gen(ZZ &n, ZZ &e, ZZ &d) {         
    ZZ p;
    ZZ q;
    ZZ phi;
    GenPrime(p, 1024);
    GenPrime(q, 1024);
    n = p * q;
    phi = (p - 1) * (q - 1);
    cout<<"enter an odd number greater than 2 :"<<endl;
    cout<<"default 65537"<<endl;
    cin >> e; 
    while(e<3 || GCD(e,phi)!=1){
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
int main() {
    ZZ n;
    ZZ e;
    ZZ d;
    ZZ cipher;
    ZZ text;
    string msg;
    int password = 123;
    bool key = false;
    ZZ decrypted;
    while (true) {
        cout << "rsa by dhafer"<<endl;
        cout << "1 : generate a key"<<endl;
        cout << "2 : encryption using the key "<<endl;
        cout << "3 : Decryption "<<endl;
        cout << "4 : leave"<<endl;
        cout << "Choose an option: ";
        int x;
        int y = 3;
	int pass;
        cin >> x;
        cin.ignore();

        if (x == 1){
            key_gen(n, e, d);
            key = true;
            cout << " n = " << n << endl;
            cout << "e = " << e << endl;
            while (y!=1){
                
                cout<<"for private key type the password to see it ";
                cin >> pass ;
                if (pass == password) {
                    cout << "the private key d :" << d << endl;
		    break;
                }
                else {
                    cout<<"wrong password try again"<<endl;
                   y=y-1;
                }
            }

            
        }

        else if (x == 2) {
            if (!key){
                cout<<"gen a key first "<<endl;
                continue;
            }
            cout << " type the message to encrypt ";
            getline(cin, msg);

            ZZ msgZZ = text_to_number(msg);
            cipher = enc_func(msgZZ, e, n);
            cout << " Cipher text : " << cipher << endl;
        }

        else if (x == 3) {
            if (!key){
                cout<<"gen a key first "<<endl;
                continue;
            
	    }
	    ZZ user_cipher;
	    cout << " type the cipher to decrypt ";
            
	    cin >>user_cipher;
	    
	    

            decrypted = dec_func(user_cipher, d, n);
	    string p_text = number_to_text(decrypted);

            

            cout << "Decrypted message " << p_text << endl;
        }

        else if (x == 4) {
            break;
        }

        else {
            cout << "wrong input "<<endl;
        }
    }

    return 0;
}



