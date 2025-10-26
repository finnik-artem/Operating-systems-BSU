#include <iostream>
#include <vector>
#include <thread>
#include <random>
#include <chrono>
#include <mutex>

using namespace std;
using namespace std::chrono;

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

void multiplyBlock(const vector<vector<double>>& A, const vector<vector<double>>& B,
                   vector<vector<double>>& C, int i0, int j0, int r0, int k, int N, mutex& mtx)
{
    for (int i = i0; i < i0 + k; ++i) {
        for (int j = j0; j < j0 + k; ++j) {
            double sum = 0;
            for (int r = r0; r < r0 + k; ++r)
                sum += A[i][r] * B[r][j];
            lock_guard<mutex> lock(mtx);
            C[i][j] += sum;
        }
    }
}

vector<int> getDivisors(int N) {
    vector<int> divisors;
    for (int k = 1; k <= N; ++k)
        if (N % k == 0)
            divisors.push_back(k);
    return divisors;
}

int main() {
    int N = 8;
    cout << "Введите размер матрицы - ";
    cin >> N;
    cout << "\nMatrix size N = " << N << endl;

    auto A = generateMatrix(N);
    auto B = generateMatrix(N);

    auto start_single = high_resolution_clock::now();
    auto C_single = multiplySimple(A, B);
    auto end_single = high_resolution_clock::now();
    double time_single = duration<double, milli>(end_single - start_single).count();

    cout << "\nSingle-threaded time: " << time_single << " ms\n" << endl;

    auto divisors = getDivisors(N);
    cout << "Block size (k)\tThreads\tTime (ms)\tSpeedup\n";

    for (int k : divisors) {
        vector<vector<double>> C(N, vector<double>(N, 0.0));
        mutex mtx;
        vector<thread> threads;

        auto start = high_resolution_clock::now();

        for (int i = 0; i < N; i += k)
            for (int j = 0; j < N; j += k)
                for (int r = 0; r < N; r += k)
                    threads.emplace_back(multiplyBlock, cref(A), cref(B), ref(C),
                                         i, j, r, k, N, ref(mtx));

        for (auto& t : threads) t.join();

        auto end = high_resolution_clock::now();
        double time = duration<double, milli>(end - start).count();

        cout << k << "\t\t" << threads.size() << "\t" 
             << time << "\t\t" << (time_single / time) << "x" << endl;
    }

    return 0;
}
