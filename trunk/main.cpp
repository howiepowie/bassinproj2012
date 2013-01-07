#include <iostream>
#include "matrice.hpp"
#include "mpi.h"

using namespace std;

void etape1(int * tab,int rang, int nprocs,int taillex,int tailley,float max)
{
    /*    int * bn,* bs, poss = (taillex-1)*tailley;
    bn = new int[tailley];
    bs = new int[tailley];
    //échange des bords
    if(rang==0)
        MPI_Sendrecv(tab+poss,tailley,MPI_INT,1,MPI_ANY_TAG,bs,tailley,MPI_INT,1,MPI_ANY_TAG,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    else if(rang == nprocs-1)
        MPI_Sendrecv(tab,tailley,MPI_INT,rang-1,MPI_ANY_TAG,bn,tailley,MPI_INT,rang-1,MPI_ANY_TAG,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    else
    {
        MPI_Sendrecv(tab,tailley,MPI_INT,rang-1,MPI_ANY_TAG,bs,tailley,MPI_INT,rang+1,MPI_ANY_TAG,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        MPI_Sendrecv(tab+poss,tailley,MPI_INT,rang+1,MPI_ANY_TAG,bn,tailley,MPI_INT,rang-1,MPI_ANY_TAG,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    }*/
    float epsilon=0.01;
    float * w;
    bool condition = true;
    for(int i=0;i<taillex;i++)
    {
        for(int j=0;j<tailley;j++)
        {
            if(i == 0 || i == taillex-1)
                w[j+tailley*i] = tab[j+tailley*i] + .0;
            else
            {
                if(j == 0 || j == tailley-1)
                    w[j+tailley*i]= tab[j+tailley*i] + .0;
                else
                    w[j + tailley*i] = max;
            }
        }
    }
    while(condition)
    {
        condition = false;
        for(int i = 1;i<taillex-1;i++)
        {
            for(int j = 1;j<tailley-1;j++)
            {
                if(w[i*tailley+j]>tab[i*tailley+j])
                {

                }
            }
        }
    }
}

void etape2(int * tab,int rang, int nprocs,int taillex,int tailley)
{
    /*    int * bn,* bs,*dir, poss = (taillex-1)*tailley,dir;
    bn = new int[tailley];
    bs = new int[tailley];
    dir = new int[taillex*tailley];
    //échange des bords
    if(rang==0)
        MPI_Sendrecv(tab+poss,tailley,MPI_INT,1,MPI_ANY_TAG,bs,tailley,MPI_INT,1,MPI_ANY_TAG,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    else if(rang == nprocs-1)
        MPI_Sendrecv(tab,tailley,MPI_INT,rang-1,MPI_ANY_TAG,bn,tailley,MPI_INT,rang-1,MPI_ANY_TAG,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    else
    {
        MPI_Sendrecv(tab,tailley,MPI_INT,rang-1,MPI_ANY_TAG,bs,tailley,MPI_INT,rang+1,MPI_ANY_TAG,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        MPI_Sendrecv(tab+poss,tailley,MPI_INT,rang+1,MPI_ANY_TAG,bn,tailley,MPI_INT,rang-1,MPI_ANY_TAG,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    }*/
    for(int i = 0; i<taillex;i++)
    {
        for(int j = 0; j<tailley; j++)
        {
            if(i == 0)
            {
                if(rang == 0)
                {
                    if(j != (tailley-1))
                    {
                        if(i != taillex-1)
                        {
                            min = 0;
                        }
                    }
                }
                else
                {

                }
            }
        }
    }
}

void etape3(int * tab,int rang, int nprocs,int taillex,int tailley)
{
    /*    int * bn,* bs, poss = (taillex-1)*tailley;
    bn = new int[tailley];
    bs = new int[tailley];
    //échange des bords
    if(rang==0)
        MPI_Sendrecv(tab+poss,tailley,MPI_INT,1,MPI_ANY_TAG,bs,tailley,MPI_INT,1,MPI_ANY_TAG,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    else if(rang == nprocs-1)
        MPI_Sendrecv(tab,tailley,MPI_INT,rang-1,MPI_ANY_TAG,bn,tailley,MPI_INT,rang-1,MPI_ANY_TAG,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    else
    {
        MPI_Sendrecv(tab,tailley,MPI_INT,rang-1,MPI_ANY_TAG,bs,tailley,MPI_INT,rang+1,MPI_ANY_TAG,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        MPI_Sendrecv(tab+poss,tailley,MPI_INT,rang+1,MPI_ANY_TAG,bn,tailley,MPI_INT,rang-1,MPI_ANY_TAG,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    }*/

}

int main(int argc,char **argv)
{
    int rang,nprocs,*recv,taillex,tailley,*mnt;
    float max;
    Matrice * mat;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&rang);
    if(rang == 0)
    {
        mat = new Matrice("ex.txt");
        taillex = mat->Taillex();
        tailley = mat->Tailley();
        max = mat->Max();
        mnt = mat->getMat();
    }
    MPI_Bcast(&taillex,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(&tailley,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(&max,1,MPI_FLOAT,0,MPI_COMM_WORLD);

    int taillebande = (taillex/nprocs) * tailley;
    int reste;

    reste  = rang == (nprocs-1)?(taillex/nprocs) * tailley:0;

    if(rang == (nprocs-1))
        recv = new int[taillebande + reste];
    else
        recv = new int[taillebande];
    MPI_Scatter(mnt,taillebande,MPI_INT,recv,taillebande,MPI_INT,0,MPI_COMM_WORLD);
    if(reste != 0)
    {
        if(rang == 0)
            MPI_Send(mnt + taillebande*nprocs,reste,MPI_INT,nprocs-1,MPI_ANY_TAG,MPI_COMM_WORLD);
        else if(rang == nprocs-1)
            MPI_Recv(recv+taillebande,taillebande,MPI_INT,0,MPI_ANY_TAG,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    }
    etape1(recv,rang,nprocs,taillebande+reste,tailley,max);
    //    etape2(recv,rang,nprocs,taillebande+reste,tailley);
    //    etape3(recv,rang,nprocs,taillebande+reste,tailley);

    MPI_Gather(recv,taillebande,MPI_INT,mnt,taillebande,MPI_INT,0,MPI_COMM_WORLD);
    if(reste != 0)
    {
        if(rang == 0)
            MPI_Recv(mnt + taillebande*nprocs,reste,MPI_INT,nprocs-1,MPI_ANY_TAG,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        else if(rang == nprocs-1)
            MPI_Send(recv+taillebande,taillebande,MPI_INT,0,MPI_ANY_TAG,MPI_COMM_WORLD);
    }
    if(rang==0)
    {
        mat->write();
    }
    MPI_Finalize();
    return 0;
}

