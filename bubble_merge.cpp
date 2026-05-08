#include <iostream>
#include <vector>
#include <omp.h>
using namespace std;

// ---------------- BUBBLE SORT ----------------

// Sequential Bubble Sort
void sequentialBubbleSort(vector<int>& arr) {
    int n = arr.size();
    for(int i = 0; i < n - 1; i++) {
        for(int j = 0; j < n - i - 1; j++) {
            if(arr[j] > arr[j + 1]) {
                swap(arr[j], arr[j + 1]);
            }
        }
    }
}

// Parallel Bubble Sort (Odd-Even Transposition Sort)
void parallelBubbleSort(vector<int>& arr) {
    int n = arr.size();
    for(int i = 0; i < n; i++) {
        // Even phase
        #pragma omp parallel for shared(arr)
        for(int j = 0; j < n - 1; j += 2) {
            if(arr[j] > arr[j + 1]) {
                swap(arr[j], arr[j + 1]);
            }
        }
        // Odd phase
        #pragma omp parallel for shared(arr)
        for(int j = 1; j < n - 1; j += 2) {
            if(arr[j] > arr[j + 1]) {
                swap(arr[j], arr[j + 1]);
            }
        }
    }
}

// ---------------- MERGE SORT ----------------

void merge(vector<int>& arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    vector<int> L(n1), R(n2);

    for(int i = 0; i < n1; i++)
        L[i] = arr[left + i];
    for(int i = 0; i < n2; i++)
        R[i] = arr[mid + 1 + i];

    int i = 0, j = 0, k = left;
    while(i < n1 && j < n2) {
        if(L[i] <= R[j])
            arr[k++] = L[i++];
        else
            arr[k++] = R[j++];
    }
    while(i < n1) arr[k++] = L[i++];
    while(j < n2) arr[k++] = R[j++];
}

// Sequential Merge Sort
void sequentialMergeSort(vector<int>& arr, int left, int right) {
    if(left < right) {
        int mid = (left + right) / 2;
        sequentialMergeSort(arr, left, mid);
        sequentialMergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    }
}

// Parallel Merge Sort using tasks (correct approach)
void parallelMergeSort(vector<int>& arr, int left, int right, int depth = 0) {
    if(left < right) {
        int mid = (left + right) / 2;

        if(depth < 4) {
            // Use tasks only up to a limited depth to avoid thread explosion
            #pragma omp task shared(arr)
            parallelMergeSort(arr, left, mid, depth + 1);

            #pragma omp task shared(arr)
            parallelMergeSort(arr, mid + 1, right, depth + 1);

            #pragma omp taskwait  // Wait for both halves to finish before merging
        } else {
            // Fall back to sequential for deeper recursion levels
            sequentialMergeSort(arr, left, mid);
            sequentialMergeSort(arr, mid + 1, right);
        }

        merge(arr, left, mid, right);
    }
}

// ---------------- UTILITY FUNCTIONS ----------------

void printArray(const vector<int>& arr, const string& label) {
    cout << label << ": ";
    for(int x : arr)
        cout << x << " ";
    cout << endl;
}

// ---------------- MAIN FUNCTION ----------------

int main() {
    vector<int> data = {45, 23, 89, 12, 78, 34, 56, 90, 11, 67};

    vector<int> arr1 = data;
    vector<int> arr2 = data;
    vector<int> arr3 = data;
    vector<int> arr4 = data;

    double t_start, t_end;

    cout << "=== Sorting Algorithm Performance ===" << endl;
    cout << "Number of threads available: " << omp_get_max_threads() << endl;
    printArray(data, "\nOriginal Array");

    // Sequential Bubble Sort
    t_start = omp_get_wtime();
    sequentialBubbleSort(arr1);
    t_end = omp_get_wtime();
    cout << "\nSequential Bubble Sort Time: " << (t_end - t_start) << " seconds";
    printArray(arr1, "Result");

    // Parallel Bubble Sort
    t_start = omp_get_wtime();
    parallelBubbleSort(arr2);
    t_end = omp_get_wtime();
    cout << "\nParallel Bubble Sort Time:   " << (t_end - t_start) << " seconds";
    printArray(arr2, "Result");

    // Sequential Merge Sort
    t_start = omp_get_wtime();
    sequentialMergeSort(arr3, 0, arr3.size() - 1);
    t_end = omp_get_wtime();
    cout << "\nSequential Merge Sort Time:  " << (t_end - t_start) << " seconds";
    printArray(arr3, "Result");

    // Parallel Merge Sort — must be called inside a parallel region with single
    t_start = omp_get_wtime();
    #pragma omp parallel
    {
        #pragma omp single  // Only one thread initiates the task tree
        parallelMergeSort(arr4, 0, arr4.size() - 1);
    }
    t_end = omp_get_wtime();
    cout << "\nParallel Merge Sort Time:    " << (t_end - t_start) << " seconds";
    printArray(arr4, "Result");

    return 0;
}
