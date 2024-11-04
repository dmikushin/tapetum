#include <stdio.h>

// Define a structure
struct Test {
    int a;
    float b;
};

// Test serialization and deserialization of a structure
int test_serialize_deserialize() {
    struct Test val = {10, 20.5};

    #pragma serialize(from : val, to: json)
    char* json;

    printf("Serialized JSON: %s\n", json);

    struct Test new_val;
    #pragma deserialize(from: json, to: new_val)

    printf("Deserialized structure: a = %d, b = %f\n", new_val.a, new_val.b);
    return 1;
}

int test(int a, float b) {
    printf("Function called with deserialized arguments: a = %d, b = %f\n", a, b);
    return 1;
}

// Test calling a function from serialized arguments
void test_function_call() {
    const char* json = "{\"a\": 10, \"b\": 20.5}";

    #pragma deserialize(from: json)
    int ret = test_serialize_deserialize();
}

// Test serialization of function arguments
void test_function_args(int a, float b) {
    printf("Function called with: a = %d, b = %f\n", a, b);
}

int main() {
    test_serialize_deserialize();
    test_function_call();

    // Test serialization of function arguments
    #pragma serialize(to: json)
    test_function_args(10, 20.5);

    return 0;
}

