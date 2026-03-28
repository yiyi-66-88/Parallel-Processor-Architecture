#include <iostream>
#include <cstdlib>   
#include <iomanip>   
#include <cmath>     
#include <windows.h> 

using namespace std;


float* alloc_array(int n) {
    float* arr = (float*)malloc(n * sizeof(float));
    return arr;
}

void free_array(float* arr) {
    free(arr);
}

void init_array(float* arr, int n) {
    for (int i = 0; i < n; i++) {
        arr[i] = (float)i + 0.1f;
    }
}

void compare_precision(float res_naive, float res_opt, const string& opt_name) {
    float abs_error = fabs(res_naive - res_opt);
    float rel_error = abs_error / (fabs(res_naive) + 1e-9); 
    cout << "  " << opt_name << " 绝对误差：" << abs_error << endl;
    cout << "  " << opt_name << " 相对误差：" << rel_error << endl;
}

double timeit(float (*func)(float*, int), float* arr, int n, int repeat = 100) {
    LARGE_INTEGER freq, start, end;
    QueryPerformanceFrequency(&freq);
    
    func(arr, n);

    QueryPerformanceCounter(&start);
    for (int r = 0; r < repeat; r++) {
        func(arr, n);
    }
    QueryPerformanceCounter(&end);

    double elapsed = (double)(end.QuadPart - start.QuadPart) / freq.QuadPart;
    return elapsed / repeat;
}

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
        if (i + 1 < n) { 
            sum2 += arr[i + 1];
        }
    }
    return sum1 + sum2;
}

float sum_four_way(float* arr, int n) {
    float sum1=0, sum2=0, sum3=0, sum4=0;
    int i;
    for (i = 0; i < n - 3; i += 4) {
        sum1 += arr[i];
        sum2 += arr[i+1];
        sum3 += arr[i+2];
        sum4 += arr[i+3];
    }
    for (; i < n; i++) {
        sum1 += arr[i];
    }
    return sum1 + sum2 + sum3 + sum4;
}

float sum_eight_way(float* arr, int n) {
    float sum1=0, sum2=0, sum3=0, sum4=0, sum5=0, sum6=0, sum7=0, sum8=0;
    int i;
    for (i = 0; i < n - 7; i += 8) {
        sum1 += arr[i];
        sum2 += arr[i+1];
        sum3 += arr[i+2];
        sum4 += arr[i+3];
        sum5 += arr[i+4];
        sum6 += arr[i+5];
        sum7 += arr[i+6];
        sum8 += arr[i+7];
    }
    for (; i < n; i++) {
        sum1 += arr[i];
    }
    return sum1 + sum2 + sum3 + sum4 + sum5 + sum6 + sum7 + sum8;
}

float sum_sixteen_way(float* arr, int n) {
    float sum1=0, sum2=0, sum3=0, sum4=0, 
          sum5=0, sum6=0, sum7=0, sum8=0,
          sum9=0, sum10=0, sum11=0, sum12=0,
          sum13=0, sum14=0, sum15=0, sum16=0;
    int i;
    for (i = 0; i < n - 15; i += 16) {
        sum1 += arr[i];
        sum2 += arr[i+1];
        sum3 += arr[i+2];
        sum4 += arr[i+3];
        sum5 += arr[i+4];
        sum6 += arr[i+5];
        sum7 += arr[i+6];
        sum8 += arr[i+7];
        sum9 += arr[i+8];
        sum10 += arr[i+9];
        sum11 += arr[i+10];
        sum12 += arr[i+11];
        sum13 += arr[i+12];
        sum14 += arr[i+13];
        sum15 += arr[i+14];
        sum16 += arr[i+15];
    }
    for (; i < n; i++) {
        sum1 += arr[i];
    }
    return sum1 + sum2 + sum3 + sum4 + sum5 + sum6 + sum7 + sum8 +
           sum9 + sum10 + sum11 + sum12 + sum13 + sum14 + sum15 + sum16;
}

float sum_recursive_iter(float* arr, int n) {
    if (n == 0) return 0.0f;
    float* temp = alloc_array(n);
    for (int i = 0; i < n; i++) {
        temp[i] = arr[i];
    }

    int len = n;
    while (len > 1) {
        int new_len = len / 2;
        for (int i = 0; i < new_len; i++) {
            temp[i] = temp[2*i] + temp[2*i + 1];
        }
        if (len % 2 != 0) {
            temp[new_len] = temp[len - 1];
            new_len++;
        }
        len = new_len;
    }

    float res = temp[0];
    free_array(temp);
    return res;
}

int main() {
    int n = 16384;
    int repeat = 10000;
    float* arr = alloc_array(n);
    init_array(arr, n);

    double t_naive = timeit(sum_naive, arr, n, repeat);
    double t_eight = timeit(sum_eight_way, arr, n, repeat);

    cout << "n=" << n << " 平凡算法时间：" << t_naive << "秒" << endl;
    cout << "n=" << n << " 8路展开时间：" << t_eight << "秒" << endl;

    free_array(arr);
    return 0;
}