// Complex C++ program for lexer testing
// Includes all requested features: conditionals, loops, operators, etc.
#include <string>
using namespace std;
int calculate_sum(int a, int b) {
    int result = a + b;
    return result;
}

float process_data(float values[], int size) {
    float sum = 0.0;
    for (int i = 0; i < size; i++) {
        sum += values[i];
    }
    return sum / size;
}

void string_demo() {
    string greeting = "Hello, \"World\"!\nThis is a test.";
    string emoji_str = "👍 Unicode and emojis work too! 😊";
    string tab_example = "Column1\tColumn2\tColumn3";
    
    // Test escape sequences
    string escapes = "Newline:\nTab:\tBackslash:\\";
    
    // Comments should be ignored by lexer
    /* Multi-line
       comment */
}

bool validate_input(int x, float y) {
    // Conditional statements
    if (x > 100 && y < 0.0) {
        return true;
    } else if (x == 42 || y >= 100.5) {
        return false;
    } else {
        return x != 0;
    }
}

void loop_demo(int count) {
    int i = 0;
    
    // While loop
    while (i < count) {
        i++;
    }
    
    // Do-while loop
    do {
        count--;
    } while (count > 0);
    
    // For loop with complex condition
    for (int j = 0; j < 10 && j != 5; j += 2) {
        if (j == 4) {
            continue;
        }
        // Process j
    }
}

void operator_demo() {
    int a = 10;
    int b = 3;
    
    // Arithmetic operators
    int add = a + b;
    int sub = a - b;
    int mul = a * b;

    float div = a / b;
    int mod = a % b;
    
    // Comparison operators
    bool eq = (a == b);
    bool neq = (a != b);
    bool lt = (a < b);
    bool gt = (a > b);
    bool le = (a <= b);
    bool ge = (a >= b);
    
    // Logical operators
    bool and_op = (a > 5 && b < 5);
    bool or_op = (a == 10 || b == 3);
    bool not_op = !(a == b);
    
    // Assignment operators
    a += 5;
    b *= 2;
    
    // Bitwise operators
    int and_bit = a & b;
    int or_bit = a | b;
    int xor_bit = a ^ b;
    int not_bit = ~a;
    int left_shift = a << 1;
    int right_shift = a >> 1;
}

// Main function
int main() {
    // Test all features
    int x = 42;
    float y = 3.14;
    bool flag = true;
    
    string message = "Program started";
    
    if (x == 42 && flag) {
        message = "Correct value";
    } else {
        message = "Incorrect value";
    }
    
    int result = calculate_sum(x, 10);
    
    return result;
}