#include <cstdlib>
#include <windows.h>

float sum_two_way(float* arr, int n) {
    float sum1 = 0.0f, sum2 = 0.0f;
    for (int i = 0; i < n; i += 2) {
        sum1 += arr[i];
        if (i + 1 < n) sum2 += arr[i + 1];
    }
    return sum1 + sum2;
}

float* alloc_array(int n) {
    return (float*)malloc(n * sizeof(float));
}

void init_array(float* arr, int n) {
    for (int i = 0; i < n; i++) {
        arr[i] = (float)i + 0.1f;
    }
}

int main() {
    const int ARRAY_SIZE = 32 * 1024 * 1024; 
    const int REPEAT = 20000000;             

    float* arr = alloc_array(ARRAY_SIZE);
    init_array(arr, ARRAY_SIZE);

    volatile float sink = 0; 
    for (int r = 0; r < REPEAT; r++) {
        sink += sum_two_way(arr, ARRAY_SIZE);
    }

    free(arr);
    return (int)sink; 
}