#include <iostream>
#include <algorithm>

using namespace std;

int countBetweenAverageAndNum(int* arr, int size, int num) {
    int result = 0;
    int sum = 0;
    int avg = 0;
    int lower = 0;
    int upper = 0;

    // Загружаем значения в регистры ДО ассемблерного блока
    int* arrPtr = arr;
    int arrSize = size;
    int numVal = num;

    __asm {
        // проверка на нустоту
        mov ecx, arrSize 
        cmp ecx, 0              // сравниваем размер с нулем
        je empty_array          // если размер == 0, переходим к метке empty_array

        // подготовка вычисления суммы 
        mov esi, arrPtr         // esi = указатель на начало массива
        xor eax, eax            // eax = 0 (сумма элементов)
        xor ebx, ebx            // ebx = 0 (индекс текущего элемента)

        // сумма
        sum_loop :
            add eax, [esi + ebx * 4]    // eax += arr[ebx] (умножаем на 4 т.к. int)
            inc ebx                     // ebx++ 
            cmp ebx, ecx                // сравниваем индекс с размером массива
            jl sum_loop                 // если индекс < размера, продолжаем цикл

            // вычисление ср. арифм.
            mov sum, eax
            mov eax, sum 
            cdq 
            mov ecx, arrSize        // размер массива в ecx 
            idiv ecx                // eax = eax / ecx , edx = остаток
            mov avg, eax            

            // границы интервала 
            mov eax, numVal
            mov ebx, avg
            cmp eax, ebx
            jge num_greater_or_equal // если num >= avg, переходим к метке num_greater_or_equal

            // num < avg
            mov lower, eax          // lower = num 
            mov upper, ebx          // upper = avg 
            jmp bounds_ready

        num_greater_or_equal :
            mov lower, ebx      // lower = avg (меньшее число)
            mov upper, eax      // upper = num (большее число)

            // подготовка к аодсчету элементов
        bounds_ready :
            mov esi, arrPtr 
            mov ecx, arrSize
            xor edx, edx 
            xor eax, eax

            // подсчет элементов между границами
        count_loop:
            mov ebx, [esi + edx * 4]    
            cmp ebx, lower              // сравниваем элемент с нижней границей
            jle skip_element            // если элемент <= lower, пропускаем
            cmp ebx, upper              // сравниваем элемент с верхней границей
            jge skip_element            // если элемент >= upper, пропускаем
            inc eax                     // иначе увеличиваем счетчик result
            // переход
        skip_element :
            inc edx                     // edx++
            cmp edx, ecx                // сравниваем индекс с размером массива
            jl count_loop               // если индекс < размера, продолжаем цикл

            // сохранение
            mov result, eax
            jmp done 

        empty_array :
            mov result, 0       // для пустого массива результат всегда 0

        done :
    }

    return result;
}

int main() {
    setlocale(LC_ALL, "ru");
    int arr[] = {0};
    int size = sizeof(arr) / sizeof(arr[0]);

    int num = -100;

    cout << "Массив: ";
    for (int i = 0; i < size; i++) {
        cout << arr[i] << " ";
    }
    cout << endl;

    cout << "Число для сравнения:" << num<< endl;

    int result = countBetweenAverageAndNum(arr, size, num);

    double avg = 0;
    for (int i = 0; i < size; i++) avg += arr[i];
    avg /= size;

    cout << "Среднее арифметическое: " << avg << endl;
    cout << "Границы: (" << min((int)avg, num) << ", " << max((int)avg, num) << ")" << endl;
    cout << "Результат (ассемблер): " << result << endl;

    return 0;
}