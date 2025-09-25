#include <iostream>
#include <vector>
#include <string>

class MyClass {
private:
    int privateVar;
public:
    MyClass(int val) : privateVar(val) {}
    
    void display() const {
        std::cout << "Value: " << privateVar << std::endl;
    }
    
    template<typename T>
    T process(T input) {
        return input * 2;
    }
};

namespace Math {
    const double PI = 3.14159;
    
    double calculate_area(double radius) {
        return PI * radius * radius;
    }
}

int main() {
    // Variables and types
    int x = 42;
    double y = 3.14;
    char c = 'A';
    bool flag = true;
    std::string message = "Hello, World!";
    
    // Pointers and references
    int* ptr = &x;
    int& ref = x;
    
    // Arrays and vectors
    int arr[5] = {1, 2, 3, 4, 5};
    std::vector<int> vec = {1, 2, 3, 4, 5};
    
    // Control structures
    if (x > 0 && y < 10.0) {
        std::cout << "Positive x" << std::endl;
    } else if (x == 0) {
        std::cout << "Zero x" << std::endl;
    } else {
        std::cout << "Negative x" << std::endl;
    }
    
    // Loops
    for (int i = 0; i < 10; ++i) {
        if (i == 5) continue;
        std::cout << i << " ";
    }
    std::cout << std::endl;
    
    int j = 0;
    while (j < 5) {
        std::cout << j << " ";
        j++;
    }
    std::cout << std::endl;
    
    do {
        std::cout << j << " ";
        j--;
    } while (j > 0);
    std::cout << std::endl;
    
    // Operators
    int a = 10, b = 3;
    int sum = a + b;
    int diff = a - b;
    int product = a * b;
    int quotient = a / b;
    int remainder = a % b;
    
    // Bitwise operations
    int bit_and = a & b;
    int bit_or = a | b;
    int bit_xor = a ^ b;
    int bit_not = ~a;
    int left_shift = a << 2;
    int right_shift = a >> 1;
    
    // Compound assignment
    a += 5;
    b *= 2;
    
    // Comparison
    bool is_equal = (a == b);
    bool not_equal = (a != b);
    bool less_than = (a < b);
    bool greater_than = (a > b);
    
    // Logical operators
    bool logical_and = (a > 0 && b > 0);
    bool logical_or = (a == 0 || b == 0);
    bool logical_not = !(a == b);
    
    // Ternary operator
    int max_val = (a > b) ? a : b;
    
    // Function calls
    MyClass obj(42);
    obj.display();
    
    double area = Math::calculate_area(5.0);
    std::cout << "Area: " << area << std::endl;
    
    // Exception handling
    try {
        if (x < 0) {
            throw std::runtime_error("Negative value");
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    
    // Lambda expression
    auto lambda = [](int n) -> int { return n * n; };
    int squared = lambda(5);
    
    return 0;
}