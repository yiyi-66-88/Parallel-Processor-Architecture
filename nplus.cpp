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
    float rel_error = abs_error / (fabs(res_naive) + 1e-9); // 避免除0
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
    int test_sizes[] = {1024, 4096, 8191, 16384}; 
    int repeat = 1000; 

    for (int n : test_sizes) {
        cout << "\n==================== n = " << n << " ====================" << endl;
        float* arr = alloc_array(n);
        init_array(arr, n);

        float res_naive = sum_naive(arr, n);
        float res_two = sum_two_way(arr, n);
        float res_four = sum_four_way(arr, n);
        float res_recur = sum_recursive_iter(arr, n);

        cout << "✅ 基础结果验证：" << endl;
        cout << "  平凡算法结果：" << res_naive << endl;
        compare_precision(res_naive, res_two, "两路累加");
        compare_precision(res_naive, res_four, "4路展开");
        compare_precision(res_naive, res_recur, "递归迭代");

        cout << "\n⚡ 性能测试（平均执行时间，单位：秒）：" << endl;
        double t_naive = timeit(sum_naive, arr, n, repeat);
        double t_two = timeit(sum_two_way, arr, n, repeat);
        double t_four = timeit(sum_four_way, arr, n, repeat);
        double t_recur = timeit(sum_recursive_iter, arr, n, repeat);

        cout << fixed << setprecision(8);
        cout << "  平凡算法：" << t_naive << endl;
        cout << "  两路累加：" << t_two << "（提升" << t_naive/t_two << "倍）" << endl;
        cout << "  4路展开：" << t_four << "（提升" << t_naive/t_four << "倍）" << endl;
        cout << "  递归迭代：" << t_recur << "（提升" << t_naive/t_recur << "倍）" << endl;

        free_array(arr);
    }

    return 0;
}