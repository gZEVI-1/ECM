
#include<iostream>
#include<fstream>
#include<cstring>
#include<conio.h>
#include<cstdlib>
using namespace std;


extern "C"
{
    int calcSumm(int, int);
}
int main()
{
    int k, a, b;
    //std::ifstream in("myin.txt"); //Открываем файл для считывания информации 
    //in >> a >> b; //считываем intы
    //in.close(); // Закрываем файл
    cout << "a:";
    cin >> a;;
    cout << "\nb:";
    cin >> b;


    
    k = calcSumm(a, b);

    std::ofstream out("myout.txt"); // Открываем файл для записи
    std::cout <<'\n' << k << '\n';
    out << k << "\n"; //Записываем первую строчку

    out.close(); //Закрываем файл

    return 0;
}

