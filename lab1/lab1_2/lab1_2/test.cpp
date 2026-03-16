#include<iostream>

using namespace std;
int main()
{
    int a, b, of = 0, result;
    cout << "Enter 'a' num: ";
    cin >> a;
    cout << "Enter 'b' num: " ;
    cin >> b;
    cout << '\n';
    __asm {
        mov eax, a; eax = a
        mov ebx, b; ebx = b
        add eax, ebx; eax = a + b
        jo    error_of; ошибка переполнения

        mov result, eax;
        jmp exit1
            error_of :
        mov of, 1; флаг ошибки переполнения
            exit1 :
    }
    if (of == 1)
        cout << endl << "Out of size!" << endl;
    if (of == 0)
        cout << "Result __asm: " << result << endl;

    system("pause");
    return 0;
}
