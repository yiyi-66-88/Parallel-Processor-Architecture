#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <cmath>
#include <windows.h>

using namespace std;

float** alloc_matrix(int n) {
    float** mat = (float**)malloc(n * sizeof(float*));
    for (int i = 0; i < n; i++) {
        mat[i] = (float*)malloc(n * sizeof(float));
    }
    return mat;
}

float* alloc_vector(int n) {
    float* vec = (float*)malloc(n * sizeof(float));
    return vec;
}

void free_matrix(float** mat, int n) {
    for (int i = 0; i < n; i++) {
        free(mat[i]);
    }
    free(mat);
}

void free_vector(float* vec) {
    free(vec);
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

bool verify_result(float* res_naive, float* res_opt, int n) {
    const float eps = 1e-6f;
    for (int j = 0; j < n; j++) {
        if (fabs(res_naive[j] - res_opt[j]) > eps) {
            cerr << "验证失败！位置 j=" << j
                 << " naive=" << res_naive[j]
                 << " opt=" << res_opt[j] << endl;
            return false;
        }
    }
    cout << "正确性验证通过！" << endl;
    return true;
}

double timeit(void (*func)(float**, float*, float*, int), float** mat, float* vec, float* res, int n, int repeat = 100) {
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

int main() {
    int n_small = 10;
    float** mat_small = alloc_matrix(n_small);
    float* vec_small = alloc_vector(n_small);
    float* res_naive_small = alloc_vector(n_small);
    float* res_opt_small = alloc_vector(n_small);

    init_matrix(mat_small, n_small);
    init_vector(vec_small, n_small);

    mat_vec_dot_naive(mat_small, vec_small, res_naive_small, n_small);
    mat_vec_dot_cache_opt(mat_small, vec_small, res_opt_small, n_small);

    verify_result(res_naive_small, res_opt_small, n_small);

    free_matrix(mat_small, n_small);
    free_vector(vec_small);
    free_vector(res_naive_small);
    free_vector(res_opt_small);

    int n_large = 1024;
    float** mat_large = alloc_matrix(n_large);
    float* vec_large = alloc_vector(n_large);
    float* res_naive_large = alloc_vector(n_large);
    float* res_opt_large = alloc_vector(n_large);

    init_matrix(mat_large, n_large);
    init_vector(vec_large, n_large);

    int repeat = 10;
    double t_naive = timeit(mat_vec_dot_naive, mat_large, vec_large, res_naive_large, n_large, repeat);
    double t_opt = timeit(mat_vec_dot_cache_opt, mat_large, vec_large, res_opt_large, n_large, repeat);

    cout << "\n=== 性能测试结果（n=" << n_large << "） ===" << endl;
    cout << fixed << setprecision(8);
    cout << "平凡算法（逐列）平均时间：" << t_naive << " 秒" << endl;
    cout << "cache 优化算法（逐行）平均时间：" << t_opt << " 秒" << endl;
    cout << "性能提升比：" << t_naive / t_opt << " 倍" << endl;

    free_matrix(mat_large, n_large);
    free_vector(vec_large);
    free_vector(res_naive_large);
    free_vector(res_opt_large);

    return 0;
}