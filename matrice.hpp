#ifndef MATRICE_HPP
#define MATRICE_HPP

#include <string>

using namespace std;

class Matrice
{
    float * mat;
    int taillex,tailley,nodata,cellsize,xllcorner,yllcorner;
    float max;
    string fic;
public:
    Matrice(int x,int y);
    Matrice(string fic);
    Matrice(float * tab,int x,int y);
    ~Matrice();
    float get(int x,int y) const;
    float *getMat();
    int Nodata() const;
    int Taillex() const;
    int Tailley() const;
    float Max() const;
    void set(int x, int y, float val);
    void write();
    int getvoisin(int x,int y,float * tab);
    void setmatnull();
    void writeetape1(float * res);
    void writeetape2(float * res);
    void writeetape3(float * res);
};

ostream& operator<<( ostream &flux,  const Matrice & matrice );

#endif // MATRICE_HPP
