#ifndef MATRICE_HPP
#define MATRICE_HPP

#include <string>

using namespace std;

class Matrice
{
    int * mat;
    int taillex,tailley,nodata,cellsize,xllcorner,yllcorner;
    float max;
    string fic;
public:
    Matrice(int x,int y);
    Matrice(string fic);
    ~Matrice();
    int get(int x,int y) const;
    int *getMat();
    int Nodata() const;
    int Taillex() const;
    int Tailley() const;
    float Max() const;
    void write();
};

ostream& operator<<( ostream &flux,  const Matrice & matrice );

#endif // MATRICE_HPP
