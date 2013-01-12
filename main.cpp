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

    if(pid == 0)
    {
        MPI_Sendrecv(send+(taillex-2),tailley,MPI_FLOAT,1,
                     123,send+taillex-1,tailley,MPI_FLOAT,1,123,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
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
        MPI_Sendrecv(send+taillex-2,tailley,MPI_FLOAT,pid+1,123,
                     send+taillex-1,tailley,MPI_FLOAT,pid+1,123,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    }
    MPI_Barrier(MPI_COMM_WORLD);
}

//distribution du tableau initiale et renvoie la taille bande distribué + le reste pour le dernier processeur
int distribue(float * send,float * recv, int taillex, int tailley)
{
    int pid,nprocs;
    MPI_Comm_rank(MPI_COMM_WORLD,&pid);
    MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
    int taillebande = taillex/nprocs, reste = taillex%nprocs;

    /*    if(pid == 0)
    {
        MPI_Sendrecv(send,taillebande*tailley,MPI_FLOAT,0,123,
                     recv,taillebande*tailley,MPI_FLOAT,0,123,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        for(int i = 1;i<nprocs;i++)
        {
            if(i == nprocs-1)
                MPI_Send(send+((i*taillebande)*tailley),(taillebande+reste)*tailley,MPI_FLOAT,i,123,MPI_COMM_WORLD);
            else
                MPI_Send(send+((i*taillebande)*tailley),taillebande*tailley,MPI_FLOAT,i,123,MPI_COMM_WORLD);
        }
    }
    else
    {
        if(pid == nprocs-1)
            MPI_Recv(recv+tailley,(taillebande+reste)*tailley,MPI_FLOAT,0,123,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        else
            MPI_Recv(recv+tailley,taillebande*tailley,MPI_FLOAT,0,123,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    }*/
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

float * etape1(float * tab,int taillex,int tailley,float max)
{
    float * w = new float[taillex*tailley];
    //ajouter les communications pour les bords
    const static int voisin[] = {-tailley-1,-tailley,-tailley+1,-1,1,tailley-1,tailley,tailley+1};
    for(int i=0;i<taillex;i++)
    {
        for(int j=0;j<tailley;j++)
        {
            if(i==0 || i==taillex-1)
                w[i*tailley+j]= tab[i*tailley+j];
            else
            {
                if(j==0 || j==tailley-1)
                    w[i*tailley+j]= tab[i*tailley+j];
                else
                    w[i*tailley+j]=max;
            }
        }
    }
    bool condition=true;
    while(condition)
    {
        condition = false;
        for(int i=1;i<taillex-1;i++)
        {
            for(int j=1;j<tailley-1;j++)
            {
                int c = i*tailley+j;
                if(w[c] > tab[c])
                {
                    //cout<<w[c]<<" "<<tab[c]<<endl;
                    for(int k = 0;k<8;k++)
                    {
                        if(tab[c] >= w[c+voisin[k]]+eps)
                        {
                            w[c] = tab[c];
                            condition=true;
                        }
                        else if(w[c] >  w[c+voisin[k]]+eps)
                        {
                            w[c] = w[c+voisin[k]]+eps;
                            condition=true;
                        }
                    }
                    cout<<"fin boucle voisin"<<endl;
                }
            }
        }
        //ajouter communication des bords
        //cout<<"infinite loop"<<endl;
    }
    return w;
}

float * etape2(float * tab,int taillex,int tailley,float max)
{
    //echange des bords
    float * res = new float[taillex*tailley];
    //const static int voisin[] = {-tailley-1,-tailley,-tailley+1,-1,1,tailley-1,tailley,tailley+1};
    for(int i =0; i<taillex;i++)
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
    const static int voisin[] = {-tailley-1,-tailley,-tailley+1,-1,1,tailley-1,tailley,tailley+1};
    const static float connection[] = {5,6,7,4,8,3,2,1};
    bool condition = true;
    float * res = new float[taillex*tailley];
    for(int i = 0; i<taillex; i++)
        for(int j =0;j<tailley; j++)
            res[i*tailley+j]=-1;
    while(condition)
    {
        condition = false;
        for(int i = 0; i<taillex; i++)
        {
            for(int j =0;j<tailley; j++)
            {
                int c = i*tailley+j;
                int som = 1,k;
                if(j == 0)
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
                    condition = true;
                res[c]=som;
            }
        }
    }
    return res;
}

int main(int argc,char ** argv)
{
    /*    Matrice m("ex.txt");
    float * res = etape1(m.getMat(),m.Taillex(),m.Tailley(),m.Max());
    cout<<"Après étape 1"<<endl;
    for(int i=0;i<m.Taillex();i++)
    {
        for(int j=0;j<m.Tailley();j++)
        {
            cout<<res[i*m.Tailley()+j]<<" ";
        }
        cout<<endl;
    }
    float * res1 = etape2(res,m.Taillex(),m.Tailley(),m.Max());
    cout<<"après étape 2:"<<endl;
    for(int i=0;i<m.Taillex();i++)
    {
        for(int j=0;j<m.Tailley();j++)
        {
            cout<<res1[i*m.Tailley()+j]<<" ";
        }
        cout<<endl;
    }
    float * res2 = etape3(res1,m.Taillex(),m.Tailley());
    cout<<"apres étape 2"<<endl;
    for(int i=0;i<m.Taillex();i++)
    {
        for(int j=0;j<m.Tailley();j++)
        {
            cout<<res2[i*m.Tailley()+j]<<" ";
        }
        cout<<endl;
    }*/
    int pid,nprocs,taillex,tailley,taillebande,reste;
    float * mat, * tab, max;
    MPI_Init(&argc,&argv);

    MPI_Comm_rank(MPI_COMM_WORLD,&pid);
    MPI_Comm_size(MPI_COMM_WORLD,&nprocs);

    if(pid == 0)
    {
        Matrice m("ex.txt");
        taillex = m.Taillex();
        tailley = m.Tailley();
        max = m.Max();
        mat = m.getMat();
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
    echangerbords(tab,k,tailley);
    if(pid == 1)
    {
        for(int i = 0;i<k+2;i++)
        {
            for(int j = 0;j<tailley;j++)
            {
                cout<<tab[i*tailley+j]<<" ";
            }
            cout<<endl;
        }
    }
    cout<<endl;
    MPI_Finalize();
}
