#include<iostream>
#include<cstring>

using namespace std;

int main() {
    char *G_str = new char [128], *fr = new char[256];
    bool *G, *serialised;

    cout << "Enter frame: ";
    cin >> fr;

    cout << "Enter generator polynomial: ";
    cin >> G_str;

    // Generator polynomial from input
    int L_G = strlen(G_str), L_f = strlen(fr);
    G = new bool [L_G], serialised = new bool [L_f+L_G-1];
    for (int i=0; i<L_G; i++) G[i] = G_str[i] - '0';
    for (int i=0; i<L_f; i++) serialised[i] = fr[i] - '0';

    // Calculating CRC 
    // Appending L_G-1 zeroes 
    for (int i=L_f; i<L_f+L_G; i++) serialised[i] = 0;
    bool *CRC = new bool [L_G];
    cout << endl << endl;
    int c_i;
    if (serialised[0]) for (c_i=0; c_i<L_G-1; c_i++) CRC[c_i] = serialised[c_i+1] ^ G[c_i+1];
    else for (c_i=0; c_i<L_G-1; c_i++) CRC[c_i] = serialised[c_i+1];
    CRC[c_i] = serialised[c_i+1];

    for (int i=0; i<L_G; i++) cout << CRC[i];
    cout << endl;
    
    for (int j=c_i+2; j<=L_G+L_f-1; j++) {
        int c;
        if (CRC[0]) {
            for (c=0; c<L_G-1; c++) CRC[c] = CRC[c+1] ^ G[c+1];
        } else {
            for (c=0; c<L_G-1; c++) CRC[c] = CRC[c+1];
        };
        CRC[c] = serialised[j];
        for (int i=0; i<L_G; i++) cout << CRC[i];
        cout << endl;
    };


    return 0;
};