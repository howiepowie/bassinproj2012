#include "matrice.hpp"
#include <mpi.h>
#include <iostream>

using namespace std;

#define eps 0.01

//echange des bordures
void echangerbords(float * send,int taillex, int tailley)
{
    int pid,nprocs;
    MPI_Comm_rank(MPI_COMM_WORLD,&pid);
    MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
    if(nprocs > 1)
    {
        if(pid == 0)
        {
            MPI_Sendrecv(send+(taillex-1)*tailley,tailley,MPI_FLOAT,1,
                         123,send+taillex*tailley,tailley,MPI_FLOAT,1,123,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        }
        else if(pid == nprocs-1)
        {
            MPI_Sendrecv(send+tailley,tailley,MPI_FLOAT,pid-1,123,
                         send,tailley,MPI_FLOAT,pid-1,123,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        }
        else
        {
            MPI_Sendrecv(send+tailley,tailley,MPI_FLOAT,pid-1,123,
                         send,tailley,MPI_FLOAT,pid-1,123,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            MPI_Sendrecv(send+taillex*tailley,tailley,MPI_FLOAT,pid+1,123,
                         send+(taillex+1)*tailley,tailley,MPI_FLOAT,pid+1,123,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
}

//distribution du tableau initiale et renvoie la taille bande distribué + le reste pour le dernier processeur
int distribue(float * send,float * recv, int taillex, int tailley)
{
    int pid,nprocs;
    MPI_Comm_rank(MPI_COMM_WORLD,&pid);
    MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
    int taillebande = taillex/nprocs, reste = taillex%nprocs;
    if(pid == 0)
        MPI_Scatter(send,taillebande*tailley,MPI_FLOAT,
                    recv,taillebande*tailley,MPI_FLOAT,0,MPI_COMM_WORLD);
    else
        MPI_Scatter(send,taillebande*tailley,MPI_FLOAT,
                    recv+tailley,taillebande*tailley,MPI_FLOAT,0,MPI_COMM_WORLD);
    if((pid == 0 || pid == nprocs-1) && reste != 0)
    {
        if(pid == 0)
            MPI_Send(send+(taillex-reste)*tailley,reste*tailley,
                     MPI_FLOAT,nprocs-1,123,MPI_COMM_WORLD);
        else
            MPI_Recv(recv+((taillebande+1)*tailley),reste*tailley,
                     MPI_FLOAT,0,123,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    if(pid == nprocs-1)
        return taillebande+reste;
    else
        return taillebande;
}

//rassemble le tableau
void rassemble(float * send,float * recv,int taillex,int tailley)
{
    int pid,nprocs;
    MPI_Comm_rank(MPI_COMM_WORLD,&pid);
    MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
    int taillebande = taillex/nprocs, reste = taillex%nprocs;
    if(pid != 0)
        MPI_Gather(send+tailley,taillebande*tailley,MPI_FLOAT,
                   recv,taillebande*tailley,MPI_FLOAT,0,MPI_COMM_WORLD);
    else
        MPI_Gather(send,taillebande*tailley,MPI_FLOAT,
                   recv,taillebande*tailley,MPI_FLOAT,0,MPI_COMM_WORLD);
    if((pid == 0 || pid == nprocs-1) && reste != 0)
    {
        if(pid == 0)
            MPI_Recv(recv+(taillex-reste)*tailley,reste*tailley,
                     MPI_FLOAT,nprocs-1,123,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        else
            MPI_Send(send+((taillebande+1)*tailley),reste*tailley,
                     MPI_FLOAT,0,123,MPI_COMM_WORLD);
    }
    MPI_Barrier(MPI_COMM_WORLD);
}

float * etape1(float * tab,int taillex,int tailley,float max)
{
    int pid,nprocs,test;
    MPI_Comm_rank(MPI_COMM_WORLD,&pid);
    MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
    float * w;
    int taillewbord; //la taille avec les bords
    if(pid == 0 || pid == nprocs-1)
        taillewbord = taillex+1;
    else
        taillewbord = taillex+2;
    w = new float[taillewbord*tailley]; //le tableau contenant le résultat
    const static int voisin[] = {-tailley-1,-tailley,-tailley+1,-1,1,tailley-1,tailley,tailley+1};
    echangerbords(tab,taillex,tailley);
    //initialisation du tableau w
    for(int i = 0;i<taillewbord;i++)
    {
        for(int j = 0;j<tailley;j++)
        {
            int c = i*tailley+j;
            if((i == 0 && pid ==0) || (i == taillewbord-1 && pid == nprocs-1))
            {
                w[c]=tab[c];
            }
            else
            {
                if(j == 0 || j == tailley-1)
                {
                    w[c] = tab[c];
                }
                else
                {
                    w[c] = max;
                }
            }
        }
    }
    bool condition=true;
    while(condition)
    {
        test = 0;
        echangerbords(w,taillex,tailley);
        condition = false;
        for(int i=1;i<taillewbord-1;i++)
        {
            for(int j=1;j<tailley-1;j++)
            {
                int c = i*tailley+j;
                if(w[c] > tab[c])
                {
                    for(int k = 0;k<8;k++)
                    {
                        if(tab[c] >= w[c+voisin[k]]+eps)
                        {
                            w[c] = tab[c];
                            test = 1;
                        }
                        else if(w[c] >  w[c+voisin[k]]+eps)
                        {
                            w[c] = w[c+voisin[k]]+eps;
                            test = 1;
                        }
                    }
                }
            }
        }
        //verification que tout le monde a fini si, après le Allreduce, test=0 tout le monde a fini
        MPI_Allreduce(&test,&test,1,MPI_INT,MPI_MAX,MPI_COMM_WORLD);
        if(test != 0)
            condition = true;
    }

    return w;
}

float * etape2(float * tab,int taillex,int tailley,float max)
{
    //echange des bords
    float * res = new float[taillex*tailley];
    int pid,nprocs,mini;
    MPI_Comm_rank(MPI_COMM_WORLD,&pid);
    MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
    int taillewbord;
    if(pid == 0)
    {
        taillewbord = taillex+1;
        max = taillewbord-1;
        mini = 0;
    }
    else if(pid == nprocs)
    {
        taillewbord = taillex+1;
        max = taillewbord;
        mini = 1;
    }
    else
    {
        taillewbord = taillex+2;
        mini = 1;
        max =taillewbord-1;
    }
    //const static int voisin[] = {-tailley-1,-tailley,-tailley+1,-1,1,tailley-1,tailley,tailley+1};
    for(int i =mini; i<max;i++)
    {
        for(int j=0; j<tailley;j++)
        {
            int c = i*tailley + j;
            float min = max;
            if( i == 0 && j==0)
            {
                if(min > tab[c])
                {
                    res[c]=0;
                    min=tab[c];
                }
                if(min > tab[c+1])
                {
                    res[c]=4;
                    min=tab[c+1];
                }
                if(min > tab[c+tailley])
                {
                    res[c]=6;
                    min=tab[c+tailley];
                }
                if(min > tab[c+tailley+1])
                {
                    res[c]=5;
                    min=tab[c+tailley+1];
                }
            }
            else if(i == 0 && j == tailley - 1 )
            {
                if(min > tab[c-1])
                {
                    res[c]=8;
                    min=tab[c-1];
                }
                if(min > tab[c])
                {
                    res[c]=0;
                    min=tab[c];
                }
                if(min > tab[c-tailley-1])
                {
                    res[c]=7;
                    min=tab[c+tailley-1];
                }
                if(min > tab[c+tailley])
                {
                    res[c]=6;
                    min=tab[c+tailley];
                }
            }
            else if(i == 0)
            {
                if(min > tab[c-1])
                {
                    res[c]=8;
                    min = tab[c-1];
                }
                if(min > tab[c])
                {
                    res[c]=0;
                    min = tab[c];
                }
                if(min > tab[c+tailley-1])
                {
                    res[c]=7;
                    min = tab[c+tailley-1];
                }
                if(min > tab[c+1])
                {
                    res[c]=4;
                    min = tab[c+1];
                }
                if(min > tab[c+tailley])
                {
                    res[c]=6;
                    min = tab[c+tailley];
                }
                if(min > tab[c+tailley+1])
                {
                    res[c]=5;
                    min = tab[c+tailley+1];
                }
            }
            else if(i == taillex-1 && j==0)
            {
                if(min > tab[c-tailley])
                {
                    res[c]=2;
                    min = tab[c-tailley];
                }
                if(min > tab[c-tailley+1])
                {
                    res[c]=3;
                    min = tab[c-tailley+1];
                }
                if(min > tab[c])
                {
                    res[c]=0;
                    min = tab[c];
                }
                if(min > tab[c+1])
                {
                    res[c]=4;
                    min =tab[c+1];
                }
            }
            else if(i == taillex-1 && j==tailley-1)
            {
                if(min > tab[c-tailley-1])
                {
                    min = tab[c-tailley-1];
                    res[c]=1;
                }
                if(min > tab[c-tailley])
                {
                    res[c]=2;
                    min = tab[c-tailley];
                }
                if(min > tab[c-1])
                {
                    res[c]=8;
                    min = tab[c-1];
                }
                if(min > tab[c])
                {
                    res[c]=0;
                    min = tab[c];
                }
            }
            else if(i == taillex-1)
            {
                if(min > tab[c-tailley-1])
                {
                    res[c]=1;
                    min = tab[c-tailley-1];
                }
                if(min > tab[c-tailley])
                {
                    res[c]=2;
                    min = tab[c-tailley];
                }
                if(min > tab[c-1])
                {
                    res[c]=8;
                    min = tab[c-1];
                }
                if(min > tab[c-tailley+1])
                {
                    res[c]=3;
                    min = tab[c-tailley+1];
                }
                if(min > tab[c])
                {
                    res[c]=0;
                    min = tab[c];
                }
                if(min > tab[c+1])
                {
                    res[c]=4;
                    min = tab[c+1];
                }
            }
            else if(j == 0)
            {
                if(min > tab[c-tailley])
                {
                    res[c]=2;
                    min = tab[c-tailley];
                }
                if(min > tab[c-tailley+1])
                {
                    res[c]=3;
                    min = tab[c-tailley+1];
                }
                if(min > tab[c])
                {
                    res[c]=0;
                    min = tab[c];
                }
                if(min > tab[c+1])
                {
                    res[c]=4;
                    min = tab[c+1];
                }
                if(min > tab[c+tailley])
                {
                    res[c]=6;
                    min = tab[c+tailley];
                }
                if(min > tab[c+tailley+1])
                {
                    res[c]=5;
                    min = tab[c+tailley+1];
                }
            }
            else if(j == tailley-1)
            {
                if(min > tab[c-tailley-1])
                {
                    res[c]=1;
                    min = tab[c+tailley-1];
                }
                if(min > tab[c-tailley])
                {
                    res[c]=2;
                    min = tab[c-tailley];
                }
                if(min > tab[c-1])
                {
                    res[c]=8;
                    min = tab[c-1];
                }
                if(min > tab[c])
                {
                    res[c]=0;
                    min = tab[c];
                }
                if(min > tab[c+tailley-1])
                {
                    res[c]=7;
                    min = tab[c+tailley-1];
                }
                if(min > tab[c+tailley])
                {
                    res[c]=6;
                    min = tab[c+tailley];
                }
            }
            else
            {
                if(min > tab[c-tailley-1])
                {
                    res[c]=1;
                    min = tab[c-tailley-1];
                }
                if(min > tab[c-tailley])
                {
                    res[c]=2;
                    min = tab[c-tailley];
                }
                if(min > tab[c-1])
                {
                    res[c]=8;
                    min = tab[c-1];
                }
                if(min > tab[c-tailley+1])
                {
                    res[c]=3;
                    min = tab[c-tailley+1];
                }
                if(min > tab[c])
                {
                    res[c]=0;
                    min = tab[c];
                }
                if(min > tab[c+tailley-1])
                {
                    res[c]=7;
                    min = tab[c+tailley-1];
                }
                if(min > tab[c+1])
                {
                    res[c]=4;
                    min = tab[c+1];
                }
                if(min > tab[c+tailley])
                {
                    res[c]=6;
                    min = tab[c+tailley];
                }
                if(min > tab[c+tailley+1])
                {
                    res[c]=5;
                    min = tab[c+tailley+1];
                }
            }
        }
    }
    return res;
}

float * etape3(float * tab,int taillex,int tailley)
{
    float * res = new float[taillex*tailley];
    int pid,nprocs,mini,max;
    MPI_Comm_rank(MPI_COMM_WORLD,&pid);
    MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
    int taillewbord;
    if(pid == 0)
    {
        taillewbord = taillex+1;
        max = taillewbord-1;
        mini = 0;
    }
    else if(pid == nprocs)
    {
        taillewbord = taillex+1;
        max = taillewbord;
        mini = 1;
    }
    else
    {
        taillewbord = taillex+2;
        mini = 1;
        max =taillewbord-1;
    }
    const static int voisin[] = {-tailley-1,-tailley,-tailley+1,-1,1,tailley-1,tailley,tailley+1};
    const static float connection[] = {5,6,7,4,8,3,2,1};
    bool condition = true;
    int k = 0,test;
    res = new float[taillewbord*tailley];
    for(int i = 0; i<taillewbord; i++)
        for(int j =0;j<tailley; j++)
            res[i*tailley+j]=-1;
    while(condition)
    {
        echangerbords(tab,taillex,tailley);
        condition = false;
        test = 0;
        for(int i = mini; i<max; i++)
        {
            for(int j =0;j<tailley; j++)
            {
                int c = i*tailley+j;
                int som = 1;
                if( i == 0 && j == 0)
                {
                    k = 4;
                    while(k<8)
                    {
                        if(tab[c+voisin[k]] == connection[k])
                        {
                            if(res[c+voisin[k]] == -1)
                            {
                                som = -1;
                                break;
                            }
                            else
                                som+=res[c+voisin[k]];
                        }
                        k++;
                        if(k==5)
                            k++;
                    }
                }
                else if( i == 0 && j == nprocs -1)
                {
                    k=3;
                    while(k<7)
                    {
                        if(tab[c+voisin[k]] == connection[k])
                        {
                            if(res[c+voisin[k]] == -1)
                            {
                                som = -1;
                                break;
                            }
                            else
                                som+=res[c+voisin[k]];
                        }
                        k++;
                        if(k==4)
                            k++;
                    }
                }
                else if(i == 0)
                {
                    k = 3;
                    while(k<8)
                    {
                        if(tab[c+voisin[k]] == connection[k])
                        {
                            if(res[c+voisin[k]] == -1)
                            {
                                som = -1;
                                break;
                            }
                            else
                                som+=res[c+voisin[k]];
                        }
                        k++;
                    }
                }
                else if(i == nprocs-1 && j == 0)
                {
                    k = 1;
                    while(k<5)
                    {
                        if(tab[c+voisin[k]] == connection[k])
                        {
                            if(res[c+voisin[k]] == -1)
                            {
                                som = -1;
                                break;
                            }
                            else
                                som+=res[c+voisin[k]];
                        }
                        k++;
                        if(k == 3)
                            k++;
                    }
                }
                else if(i == nprocs -1 && j == nprocs-1)
                {
                    k = 0;
                    while(k<4)
                    {
                        if(tab[c+voisin[k]] == connection[k])
                        {
                            if(res[c+voisin[k]] == -1)
                            {
                                som = -1;
                                break;
                            }
                            else
                                som+=res[c+voisin[k]];
                        }
                    }
                    k++;
                    if(k == 2)
                        k++;
                }
                else if( i == nprocs -1)
                {
                    k=0;
                    while(k<5)
                    {
                        if(tab[c+voisin[k]] == connection[k])
                        {
                            if(res[c+voisin[k]] == -1)
                            {
                                som = -1;
                                break;
                            }
                            else
                                som+=res[c+voisin[k]];
                        }
                        k++;
                    }
                }
                else if(j == 0)
                {
                    k = 1;
                    while(k<8)
                    {
                        if(tab[c+voisin[k]] == connection[k])
                        {
                            if(res[c+voisin[k]] == -1)
                            {
                                som = -1;
                                break;
                            }
                            else
                                som+=res[c+voisin[k]];
                        }
                        k++;
                        if(k==3 || k==5)
                            k++;
                    }
                }
                else if(j == tailley-1)
                {
                    k=0;
                    while(k<7)
                    {
                        if(tab[c+voisin[k]] == connection[k])
                        {
                            if(res[c+voisin[k]] == -1)
                            {
                                som = -1;
                                break;
                            }
                            else
                                som+=res[c+voisin[k]];
                        }
                        k++;
                        if(k==2 || k==4)
                            k++;
                    }
                }
                else
                {
                    for(k=0;k<8;k++)
                    {
                        if(tab[c+voisin[k]] == connection[k])
                        {
                            if(res[c+voisin[k]] == -1)
                            {
                                som = -1;
                                break;
                            }
                            else
                                som+=res[c+voisin[k]];
                        }
                        k++;
                        if(k==2 || k==4)
                            k++;
                    }
                }
                if(som == -1)
                    test = 1;
                res[c]=som;
            }
        }
        MPI_Allreduce(&test,&test,1,MPI_INT,MPI_MAX,MPI_COMM_WORLD);
        if(test != 0)
            condition = true;
    }
    return res;
}

int main(int argc,char ** argv)
{
    int pid,nprocs,taillex,tailley,taillebande,reste;
    float * mat, * tab, max;
    Matrice * m;
    MPI_Init(&argc,&argv);

    MPI_Comm_rank(MPI_COMM_WORLD,&pid);
    MPI_Comm_size(MPI_COMM_WORLD,&nprocs);

    if(pid == 0)
    {
        m = new Matrice("ex.txt");
        taillex = m->Taillex();
        tailley = m->Tailley();
        max = m->Max();
        mat = m->getMat();
    }
    MPI_Bcast(&taillex,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(&tailley,1,MPI_INT,0,MPI_COMM_WORLD);
    MPI_Bcast(&max,1,MPI_FLOAT,0,MPI_COMM_WORLD);

    taillebande = taillex/nprocs;
    reste = taillex%nprocs;

    if(pid == nprocs-1)
        tab = new float[(taillebande+reste+1)*tailley];
    else if(pid == 0)
        tab = new float[(taillebande+1)*tailley];
    else
        tab = new float[(taillebande+2)*tailley];

    int k = distribue(mat,tab,taillex,tailley);
    if(pid == 0)
    {
        cout<<"matrice partagée"<<endl;
    }
    float * res = etape1(tab,k,tailley,max);
    rassemble(res,mat,taillex,tailley);
    if(pid == 0)
    {
        cout<<"etape 1 terminé"<<endl;
        m->writeetape1(mat);
    }
    res = etape2(res,k,tailley,max);
    rassemble(res,mat,taillex,tailley);
    if(pid == 0)
    {
        cout<<"etape 2 terminé"<<endl;
        m->writeetape2(mat);
    }
    res = etape3(res,k,tailley);
    rassemble(res,mat,taillex,tailley);
    if(pid == 0)
    {
        cout<<"etape 3 terminé"<<endl;
        m->writeetape3(mat);
    }
    MPI_Finalize();
}
