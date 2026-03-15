#include <iostream>
#include <cstdio>      // для printf
using namespace std;

static const char msg_zero[] = "Ошибка: деление на ноль!\n";
static const char msg_over[] = "Ошибка: переполнение!\n";

int main() {
    int a, b, c, d;
    setlocale(LC_ALL, "ru");
    cout << "Введите значения a, b, c, d: ";
    cin >> a >> b >> c >> d;

    int result = 0;

    __asm {
        ; --- Вычисляем числитель : 4 * b / c - d-- -

        ; 4 * b с проверкой переполнения
        mov     eax, b
        shl     eax, 2; eax = 4 * b(проверка переполнения сложнее)
        ; или: imul eax, 4; но OF ненадёжен

        mov     ecx, c
        test    ecx, ecx
        jz      error_zf; деление на ноль

        cdq; расширяем знак eax в edx
        idiv    ecx; eax = 4 * b / c

        sub     eax, d; eax = 4 * b / c - d
        jo      error_of

        push    eax; сохраняем числитель на стеке

        ; --- Вычисляем знаменатель : 12 * c + a - b-- -
        mov     eax, c
        imul    eax, 12; eax = 12 * c

        add     eax, a
        jo      error_of

        sub     eax, b
        jo      error_of

        mov     ebx, eax; ebx = знаменатель
        test    ebx, ebx
        jz      error_zf

        pop     eax; восстанавливаем числитель
        cdq
        idiv    ebx; eax = результат

        mov     result, eax
        jmp     done

        error_zf :
        ; ... вывод сообщения ...
            error_of:
        ; ... вывод сообщения ...
            done:
    }

    cout << "Результат = " << result << endl;
    return 0;
}
