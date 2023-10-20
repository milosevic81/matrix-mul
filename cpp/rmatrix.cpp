#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>
// #include </opt/homebrew/opt/libomp/include/omp.h>

using namespace std;

typedef vector<int> Vec;
typedef vector<Vec> Mat;


Mat init(int n) {
    Mat m = Mat(n, Vec(n, 0));
    for (int i=0;i<n;i++) {
        for (int j=0; j<n;j++) {
            m[i][j] = rand() % 100;
        }
    }
    return m;
}

void show(const Mat& m) {
    int n  = m.size();
    for (int i=0;i<n;i++) {
        cout << "| ";
        for (int j=0; j<n;j++) {
            cout << m[i][j] << (j==n-1?" |\n":", ");
        }
    }
    cout << endl;
}

Mat mul(const Mat &a, const Mat &b) {
    // assert square matrix 2^x
    int n  = a.size();
    Mat m = Mat(n, Vec(n, 0));

    // #pragma omp parallel for
    for (int i=0; i<n; i++) {
        // printf("Hello from process: %d\n", omp_get_thread_num());
        for (int j=0; j<n; j++) {
            int acc = 0;
            for (int k=0; k<n; k++) {
                acc += a[i][k] * b[k][j];
            }
            m[i][j] = acc;
        }
    }
    return m;
}

Mat operator+(const Mat &a, const Mat &b){
    int n  = a.size();
    Mat m = Mat(n, Vec(n, 0));
    for (int i=0; i<n; i++) {
        for (int j=0; j<n; j++) {            
            m[i][j] = a[i][j] + b[i][j];
        }
    }
    return m;
}

Mat operator-(const Mat &a, const Mat &b) {
    int n  = a.size();
    Mat m = Mat(n, Vec(n, 0));
    for (int i=0; i<n; i++) {
        for (int j=0; j<n; j++) {            
            m[i][j] = a[i][j] - b[i][j];
        }
    }
    return m;
}

Mat slice(const Mat &a, int m1, int m2, int n1, int n2) {
    Mat m = Mat(m2-m1, Vec(n2-n1, 0));
    for (int i = m1; i < m2; i++)
    {
        for (int j = n1; j < n2; j++) {
            m[i-m1][j-n1] = a[i][j]; 
        }
    }
    return m;    
}

Mat merge(const Mat& m1, const Mat& m2, const Mat& m3, const Mat& m4) {
    // assume square matrix
    int n = m1.size();
    Mat m = Mat(2*n, Vec(2*n, 0));

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            m[i][j] = m1[i][j];
            m[i][j+n] = m2[i][j];
            m[i+n][j] = m3[i][j];
            m[i+n][j+n] = m4[i][j];
        }
    }
    return m;
}

int compare(Mat &x, Mat &y) {
    int n  = x.size();
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (x[i][j] != y[i][j]) {
                return -1;
            }
        }
    }
    return 0;
}

// Recursive multiplication
Mat rmul(const Mat &x, const Mat &y) {

    int n  = x.size();
    if (n <= 128) {
        return mul(x, y);
    }

    Mat a11 = slice(x, 0, n/2, 0, n/2);
    Mat a12 = slice(x, 0, n/2, n/2, n);
    Mat a21 = slice(x, n/2, n, 0, n/2);
    Mat a22 = slice(x, n/2, n, n/2, n);

    Mat b11 = slice(y, 0, n/2, 0, n/2);
    Mat b12 = slice(y, 0, n/2, n/2, n);
    Mat b21 = slice(y, n/2, n, 0, n/2);
    Mat b22 = slice(y, n/2, n, n/2, n);

    Mat m1 = (rmul(a11, b11) + rmul(a12, b21));
    Mat m2 = (rmul(a11, b12) + rmul(a12, b22));
    Mat m3 = (rmul(a21, b11) + rmul(a22, b21));
    Mat m4 = (rmul(a21, b12) + rmul(a22, b22));

    return merge(m1, m2, m3, m4);
}

// Recursive with Strassen formula
Mat smul(const Mat &x, const Mat &y) {
    
    int n  = x.size();
    if (n <= 128) {
        return mul(x, y);
    }

    Mat a11 = slice(x, 0, n/2, 0, n/2);
    Mat a12 = slice(x, 0, n/2, n/2, n);
    Mat a21 = slice(x, n/2, n, 0, n/2);
    Mat a22 = slice(x, n/2, n, n/2, n);

    Mat b11 = slice(y, 0, n/2, 0, n/2);
    Mat b12 = slice(y, 0, n/2, n/2, n);
    Mat b21 = slice(y, n/2, n, 0, n/2);
    Mat b22 = slice(y, n/2, n, n/2, n);

    Mat m1 = smul((a11 + a22), (b11 + b22));
    Mat m2 = smul((a21 + a22), b11);
    Mat m3 = smul(a11, (b12 - b22));
    Mat m4 = smul(a22, (b21 - b11));
    Mat m5 = smul((a11 + a12), b22);
    Mat m6 = smul((a21 - a11), (b11 + b12));
    Mat m7 = smul((a12 - a22), (b21 + b22));

    return merge(
        ((m1 + m4) + (m7 - m5)),
        (m3 + m5),
        (m2 + m4),
        ((m3 + m6) + (m1 - m2)));
}

void test_1() {
    cout << "## Test 1 ##" << endl;
    auto start = chrono::high_resolution_clock::now();
    auto duration = chrono::high_resolution_clock::now() - start;
    auto duration_milis = chrono::duration_cast<chrono::milliseconds>(duration);
    Mat c;

    for (int i = 8; i < 14; i++)
    {
        int n = pow(2, i);
        n=4;

        Mat a = init(n);
        Mat b = init(n);

        start = chrono::high_resolution_clock::now();
        Mat d = rmul(a, b);
        duration = chrono::high_resolution_clock::now() - start;
        duration_milis = chrono::duration_cast<chrono::milliseconds>(duration);
        cout << "Matrix size " << n << " rmul Duration: " << duration.count()/1000000000.0 << endl;
      
        start = chrono::high_resolution_clock::now();
        Mat e = smul(a, b);
        duration = chrono::high_resolution_clock::now() - start;
        duration_milis = chrono::duration_cast<chrono::milliseconds>(duration);
        cout << "Matrix size " << n << " smul Duration: " << duration.count()/1000000000.0 << endl;

        if ( n<=2048 ) {
            start = chrono::high_resolution_clock::now();
            Mat c = mul(a, b);
            duration = chrono::high_resolution_clock::now() - start;
            duration_milis = chrono::duration_cast<chrono::milliseconds>(duration);
            cout << "Matrix size " << n << " mul Duration: " << duration.count()/1000000000.0 << endl;

            cout << "Compare mul rmul : " << compare(c, d) << endl;
            cout << "Compare mul smul : " << compare(c, e) << endl;
        }
    }    
}

int main() {
    test_1();
}


/*

+------+--------+---------+
| size | mul(s) | rmul(s) |
+------+--------+---------+
| 1024 | 1.00   | 0.40    |
| 2048 | 24.8   | 3.28    |
| 4096 | 302.9  | 26.0    |
+------+--------+---------+

TODO

Implement Strassen algorithm
https://en.wikipedia.org/wiki/Strassen_algorithm

Implement Winograd form
Paralellize naive, strassen and winograd using openMP

Implement on CUDA

Implement on M1
https://developer.apple.com/documentation/accelerate/veclib
https://developer.apple.com/library/archive/documentation/Performance/Conceptual/vDSP_Programming_Guide/Introduction/Introduction.html

*/