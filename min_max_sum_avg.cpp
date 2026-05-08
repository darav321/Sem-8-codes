#include <iostream>
#include <vector>
#include <omp.h>
using namespace std;

int main() {
    vector<int> arr = {10, 20, 30, 40, 50, 60, 70, 80};
    int n = arr.size();

    int sum = 0;
    int minimum = arr[0];
    int maximum = arr[0];
    double average;

    double t_start, t_end;

    // Display original array
    cout << "Array Elements: ";
    for(int x : arr)
        cout << x << " ";
    cout << endl;

    cout << "\nNumber of threads available: " << omp_get_max_threads() << endl;

    t_start = omp_get_wtime();

    // Single parallel loop for Sum, Minimum and Maximum
    #pragma omp parallel for reduction(+:sum) reduction(min:minimum) reduction(max:maximum)
    for(int i = 0; i < n; i++) {
        sum      += arr[i];
        if(arr[i] < minimum) minimum = arr[i];
        if(arr[i] > maximum) maximum = arr[i];
    }

    // Average (calculated after parallel section)
    average = (double)sum / n;

    t_end = omp_get_wtime();

    // Output Results
    cout << "\n==============================";
    cout << "\n       RESULTS               ";
    cout << "\n==============================";
    cout << "\nSum            = " << sum;
    cout << "\nMinimum        = " << minimum;
    cout << "\nMaximum        = " << maximum;
    cout << "\nAverage        = " << average;
    cout << "\n==============================";
    cout << "\nExecution Time = " << (t_end - t_start) << " seconds";
    cout << endl;

    return 0;
}
