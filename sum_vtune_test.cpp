#include <cstdlib>
#include <windows.h>

float sum_naive(float* arr, int n) {
    float sum = 0.0f;
    for (int i = 0; i < n; i++) {
        sum += arr[i];
    }
    return sum;
}

float sum_two_way(float* arr, int n) {
    float sum1 = 0.0f, sum2 = 0.0f;
    for (int i = 0; i < n; i += 2) {
        sum1 += arr[i];
        if (i + 1 < n) sum2 += arr[i + 1];
    }
    return sum1 + sum2;
}

float sum_four_way(float* arr, int n) {
    float sum1=0, sum2=0, sum3=0, sum4=0;
    int i;
    for (i = 0; i < n - 3; i += 4) {
        sum1 += arr[i]; sum2 += arr[i+1]; sum3 += arr[i+2]; sum4 += arr[i+3];
    }
    for (; i < n; i++) sum1 += arr[i];
    return sum1 + sum2 + sum3 + sum4;
}

float sum_eight_way(float* arr, int n) {
    float sum1=0, sum2=0, sum3=0, sum4=0, sum5=0, sum6=0, sum7=0, sum8=0;
    int i;
    for (i = 0; i < n - 7; i += 8) {
        sum1 += arr[i]; sum2 += arr[i+1]; sum3 += arr[i+2]; sum4 += arr[i+3];
        sum5 += arr[i+4]; sum6 += arr[i+5]; sum7 += arr[i+6]; sum8 += arr[i+7];
    }
    for (; i < n; i++) sum1 += arr[i];
    return sum1 + sum2 + sum3 + sum4 + sum5 + sum6 + sum7 + sum8;
}

float sum_sixteen_way(float* arr, int n) {
    float sum1=0, sum2=0, sum3=0, sum4=0, sum5=0, sum6=0, sum7=0, sum8=0,
          sum9=0, sum10=0, sum11=0, sum12=0, sum13=0, sum14=0, sum15=0, sum16=0;
    int i;
    for (i = 0; i < n - 15; i += 16) {
        sum1 += arr[i]; sum2 += arr[i+1]; sum3 += arr[i+2]; sum4 += arr[i+3];
        sum5 += arr[i+4]; sum6 += arr[i+5]; sum7 += arr[i+6]; sum8 += arr[i+7];
        sum9 += arr[i+8]; sum10 += arr[i+9]; sum11 += arr[i+10]; sum12 += arr[i+11];
        sum13 += arr[i+12]; sum14 += arr[i+13]; sum15 += arr[i+14]; sum16 += arr[i+15];
    }
    for (; i < n; i++) sum1 += arr[i];
    return sum1+sum2+sum3+sum4+sum5+sum6+sum7+sum8+sum9+sum10+sum11+sum12+sum13+sum14+sum15+sum16;
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
    const int ARRAY_SIZE = 16 * 1024 * 1024; 
    const int REPEAT = 20000;              

    float* arr = alloc_array(ARRAY_SIZE);
    init_array(arr, ARRAY_SIZE);

    __asm__ volatile ("nop"); 
    for (int r = 0; r < REPEAT; r++) {
        sum_naive(arr, ARRAY_SIZE);
    }

    free(arr);
    return 0;
}