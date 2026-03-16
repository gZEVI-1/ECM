#include<iostream>

using namespace std;
int main()
{
    int  result;
   
    __asm {
        N DB ?
        K DW ?
        mov AL, N;
        adc AL, 1;
        adc AH, 0; 
        movzx BX, N; 
        mul BX ;
        mov BX, 2;
        div BX;
        mov K, AX;
    }
    
    cout << "Result __asm: " << result << endl;

    system("pause");
    return 0;
}