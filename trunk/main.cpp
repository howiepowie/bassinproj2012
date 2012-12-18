#include <iostream>
#include "matrice.hpp"
#include "mpi.h"

using namespace std;

void etape1(int * tab,int rang, int nprocs,int taillex,int tailley)
{

}

void etape2(int * tab,int rang, int nprocs,int taillex,int tailley)
{

}

void etape3(int * tab,int rang, int nprocs,int taillex,int tailley)
{

}

int main(int argc,char **argv)
{
    int rang,nprocs,*recv,taillex,tailley,*mnt;
    float epsilon=0.01;
    Matrice * mat;
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&rang);
    if(rang == 0)
    {
        mat = new Matrice("ex.txt");
        taillex = mat->Taillex();
        tailley = mat->Tailley();
        mnt = mat->getMat();
    }
    MPI_Bcast(&taillex,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(&tailley,1,MPI_INT,0,MPI_COMM_WORLD);
    int taillebande = (taillex/nprocs) * tailley;
    int reste;

    rang == (nprocs-1)?(taillex/nprocs) * tailley:0;

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
    etape1(recv,rang,nprocs,taillebande+reste,tailley);
    etape2(recv,rang,nprocs,taillebande+reste,tailley);
    etape3(recv,rang,nprocs,taillebande+reste,tailley);

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

