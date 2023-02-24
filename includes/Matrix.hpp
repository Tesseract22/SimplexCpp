#include <Matrix.h>
#include <iostream>

using namespace std;
template<typename T, int M, int N>
void Matrix<T, M, N>::debugPrint() {
    for (int i = 0; i < M; ++i) {
        for (int j = 0;j  < N; ++j) {
            cout << get(i, j) << ' ';
        }
        cout << endl;
    }
}


template<typename T, int M, int N>
