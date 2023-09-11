#include<iostream>
#include<cstring>

using namespace std;

inline int pow2 (int x) {
    return (1 << x);
};

int main() {
    char *str = new char[256], *G_str = new char [128];
    bool **block, **p_block, *serialised, *G;
    int m, r_h, row_count;
    double p;

    cout << "Enter data string: ";
    cin.getline(str, 256);
    cout << "Enter number of data bytes in a row (m): ";
    cin >> m;
    cout << "Enter probability (p): ";
    cin >> p;
    cout << "Enter generator polynomial: ";
    cin >> G_str;

     // Padding string to length multiple of m
    int L = strlen(str);
    if (L%m) {
        int L_n = L+(m-L%m), e;
        for (e=L; e<L_n; e++) str[e] = '~';
        str[e] = '\0';
        L = L_n;
    };
    cout << "Data string after padding: " << str << endl << endl;;

    // Calculating no. of parity bits 
    for (r_h = 1; pow2(r_h) < m*8 + r_h+1; r_h++);

    cout << endl << endl << endl;

    // Arranging data into block
    row_count = L/m;
    block = new bool* [row_count];
    for (int k=0; k<row_count; k++) {
        block[k] = new bool [m*8];
        for (int i=0; i<m; i++) {
            for (int j=7; j>=0; j--) {
                block[k][8*i+7-j] = ((str[k*m+i] & (1<<j)) >> j);
            };
        };
    };
    cout << "Data block (ASCII code of m characters per row):" << endl;
    for (int k=0; k<row_count; k++) {
        for (int i=0; i<m*8; i++)
            cout << block[k][i];
        cout << endl;
    };
    cout << endl;

    // Making block with parity bits
    p_block = new bool* [row_count];
    int m_h = m*8 + r_h;
    for (int k=0; k<row_count; k++) {
        p_block[k] = new bool [m_h+1]();
        // Placing data bits in Hamming code 
        int curr_r = 0;
        for (int i=1, j=0; i<=m_h; i++) {
            if (i == pow2(curr_r)) {
                curr_r++; continue;
            };
            p_block[k][i] = block[k][j];
            j++;
        };
        // Generating parity bits in 2^r position
        for (curr_r = 0; curr_r < r_h; curr_r++) {
            for (int i = pow2(curr_r); i<=m_h; i += (pow2(curr_r)<<1)) {
                for (int j=i; j<=m_h && j<i+pow2(curr_r); j++) {
                    p_block[k][pow2(curr_r)] ^= p_block[k][j]; 
                };
            };
        };
    };
    cout << "Data block after adding check bits:" << endl;
    for (int k=0; k<row_count; k++) {
        int curr_r = 0;
        for (int i=1; i<=m_h; i++) {
            bool is_parity = (pow2(curr_r) == i);
            if (is_parity) {
                curr_r++; 
                cout << "\e[1;32m";
            };
            cout << p_block[k][i];
            if (is_parity) cout << "\e[0m";
        };
        cout << endl;
    };
    cout << endl;

    // Generator polynomial from input
    int L_G = strlen(G_str);
    G = new bool [L_G];
    for (int i=0; i<L_G; i++) G[i] = G_str[i] - '0';

    // Serialising data block in column-major order
    serialised = new bool [m_h*row_count+L_G-1];
    for (int i=1; i<=m_h; i++)
        for (int k=0; k<row_count; k++)
            serialised[row_count*(i-1)+k] = p_block[k][i];
    cout << "Data bits after column-wise serialisation:" << endl;
    for (int i=0; i<m_h*row_count; i++) 
        cout << serialised[i];
    cout << endl << endl;

    // Calculating CRC 
    // Appending L_G-1 zeroes 
    for (int i=m_h*row_count; i<m_h*row_count+L_G; i++) serialised[i] = 0;
    bool *CRC = new bool [L_G];

    int c_i;
    for (c_i=0; !serialised[c_i]; c_i++);
    for (; c_i<L_G-1; c_i++) CRC[c_i] = serialised[c_i+1] ^ G[c_i+1];
    CRC[c_i] = serialised[c_i+1];
    for (int i=0; i<L_G; i++) cout << CRC[i];
    cout << endl;
    for (int j=c_i+2; j<=L_G+m_h*row_count-1; j++) {
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
    cout << endl;
    return 0;
};