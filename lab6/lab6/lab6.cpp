#include <cmath>
#include <limits>
#include <iomanip>
#include <iostream>

using namespace std;

// Специальные значения для маркировки разных типов ошибок
const double INF_ZE = numeric_limits<double>::infinity();  // деление на ноль
const double INF_OE = 1e300 * 1e300;  // переполнение (тоже inf, но мы будем различать по флагу)

double calculate_cpp(double a, double b) {
    if (isnan(a) || isnan(b)) {
        return numeric_limits<double>::quiet_NaN();
    }
    if (a > b) {
        if (a == 0.0) {
            return numeric_limits<double>::infinity();  // INF (ZE)
        }
        // Проверка на переполнение
        double temp = (7.0 * a - 2.0 * b) / a + 4.0;
        if (isinf(temp)) {
            return numeric_limits<double>::infinity();  // INF (OE)
        }
        return temp;
    }
    else if (a == b) {
        double temp = -a * a * a;
        if (isinf(temp)) {
            return numeric_limits<double>::infinity();  // INF (OE)
        }
        return temp;
    }
    else {
        if (b == 0.0) {
            return numeric_limits<double>::infinity();  // INF (ZE)
        }
        double temp = (a - 25.0) / b;
        if (isinf(temp)) {
            return numeric_limits<double>::infinity();  // INF (OE)
        }
        return temp;
    }
}

double calculate_asm(double a, double b) {
    double result;
    double c7 = 7.0;
    double c2 = 2.0;
    double c4 = 4.0;
    double c25 = 25.0;
    double inf_val = numeric_limits<double>::infinity();
    double nan_val = numeric_limits<double>::quiet_NaN();
    unsigned short control_word;
    unsigned short status_word;

    __asm {
        finit;

        fstcw word ptr[control_word];
        and control_word, 0xF3FF;  // сбросить биты маски
        fldcw word ptr[control_word];

        //проверка NaN для a 
        fld qword ptr[a];
        fld st(0);
        fcompp;
        fstsw ax;
        sahf;
        jp nan_result;

        //проверка NaN для b
        fld qword ptr[b];
        fld st(0);
        fcompp;
        fstsw ax;
        sahf;
        jp nan_result;

        fld qword ptr[a];    // st(0) = a
        fld qword ptr[b];    // st(0) = b, st(1) = a

        fcom st(1);          // сравнить b и a
        fstsw ax;
        sahf;
        je equal_case;       // a == b
        ja greater_case;     // b > a => a < b

        ;// a > b: (7a - 2b)/a + 4

        fldz;
        fucom st(2);         // сравнить 0 с a
        fstsw ax;
        sahf;
        je div_zero_error_ze;   // a == 0 - деление на ноль

        fstp st(0);          // сбросить 0, стек [b, a]

        ;// 7a
        fld qword ptr[a];
        fmul qword ptr[c7];  // st(0)=7a

        fstsw word ptr[status_word];
        test status_word, 8;  // бит C1 (переполнение/недополнение)
        jnz overflow_error_oe;

        ;// 2b
        fld qword ptr[b];
        fmul qword ptr[c2];  // st(0)=2b, st(1)=7a

        ;// 7a - 2b
        fsubp st(1), st;     // st(0)=7a-2b

        fld qword ptr[a];
        fdivp st(1), st;     // st(0)=(7a-2b)/a

        fstsw word ptr[status_word];
        test status_word, 8;
        jnz overflow_error_oe;

        ;// +4
        fadd qword ptr[c4];

        fstsw word ptr[status_word];
        test status_word, 8;
        jnz overflow_error_oe;

        jmp save_result;

    greater_case:
        ;// a < b: (a-25)/b

        fldz;
        fucom st(1);         // сравнить 0 с b
        fstsw ax;
        sahf;
        je div_zero_error_ze;   // b == 0 - деление на ноль

        fstp st(0);          // сбросить 0, стек [b, a]

        ;// a - 25
        fxch st(1);          // st(0)=a, st(1)=b
        fld qword ptr[c25];
        fsubp st(1), st;     // st(0)=a-25, st(1)=b

        ;// (a-25)/b
        fdivrp st(1), st;    // st(0)=(a-25)/b

        fstsw word ptr[status_word];
        test status_word, 8;
        jnz overflow_error_oe;

        jmp save_result;

    equal_case:
        fstp st(0);          // сбросить b, стек [a]
        fld st(0);           // st(0)=a, st(1)=a
        fmul st, st(1);      // st(0)=a*a
        fmulp st(1), st;     // st(0)=a*a*a
        fchs;                // st(0)=-a*a*a

        fstsw word ptr[status_word];
        test status_word, 8;
        jnz overflow_error_oe;

        jmp save_result;

    div_zero_error_ze:
        fstp st(0);          // сбросить b
        fstp st(0);          // сбросить a
        fstp st(0);
        fstp st(0);
        fld qword ptr[inf_val];
        // Здесь НЕ используем дополнительный флаг, просто inf (ZE)
        jmp save_result;

    overflow_error_oe:
        fstp st(0);          // сбросить текущий результат
        fstp st(0);          // очистить оставшиеся
        fstp st(0);
        fstp st(0);
        fld qword ptr[inf_val];
        // Здесь НЕ используем дополнительный флаг, просто inf (OE)
        jmp save_result;

    nan_result:
        fstp st(0);
        fld qword ptr[nan_val];
        jmp save_result;

    save_result:
        fstp qword ptr[result];
    }

    return result;
}


