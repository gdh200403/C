#include <cstdint>
#include <iostream>
#include <fstream>
#include <bitset>


#define LENGTH 1
#define MAXLEN 100
#define STUDENT_ID_LAST_DIGIT 4



int16_t lab1(int16_t n) {
    // initialize
    int16_t count = 0;
    int16_t mask = 1;

    // calculation
    if (n & mask) { // n is odd
        while (n) {
            if (!(n & mask)) {
                ++count;
            }
            n--;
        }
    } else { // n is even
        n = ~n + 1; // get 2's complement of -n
        while (n) {
            if (!(n & mask)) {
                ++count;
            }
            n--;
        }
    }

    // add the last digit of student ID
    count += STUDENT_ID_LAST_DIGIT;

    // return value
    return count;
}

int16_t lab2(int16_t n) {
    // initialize
    int16_t f = 3;
    int16_t direction = 1;

    // calculation
    for (int16_t i = 1; i < n; ++i) {
        if (direction == 1) {
            f = f + f + 2;
        } else {
            f = f + f - 2;
        }

        // check if f is divisible by 8
        int16_t temp = f;
        int16_t flag = 0;
        while (temp > 0) {
            if (temp & 7 == 0) {
                flag = 1;
                break;
            }
            temp = temp - 8;
        }

        // check if f contains the digit '8'
        temp = f;
        // calculate t1
        int16_t t1 = temp;
        while (t1 >= 1000) {
            t1 -= 1000;
        }

        // check if t1 is in the range 800~899
        if (t1 >= 800 && t1 < 900) {
            flag = 1;
        } else {
            // calculate t2
            int16_t t2 = t1;
            while (t2 >= 100) {
                t2 -= 100;
            }

            // check if t2 is in the range 80~89
            if (t2 >= 80 && t2 < 90) {
                flag = 1;
            } else {
                // calculate t3
                int16_t t3 = t2;
                while (t3 >= 10) {
                    t3 -= 10;
                }

                // check if t3 is 8
                if (t3 == 8) {
                    flag = 1;
                }
            }
        }
        if (flag == 1) {
            direction = -direction;
        }
    }

    // return value
    return f;
}

int16_t lab3(char s1[], char s2[], int input_cnt, char my_input[10][MAXLEN]) {
    // initialize
    int16_t result = 0;
    int count = 0;

    // calculation
    for (int i = 0; i < input_cnt; ++i) {
        int j = 0;
        while (s1[j] == my_input[i][j] && s1[j] != '\0' && my_input[i][j] != '\0') {
            ++j;
        }
        if (s1[j] == '\0' && my_input[i][j] == '\0') {
            result = 1; // "righ"
            break;
        } else {
            j = 0;
            while (s2[j] == my_input[i][j] && s2[j] != '\0' && my_input[i][j] != '\0') {
                ++j;
            }
            if (s2[j] == '\0' && my_input[i][j] == '\0') {
                continue;
            } else {
                result = -1; // "wron"
                return result;
            }
        }
    }

    // return value
    return result;
}


void R(int16_t memory[], int16_t &move, int16_t n) {
    if (n == 0) {
        return;
    } else if (n == 1) {
        memory[move] = 1;
        ++move;
    } else {
        R(memory, move, n - 2);
        int16_t temp = 1;
        for (int16_t i = 0; i < n; ++i) {
            temp += temp;
        }
        memory[move] = temp - 1;
        ++move;
        P(memory, move, n - 2);
        R(memory, move, n - 1);
    }
}

void P(int16_t memory[], int16_t &move, int16_t n) {
    if (n == 0) {
        return;
    } else if (n == 1) {
        memory[move] = 0;
        ++move;
    } else {
        P(memory, move, n - 1);
        int16_t temp = 1;
        for (int16_t i = 0; i < n - 1; ++i) {
            temp += temp;
        }
        memory[move] = temp - 1;
        ++move;
        R(memory, move, n - 2);
        P(memory, move, n - 1);
    }
}

int16_t lab4(int16_t memory[], int16_t n) {
    int16_t move = 0;
    R(memory, move, n);
    return move;
}


int main()
{
    std::fstream file;
    file.open("test.txt", std::ios::in);



    // lab1
    int16_t n = 0;
    std::cout << "===== lab1 =====" << std::endl;
    for (int i = 0; i < LENGTH; ++i) {
        file >> n;
        std::cout << lab1(n) << std::endl;
    }

    // lab2
    std::cout << "===== lab2 =====" << std::endl;
    for (int i = 0; i < LENGTH; ++i) {
        file >> n;
        std::cout << lab2(n) << std::endl;
    }

    // lab3
    std::cout << "===== lab3 =====" << std::endl;
    char passwd[MAXLEN]; char verify[MAXLEN];
    int input_cnt=-1;
    char my_input[10][MAXLEN];
    for (int i = 0; i < LENGTH; ++i) {
        file >> passwd >> verify;
        file >> input_cnt;
        for (int j=0; j< input_cnt; j++)
        {
            file >> my_input[j];
        }
        std::cout << lab3(passwd, verify , input_cnt, my_input) << std::endl;
    }
    
    // lab4
    std::cout << "===== lab4 =====" << std::endl;
    int16_t memory[MAXLEN], move;
    for (int i = 0; i < LENGTH; ++i) {
        file >> n;
        int16_t state = 0;
        move = lab4(memory, n);
        for(int j = 0; j < move; ++j){
            std::cout << std::bitset<16>(memory[j]) << std::endl;
        }
    }



    return 0;
}