#include<iostream>
#include<cstring>
#include<cstdlib>

using namespace std;

inline int pow2 (int x) {
    return (1 << x);
};

bool* calc_CRC (bool* serialised, bool* G, int L_data, int L_G) {
    // Calculating CRC 
    // Appending L_G-1 zeroes 
    for (int i=L_data; i<L_data+L_G; i++) serialised[i] = 0;
    bool *CRC = new bool [L_G];
    int c_i;
    if (serialised[0]) for (c_i = 0; c_i<L_G-1; c_i++) CRC[c_i] = serialised[c_i+1] ^ G[c_i+1];
    else for (c_i = 0; c_i<L_G-1; c_i++) CRC[c_i] = serialised[c_i+1];
    CRC[c_i] = serialised[c_i+1];
    for (int j=c_i+2; j<=L_G+L_data-1; j++) {
        int c;
        if (CRC[0]) {
            for (c=0; c<L_G-1; c++) CRC[c] = CRC[c+1] ^ G[c+1];
        } else {
            for (c=0; c<L_G-1; c++) CRC[c] = CRC[c+1];
        };
        CRC[c] = serialised[j];
    };
    return CRC;
};

int main() {
    srand(1);
    char *str = new char[256], *G_str = new char [128], *reconstructed_str = new char[256]();
    bool **block, **p_block, *serialised, *G, *received_frame, **reconstr_p_block, **reconstr_block;
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

    cout << endl;

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
    serialised = new bool [m_h*row_count+L_G];
    for (int i=1; i<=m_h; i++)
        for (int k=0; k<row_count; k++)
            serialised[row_count*(i-1)+k] = p_block[k][i];
    cout << "Data bits after column-wise serialisation:" << endl;
    for (int i=0; i<m_h*row_count; i++) cout << serialised[i];
    cout << endl << endl;

    // Calculating CRC after filling zeroes after data
    bool* CRC = calc_CRC (serialised, G, m_h*row_count, L_G);

    // Writing CRC checksum in palce of appended zeroes
    for (int i=m_h*row_count, j=0; i<m_h*row_count+L_G; i++, j++) serialised[i] = CRC[j];
        cout << "Data bits after appending CRC checksum (sent frame):" << endl;
    for (int i=0; i<m_h*row_count; i++) cout << serialised[i];
    cout << "\e[1;36m";
    for (int i=m_h*row_count; i<m_h*row_count+L_G-1; i++) cout << serialised[i];
    cout << "\e[0m" << endl << endl;

    // Copying to received frame with random errors
    received_frame = new bool[m_h*row_count+L_G-1];
    int rand_int = (int)(p * RAND_MAX);
    for (int i=0; i<m_h*row_count+L_G-1; i++) {
        if (rand() < rand_int) received_frame[i] = !serialised[i];
       else received_frame[i] = serialised[i];
    };
    cout << "Received frame: " << endl;
    for (int i=0; i<m_h*row_count+L_G-1; i++) {
        if (received_frame[i] != serialised[i]) cout << "\e[1;31m";
        cout << received_frame[i] << "\e[0m";
    };
    cout << endl << endl;

    // Calculating CRC of "received" data and checking for CRC match
    bool *CRC_received = new bool [L_G-1], *CRC_received_calc;
    // Saving received frame's CRC
    for (int i=0; i<L_G-1; i++) CRC_received[i] = received_frame[m_h*row_count+i];
    // Calculating CRC for received frame
    CRC_received_calc = calc_CRC (received_frame, G, m_h*row_count, L_G);
    // for (int i=0; i<L_G-1; i++) cout << CRC_received_calc[i];  // To view CRC checksum of received data
    // cout << endl;    
    // Checking calculated CRC against received CRC
    bool CRC_match = true;
    for (int i=0; i<L_G-1; i++) {
        if (CRC_received_calc[i] != CRC_received[i]) {
            CRC_match = false;
            break;
        };
    };
    cout << "Result of CRC checksum matching: ";
    if (CRC_match) cout << "no ";
    cout << "error detected" << endl << endl;

    // Reconstructing data block from received frame after removing CRC bits
    reconstr_p_block = new bool* [row_count];
    for (int k=0; k<row_count; k++) {
        reconstr_p_block[k] = new bool[m_h+1];
        for (int i=1; i<=m_h; i++) {
            reconstr_p_block[k][i] = received_frame[row_count*(i-1)+k];
        };
    };
    cout << "Data block after removing CRC checksum bits:" << endl;
    for (int k=0; k<row_count; k++) {
        for (int i=1; i<=m_h; i++) {
             if (received_frame[row_count*(i-1)+k] != serialised[row_count*(i-1)+k]) cout << "\e[1;31m";
            cout << reconstr_p_block[k][i] << "\e[0m";;
        };
        cout << endl;
    };
    cout << endl;

    // Error correction usign parity bits 
    for (int k=0; k<row_count; k++) {
        int curr_r, idx = 0;
        for (curr_r = 0; curr_r < r_h; curr_r++) {
            bool p_b = 0;
            for (int i = pow2(curr_r); i<=m_h; i += (pow2(curr_r)<<1)) {
                for (int j=i; j<=m_h && j<i+pow2(curr_r); j++) {
                    p_b ^= reconstr_p_block[k][j]; 
                };
            };
            idx |= (p_b << curr_r);
        };
        // cout << idx << endl;         // To print index of corrected bits 
        if (idx) reconstr_p_block[k][idx] ^= 1;
    };

    // Reconstructing data block without parity bits
    reconstr_block = new bool* [row_count];
    for (int k=0; k<row_count; k++) {
        reconstr_block[k] = new bool [m*8]();
        // Placing data bits in Hamming code 
        int curr_r = 0;
        for (int i=1, j=0; i<=m_h; i++) {
            if (i == pow2(curr_r)) {
                curr_r++; continue;
            };
            reconstr_block[k][j] = reconstr_p_block[k][i];
            j++;
        };
    };
    cout << "Data block after removing check bits:" << endl;
    for (int k=0; k<row_count; k++) {
        for (int i=0; i<m*8; i++)
            cout << reconstr_block[k][i];
        cout << endl;
    };
    cout << endl;

    // Reconstructing data string from block
    for (int k=0; k<row_count; k++) 
        for (int i=0; i<m; i++) 
            for (int j=0; j<8; j++) 
                reconstructed_str[k*m+i] |= (reconstr_block[k][8*i+7-j] << j);
    cout << "Output frame: " << reconstructed_str << endl << endl;

    return 0;
};