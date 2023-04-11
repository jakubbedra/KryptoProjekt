#include <iostream>
#include <bitset>

using namespace std;

class Hasher {
private:
    static const int n = 30;
    unsigned int a;
    unsigned int b;
    unsigned int c;
    unsigned int d;
    const unsigned int padding[32] = {
            0xB3, 0xC5, 0x44, 0x97, 0x42, 0x70, 0x9D,
            0x88, 0x1B, 0x6A, 0xCE, 0x10, 0x13, 0xA8,
            0x5F, 0x57, 0x8F, 0x0C, 0x24, 0xF1, 0x9F,
            0xE9, 0xA5, 0xCD, 0xD1, 0xDC, 0xD2, 0x6E,
            0x16, 0xA7, 0xBB, 0xE5
    };
public:
    Hasher() {
        a = 0x5AC24860;
        b = 0xDA545106;
        c = 0x716ADFDB;
        d = 0x4DA893CC;
    }

private:
    unsigned int rol(unsigned int x, int shift) {
        const unsigned int mask = (CHAR_BIT * sizeof(x) - 1);
        shift &= mask;
        return (x << shift) | (x >> (-shift & mask));
    }

    void hashBlock(unsigned int w[n]) {
        // i = 0 .. 9
        for (int i = 0; i < 10; i++) {
            if (i >= 6) {
                w[i] = rol((w[i - 6] ^ w[i - 5] ^ (w[i - 3] + w[i - 1])), 3);
            }
            //cout << hex << a << " " << b << " " << c << " " << d << " " << w[i] << endl;
            unsigned int tmp = (a & b) + (rol(c, 4) ^ (~d)) + w[i] + 0xFE887401;
            a = b;
            b = c;
            c = d;
            d = tmp;
        }
        // i = 10 .. 19
        for (int i = 10; i < 20; i++) {
            w[i] = rol((w[i - 6] ^ w[i - 5] ^ (w[i - 3] + w[i - 1])), 3);
            //cout << hex << a << " " << b << " " << c << " " << d << " " << w[i] << endl;
            unsigned int tmp = (a & b) ^ ((~a) & c) ^ (c & (rol(d, 2))) ^ w[i] ^ 0x44C38316;
            a = b;
            b = c;
            c = d;
            d = tmp;
        }
        // i = 20 .. 29
        for (int i = 20; i < 30; i++) {
            w[i] = rol((w[i - 6] ^ w[i - 5] ^ (w[i - 3] + w[i - 1])), 3);
            //cout << hex << a << " " << b << " " << c << " " << d << " " << w[i] << endl;
            unsigned int tmp = ((a ^ rol(b, 2)) ^ (rol(c, 4)) ^ (rol(d, 7))) + (w[i] ^ 0x21221602);
            a = b;
            b = c;
            c = d;
            d = tmp;
        }
    }

public:
    void hash(char *inputMsg, int length) {
        // check if padding is needed, if yes add it
        char *msg = inputMsg;
        int paddingLength = 24 - length % 24;
        int paddingCtr = 0;
        // convert char array to int array
        unsigned int *array = new unsigned int[(length + paddingLength) / 4];
        for (int i = 0; i < (length + paddingLength) / 4; i++) {
            array[i] = 0x00000000;

            unsigned int tmp0 = i * 4 + 0 < length ? static_cast<unsigned int>(msg[i * 4 + 0]) : padding[paddingCtr++];
            array[i] = array[i] | tmp0;
            array[i] = array[i] << 8;
            unsigned int tmp1 = i * 4 + 1 < length ? static_cast<unsigned int>(msg[i * 4 + 1]) : padding[paddingCtr++];
            array[i] = array[i] | tmp1;
            array[i] = array[i] << 8;
            unsigned int tmp2 = i * 4 + 2 < length ? static_cast<unsigned int>(msg[i * 4 + 2]) : padding[paddingCtr++];
            array[i] = array[i] | tmp2;
            array[i] = array[i] << 8;
            unsigned int tmp3 = i * 4 + 3 < length ? static_cast<unsigned int>(msg[i * 4 + 3]) : padding[paddingCtr++];
            array[i] = array[i] | tmp3;
        }
        // divide msg into blocks
        for (int i = 0; i < (length + paddingLength) / 24; i++) {
            unsigned int w[n] = {
                    array[i * 6], array[i * 6 + 1], array[i * 6 + 2],
                    array[i * 6 + 3], array[i * 6 + 4], array[i * 6 + 5],
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
            };
            hashBlock(w);
        }

        delete array;
    }

    unsigned int getA() {
        return a;
    }

    unsigned int getB() {
        return b;
    }

    unsigned int getC() {
        return c;
    }

    unsigned int getD() {
        return d;
    }
};

