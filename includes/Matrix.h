
template<typename T, int M, int N>
class Matrix {

public:
    Matrix() {
        array = new T[M * N];
    }



    T& get(int row, int col) {
        return array[index(row, col)];
    }

    void debugPrint();

    


private:
    unsigned index(int row, int col) {
        return row * N + col;
    }
    
    T* array;
};