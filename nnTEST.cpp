#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <iomanip>
#include <cmath>
#include <windows.h>
#include <vector>
#include <numeric> 

using namespace std;

// 测试的问题规模列表
const vector<int> N_LIST = {64, 128, 256, 1024, 2048, 4096, 16384};
const int RUN_TIMES = 5;
const int L1_CACHE_SIZE = 1126400;  
const int L2_CACHE_SIZE = 14680064; 
const int L3_CACHE_SIZE = 18874368; 


size_t calc_mat_memory(int n) {
    return n * n * sizeof(float);
}

string get_cache_range(int n) {
    size_t mem = calc_mat_memory(n);
    if (mem <= L1_CACHE_SIZE) {
        return "L1 Cache";
    } else if (mem <= L2_CACHE_SIZE) {
        return "L2 Cache";
    } else if (mem <= L3_CACHE_SIZE) {
        return "L3 Cache";
    } else {
        return "Main Memory";
    }
}


int get_repeat_count(int n) {
    if (n <= 256)      return 1000;  
    else if (n <= 4096) return 100;   
    else if (n <= 16384) return 20;   
    else               return 5;    
}

float** alloc_matrix(int n) {
    if (n <= 0) {
        cerr << "错误：矩阵维度n必须为正整数！n=" << n << endl;
        exit(EXIT_FAILURE);
    }
    float** mat = (float**)malloc(n * sizeof(float*));
    if (mat == nullptr) {
        cerr << "错误：矩阵内存分配失败！n=" << n << endl;
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < n; i++) {
        mat[i] = (float*)malloc(n * sizeof(float));
        if (mat[i] == nullptr) {
            cerr << "错误：矩阵第" << i << "行内存分配失败！n=" << n << endl;
            for (int j = 0; j < i; j++) free(mat[j]);
            free(mat);
            exit(EXIT_FAILURE);
        }
    }
    return mat;
}

float* alloc_vector(int n) {
    if (n <= 0) {
        cerr << "错误：向量维度n必须为正整数！n=" << n << endl;
        exit(EXIT_FAILURE);
    }
    float* vec = (float*)malloc(n * sizeof(float));
    if (vec == nullptr) {
        cerr << "错误：向量内存分配失败！n=" << n << endl;
        exit(EXIT_FAILURE);
    }
    return vec;
}

void free_matrix(float** mat, int n) {
    if (mat != nullptr) {
        for (int i = 0; i < n; i++) {
            if (mat[i] != nullptr) free(mat[i]);
        }
        free(mat);
    }
}

void free_vector(float* vec) {
    if (vec != nullptr) free(vec);
}

void init_matrix(float** mat, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            mat[i][j] = (float)(i + j);
        }
    }
}

void init_vector(float* vec, int n) {
    for (int i = 0; i < n; i++) {
        vec[i] = (float)i;
    }
}

void mat_vec_dot_naive(float** mat, float* vec, float* res, int n) {
    for (int j = 0; j < n; j++) { 
        res[j] = 0.0f;
        for (int i = 0; i < n; i++) { 
            res[j] += mat[i][j] * vec[i];
        }
    }
}

void mat_vec_dot_cache_opt(float** mat, float* vec, float* res, int n) {
    for (int j = 0; j < n; j++) {
        res[j] = 0.0f;
    }
    for (int i = 0; i < n; i++) { 
        for (int j = 0; j < n; j++) { 
            res[j] += mat[i][j] * vec[i];
        }
    }
}

double timeit(void (*func)(float**, float*, float*, int), float** mat, float* vec, float* res, int n, int repeat) {
    LARGE_INTEGER freq, start, end;
    QueryPerformanceFrequency(&freq);
    func(mat, vec, res, n);

    QueryPerformanceCounter(&start);
    for (int r = 0; r < repeat; r++) {
        func(mat, vec, res, n);
    }
    QueryPerformanceCounter(&end);

    double elapsed = (double)(end.QuadPart - start.QuadPart) / freq.QuadPart;
    return elapsed / repeat;
}

double timeit_avg(void (*func)(float**, float*, float*, int), float** mat, float* vec, float* res, int n, int repeat) {
    vector<double> times(RUN_TIMES);
    for (int i = 0; i < RUN_TIMES; i++) {
        times[i] = timeit(func, mat, vec, res, n, repeat);
    }
    double avg = accumulate(times.begin(), times.end(), 0.0) / RUN_TIMES;
    return avg;
}

int main() {
    cout << "表 2-1 矩阵内积性能测试结果表" << endl;
    cout << left << setw(12) << "问题规模n" 
         << setw(15) << "Cache 区间" 
         << setw(12) << "重复次数" 
         << setw(20) << "平凡算法时间(秒)" 
         << setw(20) << "优化算法时间(秒)" 
         << setw(20) << "性能提升倍数" << endl;
    cout << string(99, '-') << endl;

    for (int n : N_LIST) {
  
        string cache_range = get_cache_range(n);
        int repeat = get_repeat_count(n);

        float** mat = alloc_matrix(n);
        float* vec = alloc_vector(n);
        float* res_naive = alloc_vector(n);
        float* res_opt = alloc_vector(n);
        init_matrix(mat, n);
        init_vector(vec, n);

        double t_naive = timeit_avg(mat_vec_dot_naive, mat, vec, res_naive, n, repeat);
        double t_opt = timeit_avg(mat_vec_dot_cache_opt, mat, vec, res_opt, n, repeat);
        double speedup = (t_opt > 1e-9) ? (t_naive / t_opt) : 0.0; // 避免除零

        cout << left << setw(12) << n
             << setw(15) << cache_range
             << setw(12) << repeat
             << setw(20) << fixed << setprecision(8) << t_naive
             << setw(20) << fixed << setprecision(8) << t_opt
             << setw(20) << fixed << setprecision(4) << speedup << endl;

        free_matrix(mat, n);
        free_vector(vec);
        free_vector(res_naive);
        free_vector(res_opt);
    }

    cout << "\n✅ 测试完成！编译选项：-O2 | 控制变量：无后台程序 | 数据来源：" << RUN_TIMES << "次运行取平均" << endl;
    return 0;
}