unsigned int rol2(unsigned int x, int shift) {
    const unsigned int mask = (CHAR_BIT * sizeof(x) - 1);
    shift &= mask;
    return (x << shift) | (x >> (-shift & mask));
}

const string charset = "qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM1234567890!@#$%^&*-_=+([{<)]}>'\";:?,./|";

void hashuj2() {
    const unsigned int hash2[] = {0x0F4D8C79, 0xBDAEEE50, 0x19470D10, 0xD2E2E920};
    for (int i = 0; i < charset.length(); i++) {
        for (int j = 0; j < charset.length(); j++) {
            char word[2] = {charset[i], charset[j]};
            Hasher hasher;
            hasher.hash(word, 2);
            bool found = hash2[0] == hasher.getA() && hash2[1] == hasher.getB() &&
                         hash2[2] == hasher.getC() && hash2[3] == hasher.getD();
            if (found) {
                cout << "The word was found! It is: " << word[0] << word[1] << endl;
                return;
            } else {
                //cout << word[0] << word[1] << endl;
                //cout << hasher.getA() << " " << hasher.getB() << " " << hasher.getC() << " " << hasher.getD() << endl;
            }
        }
    }
}

void hashuj3() {
    const unsigned int hash3[] = {0x602064B8, 0x39B20D8B, 0x2ABF78B6, 0xC6C43D71};
    for (int i = 0; i < charset.length(); i++) {
        for (int j = 0; j < charset.length(); j++) {
            for (int k = 0; k < charset.length(); k++) {
                char word[3] = {charset[i], charset[j], charset[k]};
                Hasher hasher;
                hasher.hash(word, 3);
                bool found = hash3[0] == hasher.getA() && hash3[1] == hasher.getB() &&
                             hash3[2] == hasher.getC() && hash3[3] == hasher.getD();
                if (found) {
                    cout << "The word was found! It is: " << word[0] << word[1] << word[2] << endl;
                    return;
                }
            }
        }
    }
}

void hashuj4() {
    const unsigned int hash3[] = {0x7D6537CF, 0xF562791F, 0x673BD230, 0xF28ED621};
    for (int i = 0; i < charset.length(); i++) {
        for (int j = 0; j < charset.length(); j++) {
            for (int k = 0; k < charset.length(); k++) {
                for (int l = 0; l < charset.length(); l++) {
                    char word[4] = {charset[i], charset[j], charset[k], charset[l]};
                    Hasher hasher;
                    hasher.hash(word, 4);
                    bool found = hash3[0] == hasher.getA() && hash3[1] == hasher.getB() &&
                                 hash3[2] == hasher.getC() && hash3[3] == hasher.getD();
                    if (found) {
                        cout << "The word was found! It is: " << word[0] << word[1] << word[2] << word[3] << endl;
                        return;
                    }
                }
            }
        }
    }
}

void hashuj5() {
    const unsigned int hash3[] = {0x14B9DA32, 0x60940AB9, 0xB9424733, 0xB53FF846};
    for (int i = 0; i < charset.length(); i++) {
        for (int j = 0; j < charset.length(); j++) {
            for (int k = 0; k < charset.length(); k++) {
                for (int l = 0; l < charset.length(); l++) {
                    for (int m = 0; m < charset.length(); m++) {
                        char word[5] = {charset[i], charset[j], charset[k], charset[l], charset[m]};
                        Hasher hasher;
                        hasher.hash(word, 5);
                        bool found = hash3[0] == hasher.getA() && hash3[1] == hasher.getB() &&
                                     hash3[2] == hasher.getC() && hash3[3] == hasher.getD();
                        if (found) {
                            cout << "The word was found! It is: " << word[0] << word[1] << word[2] << word[3] << word[4] << endl;
                            return;
                        }
                    }
                }
            }
        }
    }
}

int main() {
    if (false) {
        unsigned int a = 0x5AC24860;
        unsigned int b = 0xDA545106;
        unsigned int c = 0x716ADFDB;
        unsigned int d = 0x4DA893CC;
        unsigned int wi = 0x00010203;
        d = (a & b) + (rol2(c, 4) ^ (~d)) + wi + 0xFE887401;
        cout << hex << d << endl;
        return 2137;
    }

    cout << "Hello, World!" << endl;

    // 2
    hashuj2();
    hashuj3();
    hashuj4();
    hashuj5();

    //char msg[] = {
    //        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C,
    //        0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17
    //};
//
    //Hasher *hasher = new Hasher();
    //hasher->hash(msg, 24);
    //unsigned int *hash = hasher->getHash();
//
    //for (int i = 0; i < 4; i++) {
    //    cout << hex << hash[i] << " ";
    //}

    return 0;
}
