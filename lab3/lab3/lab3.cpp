#include <iostream>
using namespace std;

// X = (7a - 2b)/a + 4, если a > b
// X = -a^3, если a = b
// X = (a - 25)/b, если a < b

bool Continue() {
    char answ;
    cout << "Continue? (Y/N):";
    while (!(cin >> answ) || (answ != 'Y' && answ != 'N' && answ != 'y' && answ != 'n' && answ != '1' && answ != '0') || (cin.peek() != '\n')) {
        cin.clear();
        while (cin.get() != '\n');
        cout << "Введите правильный ответ\n" << "Continue? (Y/N):";
    }
    if (answ == 'N' || answ == 'n' || answ == '0') { return false; }
    system("cls");
    return true;
}

int computeX(int a, int b) {
    int result = 0; 
    bool overflow = false; 
    bool division_by_zero = false; 

    __asm {
        
        mov eax, a; eax = a
        mov ebx, b; ebx = b


        cmp eax, ebx; сравнение a и b
        jg case_g; переход, если a > b
        jl case_l; переход, если a < b

        ; Случай равенства 
        ; вычисляем - a ^ 3
        imul eax, eax; eax = a * a
        imul eax, a; eax = a * a * a = a ^ 3
        imul eax,  -1; eax = -a ^ 3
        jmp done; переход к завершению

        case_g :
       
            ;(7a - 2b) / a + 4
            mov ecx, eax; ecx = a
            mov edx, 7; edx = 7
            imul edx, eax; edx = 7 * a
            mov eax, b; eax = b
            mov ebx, 2; ebx = 2
            imul ebx, eax; ebx = 2 * b
            mov eax, edx; eax = 7a
            sub eax, ebx; eax = 7a - 2b

            ; Проверка деления на ноль
            cmp ecx, 0; сравнение a с 0
            je div_by_zero_err

            ;(7a - 2b) / a
            cdq; расширение eax->edx:eax для idiv
            idiv ecx; eax = (7a - 2b) / a, edx = остаток
            add eax, 4; eax = (7a - 2b) / a + 4

            
            jo overflow_err; если переполнение
            jmp done

        case_l :
        
            ;(a - 25) / b
            mov ecx, ebx; ecx = b
            mov eax, a; eax = a
            sub eax, 25; eax = a - 25

            ; Проверка деления на ноль
            cmp ecx, 0; сравнение b с 0
            je div_by_zero_err

            ;(a - 25) / b
            cdq; расширение eax->edx:eax 
            idiv ecx;  остаток в  edx
            jmp done

        div_by_zero_err :
            mov division_by_zero, 1 
            mov eax, 0
            jmp done

        overflow_err :
            mov overflow, 1
            mov eax, 0

        done :
            mov result, eax
    }

    
    if (division_by_zero) {
        cout << "Ошибка: деление на ноль!\n";
    }
    else if (overflow) {
        cout << "Ошибка: переполнение!\n";
    }
    else {
        cout << "X = " << result << endl;
    }

    return result;
}

void CPP_calc(int a, int b) {
    int result = 0;


    if (a > b) {// X = (7a - 2b)/a + 4, если a > b
        result = (7*a - 2*b) / a + 4;
    }
    else if (a == b) {// X = -a^3, если a = b
        result = -(a * a * a);
    }
    else {// X = (a - 25)/b, если a < b
        result = (a - 25) / b;
    }

    cout << "Ркзультат с++\n";
    cout << "X = " << result << endl;
   
}

int main() {
    setlocale(LC_ALL, "ru");
    do{
        int a, b;
        cout << "Введите a и b: ";
        cin >> a >> b;
        computeX(a, b);

        CPP_calc(a, b);
    } while (Continue());

    return 0;
}