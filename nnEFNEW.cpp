#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <windows.h>
#include <cerrno> 

using namespace std;

float** alloc_matrix(int n) {
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
    float* vec = (float*)malloc(n * sizeof(float));
    if (vec == nullptr) {
        cerr << "错误：向量内存分配失败！n=" << n << "，原因：" << strerror(errno) << endl;
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

void init_mat_vec(float** mat, float* vec, int n) {
    for (int i = 0; i < n; i++) {
        vec[i] = (float)i + 0.1f;
        for (int j = 0; j < n; j++) {
            mat[i][j] = (float)(i+j) + 0.01f;
        }
    }
}

float mat_vec_dot(float** mat, float* vec, int n) {
    float sum = 0.0f;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            sum += mat[i][j] * vec[j];
        }
    }
    return sum;
}

double time_mat_vec(float** mat, float* vec, int n, int repeat) {
    LARGE_INTEGER freq, start, end;
 
    if (!QueryPerformanceFrequency(&freq)) {
        cerr << "错误：无法获取系统时钟频率！" << endl;
        exit(EXIT_FAILURE);
    }

    mat_vec_dot(mat, vec, n);

    QueryPerformanceCounter(&start);
    for (int r = 0; r < repeat; r++) {
        mat_vec_dot(mat, vec, n);
    }
    QueryPerformanceCounter(&end);

    double total_time = (double)(end.QuadPart - start.QuadPart) / freq.QuadPart;
    return total_time / repeat;
}

int get_repeat_count(int n) {
    if (n < 512) {
        return 1000;  
    } else {
        return 20;     
    }
}

int main() {
    int n_list[] = {64, 128, 256, 1024, 2048, 4096, 16384};
    int n_count = sizeof(n_list) / sizeof(int);

    cout << "问题规模n,重复次数,平均时间(秒)" << endl;

    for (int i = 0; i < n_count; i++) {
        int n = n_list[i];
        int repeat = get_repeat_count(n);

        float** mat = alloc_matrix(n);
        float* vec = alloc_vector(n);
        init_mat_vec(mat, vec, n);

        double t = time_mat_vec(mat, vec, n, repeat);
        cout << n << "," << repeat << "," << fixed << setprecision(8) << t << endl;

        free_matrix(mat, n);
        free_vector(vec);
    }

    cout << "\n✅ 矩阵向量内积性能测试完成！数据已输出为CSV格式。" << endl;
    return 0;
}