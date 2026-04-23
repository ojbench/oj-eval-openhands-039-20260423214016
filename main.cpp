#include "CSRMatrix.hpp"
#include <iostream>

int main() {
    // Simple test
    sjtu::CSRMatrix<int> mat(3, 3);
    
    mat.set(0, 0, 1);
    mat.set(1, 1, 2);
    mat.set(2, 2, 3);
    
    std::cout << "Element (0,0): " << mat.get(0, 0) << std::endl;
    std::cout << "Element (1,1): " << mat.get(1, 1) << std::endl;
    std::cout << "Element (2,2): " << mat.get(2, 2) << std::endl;
    std::cout << "Non-zero count: " << mat.getNonZeroCount() << std::endl;
    
    return 0;
}
