#include <iostream>
#include <cstring>
#include <cassert>
#include <string> // Only for std::cout formatting

// INCLUDE YOUR LIBRARY HERE
// (If you saved it as String.hpp, uncomment the line below)
#include "mystring.hpp" 

// Simple Test Framework Macros
#define ASSERT_TRUE(condition) \
    if (!(condition)) { \
        std::cerr << "   [FAIL] " << #condition << " at line " << __LINE__ << "\n"; \
        return false; \
    }

#define ASSERT_EQ_STR(actual, expected) \
    if (strcmp(actual, expected) != 0) { \
        std::cerr << "   [FAIL] Expected '" << expected << "', got '" << actual << "' at line " << __LINE__ << "\n"; \
        return false; \
    }

#define RUN_TEST(testFunc) \
    if (testFunc()) { \
        std::cout << "[PASS] " << #testFunc << "\n"; \
    } else { \
        std::cout << "[FAIL] " << #testFunc << "\n"; \
    }

// ----------------------------------------------------------------------
// TEST CASES
// ----------------------------------------------------------------------

bool Test_StringView_Basics() {
    string_view sv = "Hello World";
    
    ASSERT_TRUE(sv.size() == 11);
    ASSERT_TRUE(sv[0] == 'H');
    ASSERT_TRUE(sv.at(6) == 'W');
    ASSERT_TRUE(sv.at(100) == '\0'); // Out of bounds check
    
    // Test Comparison
    ASSERT_TRUE(sv == "Hello World");
    ASSERT_TRUE(sv != "Hello");
    
    // Test startsWith
    ASSERT_TRUE(sv.startsWith("Hello"));
    ASSERT_TRUE(!sv.startsWith("World")); // False
    
    // Test IndexOf
    ASSERT_TRUE(sv.indexOf('W') == 6);
    ASSERT_TRUE(sv.indexOf('z') == -1);

    return true;
}

bool Test_FixedString_Concat() {
    std::cout << "   [DEBUG] Creating FixedString...\n";
    FixedString<20> fstr = "Hello";
    
    // DEBUG: Check internal flags
    // You might need to make these public temporarily or add a getter to check
    std::cout << "   [DEBUG] FixedString created. Data: " << fstr.data() << "\n";
    
    std::cout << "   [DEBUG] Concatenating...\n";
    fstr.concat(" World");
    
    std::cout << "   [DEBUG] Concat done. Data: " << fstr.data() << "\n";
    
    ASSERT_EQ_STR(fstr.data(), "Hello World");
    ASSERT_TRUE(fstr.size() == 11);
    
    std::cout << "   [DEBUG] Test finished, destroying FixedString...\n";
    return true;
}

bool Test_DynamicString_Resize() {
    // Start with small capacity (min is 8)
    DynamicString dstr(8); 
    dstr = "12345";
    
    ASSERT_TRUE(dstr.capacity() >= 8);
    
    // Trigger resize by appending past capacity
    // "12345" (5) + "67890" (5) = 10 chars. 10 > 8, so it must grow.
    dstr.concat("67890");
    
    ASSERT_EQ_STR(dstr.data(), "1234567890");
    ASSERT_TRUE(dstr.capacity() >= 10); // Capacity should have doubled or increased
    ASSERT_TRUE(dstr.size() == 10);
    
    return true;
}

bool Test_DeepCopy() {
    DynamicString original = "Original";
    
    // Copy Constructor
    DynamicString copy = original;
    
    // Modify original
    original[0] = 'X';
    
    // Verify copy is NOT modified (Deep Copy check)
    ASSERT_EQ_STR(original.data(), "Xriginal");
    ASSERT_EQ_STR(copy.data(), "Original");
    
    // Verify pointers are different
    ASSERT_TRUE(original.data() != copy.data());
    
    return true;
}

bool Test_MoveSemantics() {
    DynamicString source = "MoveMe";
    const char* original_ptr = source.data();
    
    // Move Constructor
    DynamicString dest = std::move(source);
    
    // Dest should have the data
    ASSERT_EQ_STR(dest.data(), "MoveMe");
    
    // Dest should have stolen the pointer (address match)
    ASSERT_TRUE(dest.data() == original_ptr);
    
    // Source should be empty/null
    ASSERT_TRUE(source.data() == nullptr);
    ASSERT_TRUE(source.size() == 0);
    
    return true;
}

bool Test_Replace() {
    DynamicString dstr = "I like cats";
    
    // 1. Same length replacement
    dstr.replace("cats", "dogs");
    ASSERT_EQ_STR(dstr.data(), "I like dogs");
    
    // 2. Growing replacement (needs resize)
    // "dogs" (4) -> "elephants" (9)
    bool success = dstr.replace("dogs", "elephants");
    ASSERT_TRUE(success);
    ASSERT_EQ_STR(dstr.data(), "I like elephants");
    
    // 3. Shrinking replacement
    dstr.replace("elephants", "ant");
    ASSERT_EQ_STR(dstr.data(), "I like ant");
    
    return true;
}

bool Test_Polymorphism() {
    // Create a DynamicString but hold it in a base pointer
    string* polyStr = new DynamicString(8);
    *polyStr = "Base";
    
    // This should trigger DynamicString::concat (which resizes)
    // If virtual is missing, this might crash or truncate
    polyStr->concat(" is growing significantly!"); 
    
    ASSERT_EQ_STR(polyStr->data(), "Base is growing significantly!");
    
    delete polyStr;
    return true;
}
int main() {
    std::cout << "Running String Library Unit Tests...\n";
    std::cout << "------------------------------------\n";

    RUN_TEST(Test_StringView_Basics);
    RUN_TEST(Test_FixedString_Concat);
    RUN_TEST(Test_DynamicString_Resize);
    RUN_TEST(Test_DeepCopy);
    RUN_TEST(Test_MoveSemantics);
    RUN_TEST(Test_Replace);
    RUN_TEST(Test_Polymorphism);

    std::cout << "------------------------------------\n";
    std::cout << "Tests Completed.\n";
    return 0;
}