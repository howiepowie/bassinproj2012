#include "matrice.hpp"

#include <fstream>
#include <iostream>

Matrice::Matrice(int x,int y):taillex(x),tailley(y)
{
    mat = new int*[taillex];
    for(int i=0;i<taillex;i++)
    {
        mat[i]=new int[tailley];
    }
}

int * Matrice::getLine(int x)
{
    return mat[x];
}

int Matrice::get(int x, int y) const
{
    return mat[x][y];
}

Matrice::Matrice(string fic)
{
    ifstream f(fic.c_str(),ifstream::in);
    string buf;
    int tmp;
    f>>buf;
    f>>tailley;
    f>>buf;
    f>>taillex;
    f>>buf;
    f>>tmp;
    f>>buf;
    f>>tmp;
    f>>buf;
    f>>tmp;
    f>>buf;
    f>>nodata;
    mat = new int*[taillex];
    for(int i=0;i<taillex;i++)
    {
        mat[i] = new int[tailley];
    }
    for(int i=0;i<taillex;i++)
    {
        for(int j=0;j<tailley;j++)
        {
            f>>mat[i][j];
        }
    }
}

int ** Matrice::getMat()
{
    return mat;
}

Matrice::~Matrice()
{
    for(int i =0;i<taillex;i++)
    {
        delete [] mat[i];
    }
    delete [] mat;
}

int Matrice::Nodata() const
{
    return nodata;
}

int Matrice::Taillex() const
{
    return taillex;
}

int Matrice::Tailley() const
{
    return tailley;
}

ostream& operator<<( ostream &flux,  const Matrice & matrice )
{
    flux<<"nrows ="<<matrice.Taillex()<<endl;
    flux<<"nclos ="<<matrice.Tailley()<<endl;
    flux<<"nodata value ="<<matrice.Nodata()<<endl;
    for(int i=0;i<matrice.Taillex();i++)
    {
        for(int j=0;j<matrice.Tailley();j++)
        {
            flux<<matrice.get(i,j)<<" ";
        }
        flux<<endl;
    }
    return flux;
}

