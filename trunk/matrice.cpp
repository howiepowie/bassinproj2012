#include "matrice.hpp"

#include <fstream>
#include <iostream>

Matrice::Matrice(int x,int y):taillex(x),tailley(y)
{
    mat = new float[taillex*tailley];
}

float Matrice::get(int x, int y) const
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
    mat = new float[taillex*tailley];
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

float * Matrice::getMat()
{
    return mat;
}

Matrice::~Matrice()
{
    if(mat != NULL)
        delete [] mat;
}

void Matrice::setmatnull()
{
    mat = NULL;
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

float Matrice::Max() const
{
    return max;
}

Matrice::Matrice(float *tab,int x,int y):taillex(x),tailley(y)
{
    mat = new float[x*y];
    for(int i=0;i<Taillex();i++)
    {
        for(int j=0;j<Tailley();j++)
        {
            mat[i*tailley+j] = tab[i*Tailley()+j];
        }
    }

}

int Matrice::getvoisin(int x, int y, float *tab)
{
    if(tab != NULL)
    {
        delete [] tab;
    }
    int res=0;
    tab = new float[8];
    if(x == 0)
    {
        if(y == 0)
        {
            tab[res]=get(x,y+1);res++;
            tab[res]=get(x+1,y+1);res++;
            tab[res]=get(x+1,y);res++;
        }
        else if(y == tailley-1)
        {
            tab[res]=get(x,y-1);res++;
            tab[res]=get(x+1,y);res++;
            tab[res]=get(x+1,y-1);res++;
        }
        else
        {
            tab[res]=get(x,y+1);res++;
            tab[res]=get(x+1,y+1);res++;
            tab[res]=get(x+1,y);res++;
            tab[res]=get(x,y-1);res++;
            tab[res]=get(x,y-1);res++;
        }
    }
    else if(x == tailley-1)
    {
        if(y == 0)
        {
            tab[res]=get(x,y+1);res++;
            tab[res]=get(x-1,y+1);res++;
            tab[res]=get(x-1,y);res++;
        }
        else if(y == tailley-1)
        {
            tab[res]=get(x,y-1);res++;
            tab[res]=get(x-1,y);res++;
            tab[res]=get(x-1,y-1);res++;
        }
        else
        {
            tab[res]=get(x,y+1);res++;
            tab[res]=get(x-1,y+1);res++;
            tab[res]=get(x-1,y);res++;
            tab[res]=get(x,y-1);res++;
            tab[res]=get(x-1,y-1);res++;
        }
    }
    else
    {
        if(y == 0)
        {
            tab[res]=get(x-1,y);res++;
            tab[res]=get(x-1,y+1);res++;
            tab[res]=get(x,y+1);res++;
            tab[res]=get(x+1,y+1);res++;
            tab[res]=get(x+1,y);res++;

        }
        else if(y == tailley-1)
        {
            tab[res]=get(x-1,y);res++;
            tab[res]=get(x-1,y-1);res++;
            tab[res]=get(x,y-1);res++;
            tab[res]=get(x+1,y-1);res++;
            tab[res]=get(x+1,y);res++;
        }
        else
        {
            tab[res]=get(x-1,y);res++;
            tab[res]=get(x-1,y+1);res++;
            tab[res]=get(x,y+1);res++;
            tab[res]=get(x+1,y+1);res++;
            tab[res]=get(x+1,y);res++;
            tab[res]=get(x-1,y-1);res++;
            tab[res]=get(x,y-1);res++;
            tab[res]=get(x+1,y-1);res++;
        }
    }
    return res;
}

void Matrice::set(int x, int y,float val)
{
    mat[x*tailley+y] = val;
}

void Matrice::writeetape1(float *res)
{
    ofstream out("etape1.txt");
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
            out<<res[i*tailley + j]<<" ";
        }
        out<<endl;
    }
    out.close();
}

void Matrice::writeetape2(float *res)
{
    ofstream out("etape2.txt");
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
            out<<res[i*tailley + j]<<" ";
        }
        out<<endl;
    }
    out.close();
}

void Matrice::writeetape3(float *res)
{
    ofstream out("etape3.txt");
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
            out<<res[i*tailley + j]<<" ";
        }
        out<<endl;
    }
    out.close();
}
