void outputString(char *str) {
    while (*str) {
        outputChar(*str);
        str = &str[1];
    }
}

int fib(int n) {
    if (n == 1) {
        return 1;
    }
    if (n < 1) {
        return 0;
    }

    return fib(n - 1) + fib(n - 2);
}

int *getMaxPtr(int *arr, int n) {
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

    return &arr[maxIndex];
}

void replaceMaxWithZero(int *arr, int n) {
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

    arr[maxIndex] = 0;
}

int infRec(int n) {
    return infRec(n - 1);
}

int main() {
    int arr[8];
    arr[0] = 15;
    arr[1] = 8;
    arr[2] = fib(8);
    arr[3] = 40;
    arr[4] = 12;
    arr[5] = 1;
    arr[6] = 7;
    arr[7] = 3;

    replaceMaxWithZero(arr, 8);

    int *ptr = getMaxPtr(arr, 8);
    *ptr = *ptr * 2;

    char name[9];
    name[0] = 'i';
    name[1] = 'd';
    name[2] = 'a';
    name[3] = 'n';
    name[4] = ' ';
    name[5] = 'g';
    name[6] = 'u';
    name[7] = 'r';
    name[8] = '\0';

    outputString(name);
    outputChar('\n');
    outputString(name);
    outputChar('\n');

    return *ptr;
}