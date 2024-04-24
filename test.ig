
int fib(int n) {
    if (n == 1) {
        return 1;
    }
    if (n < 1) {
        return 0;
    }

    return fib(n - 1) + fib(n - 2);
}

int findMaxIndex(int *arr, int n) {
    int i = 1;
    int max = arr[0];
    int maxIndex = 0;

    while (i < n) {
        if (arr[i] > max) {
            max = arr[i];
            maxIndex = i;
        }
        i = i + 1;
    }

    return maxIndex;
}

int main() {
    int arr[8];
    arr[0] = 15;
    arr[1] = 8;
    arr[2] = 6;
    arr[3] = 20;
    arr[4] = 12;
    arr[5] = 1;
    arr[6] = 7;
    arr[7] = 3;

    return findMaxIndex(arr, 8);
}