#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <windows.h>
#include <cerrno>
#include <algorithm> 

using namespace std;

const int GLOBAL_BLOCK_SIZE = 64;

float** alloc_matrix(int n) {

    if (n <= 0) {
        cerr << "错误：矩阵维度n必须为正整数！n=" << n << endl;
        exit(EXIT_FAILURE);
    }

    float** mat = (float**)malloc(n * sizeof(float*));
    if (mat == nullptr) {
        cerr << "错误：矩阵内存分配失败！n=" << n << "，原因：" << strerror(errno) << endl;
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
        cerr << "错误：向量内存分配失败！n=" << n << "，原因：" << strerror(errno) << endl;
        exit(EXIT_FAILURE);
    }
    return vec;
}

void free_matrix(float** mat, int n) {
    if (mat != nullptr && n > 0) {
        for (int i = 0; i < n; i++) {
            if (mat[i] != nullptr) free(mat[i]);
        }
        free(mat);
    }
}

void free_vector(float* vec) {
    if (vec != nullptr) free(vec);
}

void init_mat_vec(float** mat, float* vec, int n) {
    if (mat == nullptr || vec == nullptr || n <= 0) {
        cerr << "错误：初始化失败（空指针/无效维度）！" << endl;
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < n; i++) {
        vec[i] = (float)i + 0.1f; 
        for (int j = 0; j < n; j++) {
            mat[i][j] = (float)(i+j) + 0.01f; 
        }
    }
}

float mat_vec_dot_naive(float** mat, float* vec, int n) {
    if (mat == nullptr || vec == nullptr || n <= 0) return 0.0f;

    float sum = 0.0f;
    for (int i = 0; i < n; i++) {      
        for (int j = 0; j < n; j++) {  
            sum += mat[i][j] * vec[j]; 
        }
    }
    return sum;
}

float mat_vec_dot_opt(float** mat, float* vec, int n) {
    if (mat == nullptr || vec == nullptr || n <= 0) return 0.0f;

    float sum = 0.0f;
    for (int i = 0; i < n; i++) {      
        float* row_ptr = mat[i];      
        for (int j = 0; j < n; j++) {  
            sum += row_ptr[j] * vec[j];
        }
    }
    return sum;
}

float mat_vec_dot_opt_block(float** mat, float* vec, int n, int block_size=GLOBAL_BLOCK_SIZE) {
    if (mat == nullptr || vec == nullptr || n <= 0) return 0.0f;

    float sum = 0.0f;

    for (int ii = 0; ii < n; ii += block_size) {
        for (int jj = 0; jj < n; jj += block_size) {
           
            int i_end = min(ii + block_size, n);
            int j_end = min(jj + block_size, n);
            for (int i = ii; i < i_end; i++) {
                float* row_ptr = mat[i];
                for (int j = jj; j < j_end; j++) {
                    sum += row_ptr[j] * vec[j];
                }
            }
        }
    }
    return sum;
}

float mat_vec_dot_opt_block_wrapper(float** mat, float* vec, int n) {
    return mat_vec_dot_opt_block(mat, vec, n, GLOBAL_BLOCK_SIZE);
}

double time_mat_vec(float** mat, float* vec, int n, int repeat, 
                    float (*alg_func)(float**, float*, int)) {
    LARGE_INTEGER freq, start, end;

    if (!QueryPerformanceFrequency(&freq)) {
        cerr << "错误：无法获取系统时钟频率！" << endl;
        exit(EXIT_FAILURE);
    }

   
    alg_func(mat, vec, n);

   
    QueryPerformanceCounter(&start);
    for (int r = 0; r < repeat; r++) {
        alg_func(mat, vec, n);
    }
    QueryPerformanceCounter(&end);

    
    double total_time = (double)(end.QuadPart - start.QuadPart) / freq.QuadPart;
    return total_time / repeat;
}

int get_repeat_count(int n) {
    if (n < 1000)      return 1000;   
    else if (n < 10000) return 100;   
    else               return 20;     
}


int main() {

    int n_list[] = {256, 512, 1024, 2048, 4096};
    int n_count = sizeof(n_list) / sizeof(int);

    cout << "问题规模n,重复次数,平凡算法(秒),基础优化(秒),分块优化(秒),基础优化提升比,分块优化提升比" << endl;

    for (int i = 0; i < n_count; i++) {
        int n = n_list[i];
        int repeat = get_repeat_count(n);

        float** mat = alloc_matrix(n);
        float* vec = alloc_vector(n);
        init_mat_vec(mat, vec, n);

        double t_naive = time_mat_vec(mat, vec, n, repeat, mat_vec_dot_naive);
        double t_opt = time_mat_vec(mat, vec, n, repeat, mat_vec_dot_opt);
        double t_opt_block = time_mat_vec(mat, vec, n, repeat, mat_vec_dot_opt_block_wrapper);

        double speedup_opt = (t_opt > 0) ? (t_naive / t_opt) : 0.0;
        double speedup_block = (t_opt_block > 0) ? (t_naive / t_opt_block) : 0.0;

        cout << n << "," << repeat << "," 
             << fixed << setprecision(8) << t_naive << "," 
             << fixed << setprecision(8) << t_opt << "," 
             << fixed << setprecision(8) << t_opt_block << "," 
             << fixed << setprecision(4) << speedup_opt << "," 
             << fixed << setprecision(4) << speedup_block << endl;

        free_matrix(mat, n);
        free_vector(vec);
    }

    cout << "\n✅ 三种算法性能对比完成！数据已输出为CSV格式，可直接导入Excel分析。" << endl;
    return 0;
}