int main() {
    setlocale(LC_ALL, "Russian");

    struct TestCase {
        double a, b;
        const char* desc;
        const char* expected;
        bool is_zero_division;  // true = ожидается деление на ноль (ZE)
        bool is_overflow;        // true = ожидается переполнение (OE)
    };

    TestCase tests[] = {
        {5.0, 2.0,     "Обычный случай a > b",           "10.2", false, false},
        {3.0, 3.0,     "Граничный случай a == b",        "-27.0", false, false},
        {10.0, 20.0,   "Обычный случай a < b",           "-0.75", false, false},
        {0.0, -1.0,    "Особая ситуация: a=0 (деление на 0)", "INF (ZE)", false, false},
        {10.0, 0.0,    "Особая ситуация: b=0 (деление на 0)", "11.0", false, false},
        {1e308, 1.0,   "Переполнение при умножении",      "INF (OE)", false, true},
        {-5.0, -10.0,  "Отрицательные числа, a > b",      "7.0", false, false},
        {1.0, 1.0,     "Малые равные значения",           "-1.0", false, false},
        {25.0, 30.0,   "Числитель = 0 в ветке a < b",     "0.0", false, false},
        {-1.0, 1.0,    "Отрицательный результат",         "-26.0", false, false},
        {1e-200, 1e200,"Антипереполнение при делении",    "0.0 (UE)", false, false},
        {numeric_limits<double>::quiet_NaN(), 1.0, "NaN в a", "NaN (IE)", false, false},
        {1.0, numeric_limits<double>::quiet_NaN(), "NaN в b", "NaN (IE)", false, false},
        {numeric_limits<double>::quiet_NaN(), numeric_limits<double>::quiet_NaN(), "NaN в обоих", "NaN (IE)", false, false}
    };

    const int num_tests = sizeof(tests) / sizeof(tests[0]);
    int passed = 0, failed = 0;

    for (int i = 0; i < num_tests; i++) {
        cout << endl << "+-+-+-+-+-+-+-+-+-+-+-+-+\n" << "Тест " << setw(2) << (i + 1) << "/" << num_tests
            << ": " << tests[i].desc << endl;
        cout << "Параметры: a = " << tests[i].a << ", b = " << tests[i].b << endl;
        cout << "Ожидается: " << tests[i].expected << endl << endl;

        double r_cpp = calculate_cpp(tests[i].a, tests[i].b);
        double r_asm = calculate_asm(tests[i].a, tests[i].b);

        cout << "[C++] результат:  ";
        if (tests[i].is_zero_division && isinf(r_cpp)) {
            cout << "INF (ZE)";
        }
        else if (tests[i].is_overflow && isinf(r_cpp)) {
            cout << "INF (OE)";
        }
        else if (isnan(r_cpp)) {
            cout << "NaN (IE)";
        }
        else {
            cout << r_cpp;
        }
        cout << endl;

        cout << "[ASM] результат:  ";
        if (tests[i].is_zero_division && isinf(r_asm)) {
            cout << "INF (ZE)";
        }
        else if (tests[i].is_overflow && isinf(r_asm)) {
            cout << "INF (OE)";
        }
        else if (isnan(r_asm)) {
            cout << "NaN (IE)";
        }
        else {
            cout << r_asm;
        }
        cout << endl << endl;

        bool match = false;

        // Проверка для деления на ноль (ZE)
        if (tests[i].is_zero_division) {
            if (isinf(r_cpp) && isinf(r_asm)) {
                match = true;
            }
        }
        // Проверка для переполнения (OE)
        else if (tests[i].is_overflow) {
            if (isinf(r_cpp) && isinf(r_asm)) {
                match = true;
            }
        }
        // Обычные случаи
        else if (isfinite(r_cpp) && isfinite(r_asm)) {
            double diff = abs(r_cpp - r_asm);
            cout << "Разница:        " << scientific << diff << fixed << endl;
            if (diff < 1e-9) {
                match = true;
            }
        }
        else if (isnan(r_cpp) && isnan(r_asm)) {
            match = true;
        }
        else if (isinf(r_cpp) && isinf(r_asm)) {
            if ((r_cpp > 0 && r_asm > 0) || (r_cpp < 0 && r_asm < 0)) {
                match = true;
            }
        }

        if (match) {
            cout << "[PASS] Результаты совпадают!" << endl;
            passed++;
        }
        else {
            cout << "[FAIL] Результаты НЕ совпадают!" << endl;
            failed++;
        }
    }

    cout << "\nИтого: " << passed << " passed, " << failed << " failed из " << num_tests << endl;
    return 0;
}