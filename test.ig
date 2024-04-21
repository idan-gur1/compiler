
void check(char *b, int n) {
    int a = b[0] + 4;

    b[1] = a * 5 + 4;

    while (n > 0) {
        b[1] = b[0] * n;
        n = n - 1;
    }
}

int calc(int a, int b) {
    return a * b / (a + b);
}

int *getItem(int *arr) {
    return &arr[2];
}

int fib(int n) {
    if (n < 2) {
        return 1;
    }

    return fib(n - 1) + fib(n - 2);
}

int main() {
    int intArr[12];
    int *cell = getItem(getItem(intArr));

    int val = -14;
    char arr[4];

    arr[val] = val * 12;

    char *ptr = arr;
    check(ptr, 12);

    int fibNum = fib(4);

    do {
        arr[0] = arr[0] + 12;
        val = val - 1;
        int digitPlus = 1 + val % 10;
    } while (val > 5);

    arr[2] = calc(arr[0], arr[1]);

    if (arr[1] > 100) {
        return 10;
    } else {
        return 0;
    }
}