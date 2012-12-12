#include <iostream>
#include "matrice.hpp"

using namespace std;

int main()
{
    Matrice mat("ex.txt");
    cout<<"j'ai cree la matrice"<<endl;
    cout<<mat<<endl;
    return 0;
}

