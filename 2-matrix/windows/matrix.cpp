#include <iostream>
#include <vector>
#include <windows.h>
#include <random>
#include <chrono>

using namespace std;
using namespace std::chrono;

struct ThreadData {
    const vector<vector<double>>* A;
    const vector<vector<double>>* B;
    vector<vector<double>>* C;
    int i0, j0, r0, k, N;
};

HANDLE hMutex;

vector<vector<double>> generateMatrix(int N) {
    vector<vector<double>> M(N, vector<double>(N));
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0, 10);
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            M[i][j] = dis(gen);
    return M;
}

vector<vector<double>> multiplySimple(const vector<vector<double>>& A,
                                      const vector<vector<double>>& B) {
    int N = A.size();
    vector<vector<double>> C(N, vector<double>(N, 0.0));
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            for (int k = 0; k < N; ++k)
                C[i][j] += A[i][k] * B[k][j];
    return C;
}

DWORD WINAPI multiplyBlock(LPVOID arg) {
    ThreadData* data = static_cast<ThreadData*>(arg);
    const auto& A = *(data->A);
    const auto& B = *(data->B);
    auto& C = *(data->C);

    for (int i = data->i0; i < data->i0 + data->k; ++i) {
        for (int j = data->j0; j < data->j0 + data->k; ++j) {
            double sum = 0;
            for (int r = data->r0; r < data->r0 + data->k; ++r)
                sum += A[i][r] * B[r][j];

            WaitForSingleObject(hMutex, INFINITE);
            C[i][j] += sum;
            ReleaseMutex(hMutex);
        }
    }
    return 0;
}

vector<int> getDivisors(int N) {
    vector<int> divisors;
    for (int k = 1; k <= N; ++k)
        if (N % k == 0)
            divisors.push_back(k);
    return divisors;
}

int main() {
    int N;
    cout << "Введите размер матрицы - ";
    cin >> N;
    cout << "\nMatrix size N = " << N << endl;

    auto A = generateMatrix(N);
    auto B = generateMatrix(N);

    auto t1 = high_resolution_clock::now();
    auto C_single = multiplySimple(A, B);
    auto t2 = high_resolution_clock::now();
    double time_single = duration<double, milli>(t2 - t1).count();

    cout << "\nSingle-threaded time: " << time_single << " ms\n" << endl;

    hMutex = CreateMutex(nullptr, FALSE, nullptr);

    auto divisors = getDivisors(N);
    cout << "Block size (k)\tThreads\tTime (ms)\tSpeedup\n";

    for (int k : divisors) {
        vector<vector<double>> C(N, vector<double>(N, 0.0));
        vector<HANDLE> threads;
        vector<ThreadData> threadData;

        auto start = high_resolution_clock::now();

        for (int i = 0; i < N; i += k)
            for (int j = 0; j < N; j += k)
                for (int r = 0; r < N; r += k)
                    threadData.push_back({&A, &B, &C, i, j, r, k, N});

        threads.resize(threadData.size());

        for (size_t t = 0; t < threadData.size(); ++t) {
            threads[t] = CreateThread(
                nullptr,
                0,
                multiplyBlock,
                &threadData[t],
                0,
                nullptr
            );
        }

        WaitForMultipleObjects(
            static_cast<DWORD>(threads.size()),
            threads.data(),
            TRUE,
            INFINITE
        );

        for (auto h : threads)
            CloseHandle(h);

        auto end = high_resolution_clock::now();
        double time_ms = duration<double, milli>(end - start).count();

        cout << k << "\t\t" << threads.size() << "\t"
             << time_ms << "\t\t" << (time_single / time_ms) << "x" << endl;
    }

    CloseHandle(hMutex);
    return 0;
}
