#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include <iostream>
using  namespace std;
void main()
{
    char s[20];
    int a, b, sum;
    CharToOem(_T("Введите "), s);
    //printf("%s a: ", s); 
    cout << "a:";

    //scanf("%d", &a);
    cin >> a;

    //printf("%s b: ", s);
    cout << "\nb:";

    //scanf("%d", &b);
    cin >> b;

    _asm
    {
        mov eax, a;
        mov ecx, 5
            m: add eax, b
            loop m
            mov sum, eax
    }
    printf("\n %d + 5*%d = %d", a, b, sum);
    
}