
void check(int *b, int n) {
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

int main() {
    int val = 14;
    int arr[4];

    check(&arr, 12);

    arr[2] = calc(arr[0], arr[1]);

    if (arr[1] > 100) {
        return 10;
    } else {
        return 0;
    }
}