#include <iostream>
#include <cmath>

using namespace std;

double asm_func(double a, double b, double c, double d) {
    double result;
    double c12 = 12.0;
    double c4 = 4.0;

    __asm {
        finit

        ; Проверка c на ноль
        fld qword ptr[c]
        fldz
        fucomp
        fnstsw ax
        sahf
        jz div_by_zero

        ; 4 * b / c - d
        fld qword ptr[b]; st(0) = b
        fld qword ptr[c4]; st(0) = 4, st(1) = b
        fmul; st(0) = 4 * b
        fld qword ptr[c]; st(0) = c, st(1) = 4 * b
        fdiv; st(0) = 4 * b / c
        fsub qword ptr[d]; st(0) = 4 * b / c - d

        ; 12 * c + a - b
        fld qword ptr[c]; st(0) = c, st(1) = числитель
        fmul qword ptr[c12];
        fadd qword ptr[a]; 
        fsub qword ptr[b]; st(0) = 12 * c + a - b

        ; Проверка знаменателя на ноль
        fldz
        fucomp st(1); сравниваем знаменатель(st(1)) с 0
        fnstsw ax
        sahf
        jz div_by_zero

        ; Вычисление дроби
        fxch st(1); меняем: st(0) = числитель, st(1) = знаменатель
        fdiv st(0), st(1); st(0) = st(0) / st(1)

        jmp done

        div_by_zero :
            finit
            fldz

        done :
            fstp qword ptr[result]
    }

    return result;
}

double cpp_func(double a, double b, double c, double d) {
    if (c == 0.0) {
        cout << "Ошибка: деление на ноль " << endl;
        return 0.0;
    }

    double numerator = 4.0 * b / c - d;
    double denominator = 12.0 * c + a - b;

    if (denominator == 0.0) {
        cout << "Ошибка: деление на ноль " << endl;
        return 0.0;
    }

    return numerator / denominator;
}

int main() {
    double a, b, c, d;
    setlocale(LC_ALL, "ru");

    cout << "Введите значения a, b, c, d: ";
    cin >> a >> b >> c >> d;

    double res_asm = asm_func(a, b, c, d);
    double res_cpp = cpp_func(a, b, c, d);

    cout << "Обычный тест:" << endl;
    cout << "a=" << a << " b=" << b << " c=" << c << " d=" << d << endl;
    cout << "Результат (ASM): " << res_asm << endl;
    cout << "Результат (C++): " << res_cpp << endl;
    cout << "Разница: " << fabs(res_asm - res_cpp) << endl << endl;

    return 0;
}