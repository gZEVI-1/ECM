#include <iostream>
#include <algorithm>

using namespace std;

// Объявление внешней ассемблерной функции
extern "C" int CountInInterval(int* arr, int size, int num, int avg);

int main() {
    int arr[] = { 10, 20, 30, 40, 50, 60, 70, 80, 90, 100 };
    int size = sizeof(arr) / sizeof(arr[0]);
    int num = 65;

    // Вычисляем среднее в C++
    double avg = 0;
    for (int i = 0; i < size; i++) avg += arr[i];
    avg /= size;
    int avg_int = (int)avg;

    cout << "Массив: ";
    for (int i = 0; i < size; i++) {
        cout << arr[i] << " ";
    }
    cout << endl;

    // Вызываем ассемблерную функцию
    int result = CountInInterval(arr, size, num, avg_int);

    cout << "Среднее арифметическое: " << avg << endl;
    cout << "Границы: (" << min(avg_int, num) << ", " << max(avg_int, num) << ")" << endl;
    cout << "Результат (ассемблер): " << result << endl;

    return 0;
}