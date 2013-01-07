#include "matrice.hpp"

#include <fstream>
#include <iostream>

Matrice::Matrice(int x,int y):taillex(x),tailley(y)
{
    mat = new int[taillex*tailley];
}

int Matrice::get(int x, int y) const
{
    return mat[x*tailley + y];
}

Matrice::Matrice(string fic):fic(fic)
{
    ifstream f(fic.c_str(),ifstream::in);
    string buf;
    f>>buf;
    f>>tailley;
    f>>buf;
    f>>taillex;
    f>>buf;
    f>>xllcorner;
    f>>buf;
    f>>yllcorner;
    f>>buf;
    f>>cellsize;
    f>>buf;
    f>>nodata;
    max = nodata;
    mat = new int[taillex*tailley];
    for(int i=0;i<taillex;i++)
    {
        for(int j=0;j<tailley;j++)
        {
            f>>mat[i*tailley + j];
            if(max < mat[i*tailley + j])
                max = mat[i*tailley + j];
        }
    }
    f.close();
    max += 1.0;
}

int * Matrice::getMat()
{
    return mat;
}

Matrice::~Matrice()
{
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

void Matrice::write()
{
    ofstream out(fic.c_str());
    out<<"ncols "<<Tailley()<<endl;
    out<<"nrows "<<Taillex()<<endl;
    out<<"xllcorner "<<xllcorner<<endl;
    out<<"yllcorner "<<yllcorner<<endl;
    out<<"cellsize "<<cellsize<<endl;
    out<<"NODATA_value "<<Nodata()<<endl;
    for(int i=0;i<Taillex();i++)
    {
        for(int j=0;j<Tailley();j++)
        {
            out<<mat[i*tailley + j]<<" ";
        }
        out<<endl;
    }
    out.close();
}
