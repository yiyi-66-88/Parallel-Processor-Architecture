#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <windows.h>
#include <cerrno>

using namespace std;

float* alloc_array(int n) {
    float* arr = (float*)malloc(n * sizeof(float));
    if (arr == nullptr) {
        cerr << "错误：内存分配失败！n=" << n << "，原因：" << strerror(errno) << endl;
        exit(EXIT_FAILURE); 
    }
    return arr;
}

void free_array(float* arr) {
    if (arr != nullptr) {
        free(arr);
        arr = nullptr; 
    }
}

void init_array(float* arr, int n) {
    for (int i = 0; i < n; i++) {
        arr[i] = (float)i + 0.1f; 
    }
}

float sum_naive(float* arr, int n) {
    float sum = 0.0f;
    for (int i = 0; i < n; i++) {
        sum += arr[i];
    }
    return sum;
}

float sum_sixteen_way(float* arr, int n) {
    float sum1=0, sum2=0, sum3=0, sum4=0, 
          sum5=0, sum6=0, sum7=0, sum8=0,
          sum9=0, sum10=0, sum11=0, sum12=0,
          sum13=0, sum14=0, sum15=0, sum16=0;
    int i;

    for (i = 0; i < n - 15; i += 16) {
        sum1 += arr[i];    sum2 += arr[i+1];  sum3 += arr[i+2];  sum4 += arr[i+3];
        sum5 += arr[i+4];  sum6 += arr[i+5];  sum7 += arr[i+6];  sum8 += arr[i+7];
        sum9 += arr[i+8];  sum10 += arr[i+9]; sum11 += arr[i+10]; sum12 += arr[i+11];
        sum13 += arr[i+12]; sum14 += arr[i+13]; sum15 += arr[i+14]; sum16 += arr[i+15];
    }

    for (; i < n; i++) {
        sum1 += arr[i];
    }

    return sum1 + sum2 + sum3 + sum4 + sum5 + sum6 + sum7 + sum8 +
           sum9 + sum10 + sum11 + sum12 + sum13 + sum14 + sum15 + sum16;
}

double time_algorithm(float (*func)(float*, int), float* arr, int n, int repeat) {
    LARGE_INTEGER freq, start, end;

    if (!QueryPerformanceFrequency(&freq)) {
        cerr << "错误：无法获取系统时钟频率！" << endl;
        exit(EXIT_FAILURE);
    }

    func(arr, n);

    QueryPerformanceCounter(&start);
    for (int r = 0; r < repeat; r++) {
        func(arr, n);
    }
    QueryPerformanceCounter(&end);

    double total_time = (double)(end.QuadPart - start.QuadPart) / freq.QuadPart;
    return total_time / repeat;
}

int get_repeat_count(int n) {
    if (n < 1000) {
        return 1000;  
    } else if (n < 100000) {
        return 100;    
    } else {
        return 20;     
    }
}

int main() {
    int n_list[] = {256, 1024, 4096, 16384, 65536, 262144, 1048576};
    int n_count = sizeof(n_list) / sizeof(int);

    cout << "问题规模n,算法类型,重复次数,单次平均时间(秒)" << endl;

    for (int i = 0; i < n_count; i++) {
        int n = n_list[i];
        int repeat = get_repeat_count(n);
        float* arr = alloc_array(n); 
        init_array(arr, n);          

        double t_naive = time_algorithm(sum_naive, arr, n, repeat);
        cout << n << ",平凡算法," << repeat << "," << fixed << setprecision(8) << t_naive << endl;

        double t_sixteen = time_algorithm(sum_sixteen_way, arr, n, repeat);
        cout << n << ",16路展开," << repeat << "," << fixed << setprecision(8) << t_sixteen << endl;

        free_array(arr);
    }

    cout << "\n✅ 性能测试完成！数据已输出为CSV格式，可直接复制到Excel分析。" << endl;
    return 0;
}