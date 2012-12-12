#include <iostream>
#include "matrice.hpp"
#include "mpi.h"

using namespace std;

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
    recv = new int[taillebande];
    MPI_Scatter(mnt,taillebande,MPI_INT,recv,taillebande,MPI_INT,0,MPI_COMM_WORLD);


    MPI_Finalize();
    return 0;
}

