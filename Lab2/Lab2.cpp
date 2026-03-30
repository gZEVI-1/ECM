#include <iostream>

using namespace std;


int CPP_calc(int a, int b, int c, int d) {
    return ((4 * (b / c) - d) / (12 * c + a - b));
}
int func() {
    int a, b, c, d;
    setlocale(LC_ALL, "ru");
    cout << "Введите значения a, b, c, d: ";
    cin >> a >> b >> c >> d;

    int result = 0;
    const char* err = nullptr;
    static const char ezero[] = "Ошибка: деление на ноль!\n";
    static const char eoverflow[] = "Ошибка: переполнение!\n";

    __asm {


        ; Вычисляем знаменатель : 12 * c + a - b
        mov eax, c
        imul eax, 12; eax = 12 * c
        jo error_of

        add eax, a; eax = 12 * c + a
        jo error_of

        sub eax, b; eax = 12 * c + a - b
        jo error_of

        mov ebx, eax; ebx = 12 * c + a - b
        test ebx, ebx
        jz error_zf

        push ebx; на стеке 12 * c + a - b


        ; Вычисляем числитель : 4 * b / c - d

        mov eax, b

        imul eax, 4; eax = 4 * b
        jo error_of

        mov ecx, c
        test ecx, ecx
        jz error_zf; деление на ноль

        cdq; eax = > edx:eax
        idiv ecx; eax = 4 * b / c

        sub eax, d; eax = 4 * b / c - d
        jo error_of

        ; Итоговое выражение
        pop ebx; восстанавливаем знаменатель
        cdq; eax = > edx:eax
        idiv ebx; eax = res


        jmp done

        error_zf :
        mov err, OFFSET ezero; OFFSET для обработки нестандартного типа
            jmp done
            error_of :
        mov err, OFFSET eoverflow
            jmp done
            done :
        mov result, eax

    }


    if (err != nullptr) {
        cout << err;
    }
    else {
        cout << "Ошибок нет\n";
        cout << "Результат = " << result << '\n';
        cout << "результат на с++ = " << CPP_calc(a, b, c, d) << '\n';
    }


    cout << ">>> Продолжить? (любой символ для продолжения )(-1 - выход):";
    int answ;
    cin >> answ;

    if (answ != -1) { return 0; }
    return -1;

}

int main() {
    while (func() != -1) { system("cls"); };
    return 0;
}
