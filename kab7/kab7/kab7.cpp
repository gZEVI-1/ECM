#include <iostream>
#include <cmath>
#include <cfloat>

using namespace std;

double f(double X) {
    double x = X;
    unsigned short status;
    double y = 0.0;

    __asm {
        finit
        fld qword ptr[x]; ST(0) = x

        ; x ^ 2
        fld st(0)
        fmul st(0), st(1); ST(0) = x ^ 2, ST(1) = x

        ; 3x
        fld qword ptr[x]
        fld st(0)
        fadd st(0), st(1)
        fadd st(0), st(1); ST(0) = 3x, ST(1) = x, ST(2) = x ^ 2, ST(3) = x

        ; переставляем: [x ^ 2, x, 3x]
        fxch st(2); ST(0) = x ^ 2, ST(1) = x, ST(2) = 3x, ST(3) = x
        fxch st(1); ST(0) = x, ST(1) = x ^ 2, ST(2) = 3x, ST(3) = x
        fxch st(2); ST(0) = 3x, ST(1) = x ^ 2, ST(2) = x, ST(3) = x

        ; сохраняем 3x
        fst qword ptr[x]
        fxch st(2); ST(0) = x, ST(1) = x ^ 2, ST(2) = 3x

        ; e^ x 
        fldl2e
        fmul st(0), st(1)
        fld st(0)
        frndint
        fsub st(1), st(0)
        fxch st(1)
        f2xm1
        fld1
        faddp st(1), st(0)
        fscale
        fstp st(1); ST(0) = e ^ x, ST(1) = x, ST(2) = x ^ 2

        ; выгружаем 3x
        fld qword ptr[x]; ST(0) = 3x, ST(1) = e ^ x, ST(2) = x, ST(3) = x ^ 2

        ; e^ x * 3x
        fmul st(0), st(1); ST(0) = e ^ x * 3x, ST(1) = e ^ x, ST(2) = x, ST(3) = x ^ 2

        ; удаляем лишний e^ x
        fxch st(1)
        fstp st(0); ST(0) = e ^ x * 3x, ST(1) = x, ST(2) = x ^ 2

        ; удаляем x
        fxch st(1)
        fstp st(0); ST(0) = e ^ x * 3x, ST(1) = x ^ 2

        ; x ^ 2 + e ^ x * 3x
        faddp st(1), st(0); ST(0) = x ^ 2 + e ^ x * 3x - арг

        ; дублируем аргумент для проверки
        fld st(0)

        ; проверяем аргумент > 0
        ftst
        fstsw status
        mov ax, status
        sahf

        ; если аргумент <= 0, то C3 = 1 или C0 = 1
        ; C3 = 1 если ST(0) = 0
        ; C2 = 1 если ST(0) неопределен
        ; C0 = 1 если ST(0) < 0
        test ax, 01000101b; проверяем биты C3, C0, C2
        jnz error_ln; если любой из этих битов установлен => аргумент <= 0

        ; ln(arg> 0)
        fldln2
        fxch st(1)
        fyl2x; ST(0) = ln()

        ;arctg(ln())
        fld1
        fpatan; ST(0) = arctg(ln())
        jmp finish

        error_ln :
            finit

            ; cоздаем NaN
            fld qword ptr[x]
            fdiv st(0), st(0)

        finish :
            fstp qword ptr[y]
            fstsw status
    }

    cout << "SWR (hex): 0x" << hex << status << dec << endl;

    if (status & 0x0001) cout << "- IE (Invalid operation) установлен\n";
    if (status & 0x0002) cout << "- DE (Denormal operand) установлен\n";
    if (status & 0x0004) cout << "- ZE (Divide by zero) установлен\n";
    if (status & 0x0008) cout << "- OE (Overflow) установлен\n";
    if (status & 0x0010) cout << "- UE (Underflow) установлен\n";
    if (status & 0x0020) cout << "- PE (Precision) установлен\n";

    if ((status & 0x003F) == 0) {
        cout << "Исключительные ситуации отсутствуют\n";
    }

    return y;
}

double f_cpp(double x) {
   
    double result ;

    double x_squared = x * x;
    double three_x = 3.0 * x;
    double exp_x = exp(x);

    double arg_ln = x_squared + exp_x * three_x;

    if (arg_ln <= 0.0) {
        result = nan("1");  // NaN
    }
    else {
        double ln_val = log(arg_ln);
        result = atan(ln_val);
    }

    return result;
} 


int main() {
    setlocale(LC_ALL, "Russian");
    double x;
    cout << "----------- y = arctg(ln(x^2 + e^x * (3x))) -----------------\n";
    cout << "Enter x: ";
    cin >> x;
    cout << '\n';
    double y = f(x);
    cout << '\n';


    cout << "\n\t\tASM\n\n";

    if (_isnan(y)) {
        cout << "Результат: NaN (не число)\n";
    }
    else if (!_finite(y)) {
        if (y == HUGE_VAL || y == -HUGE_VAL) {
            cout << "Результат: бесконечность\n";
        }
        else {
            cout << "Результат: переполнение\n";
        }
    }
    else {
        cout << "Результат: конечное число = " << y << endl;
    }


    cout << "\n\tПроверка через с++\n"<<"\nРезультат:" << f_cpp(x)<<endl;

    return 0;
}