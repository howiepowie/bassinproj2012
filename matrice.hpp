#ifndef MATRICE_HPP
#define MATRICE_HPP

#include <string>

using namespace std;

class Matrice
{
    int ** mat;
    int taillex,tailley,nodata;

public:
    Matrice(int x,int y);
    Matrice(string fic);
    ~Matrice();
    int *getLine(int x);
    int get(int x,int y) const;
    int **getMat();
    int Nodata() const;
    int Taillex() const;
    int Tailley() const;
};

ostream& operator<<( ostream &flux,  const Matrice & matrice );

#endif // MATRICE_HPP
