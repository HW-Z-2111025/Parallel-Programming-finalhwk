#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <stdio.h>
#include <bits/stdc++.h>
#include <vector>
#include <sys/time.h>
#include <mpi.h>
#define N 8399
#define ELI 4535

using namespace std;

struct line
{
    // 是否升级为消元子
    bool b;
    // 首个1的位置
    int num;
    bitset<N> bit;
};

// 消元子eliminer1
bitset<N> eliminer[N];
// 被消元行eline1
line eline[ELI];

void cout1()
{
    cout << "*****消元子******" << N << endl;
    for (int i = 0; i < N; i++)
        cout << eliminer[i] << endl;

    cout << "*****被消元行****" << ELI << endl;
    for (int i = 0; i < ELI; i++)
        cout << eline[i].b << "  " << eline[i].bit << endl;
}

// 判断是否全部消去完成
bool check_null()
{
    for (int i = 0; i < ELI; i++)
    {
        if (eline[i].b == false)
            return false;
    }
    return true;
}

void res()
{
    int rt = 0;
    for (int i = 0; i < N; i++)
        if (eliminer[i].any())
            rt++;
    cout << "行数：" << rt;
}

int main()
{

    ifstream file;
    file.open("./data/7/消元子.txt", ios_base::in);

    if (!file.is_open())
    {
        cout << "打开失败" << endl;
    }
    string s;

    while (getline(file, s))
    {
        bool b = true;
        int x;
        stringstream st;
        int temp;
        st << s;

        while (st >> temp)
        {
            if (b)
            {
                x = temp;
                b = false;
            }

            eliminer[x].set(temp);
        }
    }

    file.close();
    file.clear(ios::goodbit);

    file.open("./data/7/被消元行.txt", ios_base::in);

    if (!file.is_open())
    {
        cout << "打开失败" << endl;
    }
    int x = 0;
    while (getline(file, s))
    {
        bool bo = true;
        stringstream st;
        int temp;
        st << s;
        eline[x].b = false;

        while (st >> temp)
        {
            if (bo)
            {
                bo = false;
                eline[x].num = temp;
            }
            eline[x].bit.set(temp);
        }
        x++;
    }
    file.close();
    int rt = 0;
    int p = 25;

    int comm_sz;
    int my_rank;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    int range = ELI / comm_sz;
    int r1, r2;
    r1 = my_rank * range;
    if (my_rank != comm_sz - 1)
        r2 = r1 + range - 1;
    else
        r2 = ELI - 1;

    cout << "start" << endl;
    struct timeval begin1, end1;
    gettimeofday(&begin1, NULL);
    for (int i = N - 1; i >= 0; i -= p)
    {
        if (i - p < 0)
        {
            cout << "p=i+1" << endl;
            p = i + 1;
        }

        for (int j = r1; j <= r2; j++)
        {
            if (eline[j].b == 0)
            {
                for (int k = 0; k < p; k++)
                {
                    if (eline[j].bit[i - k] == 1)
                    {
                        // 消去过程
                        eline[j].bit ^= eliminer[i - k];
                    }
                    if (eline[j].bit.none())
                    {
                        eline[j].b = true;
                        cout << "null" << rt++ << endl;
                        break;
                    }
                    else
                    {
                        for (int l = eline[j].num; l >= 0; l--)
                        {
                            if (eline[j].bit[l] == 1)
                            {
                                eline[j].num = l;
                                break;
                            }
                        }
                        if (eliminer[eline[j].num].none())
                        {
                            eline[j].b = true;
                            cout << rt++ << endl;
                            break;
                        }
                    }
                }
            }
        }

        if (check_null())
        {
            cout << "null" << endl;
            break;
        }
    }
    if (my_rank != 0)
        MPI_Send(&eline[r1], (r2 - r1 + 1) * sizeof(line), MPI_BYTE, 0, 1, MPI_COMM_WORLD);
    else
        for (int q = 1; q < comm_sz; q++)
        {
            if (q != comm_sz - 1)
                MPI_Recv(&eline[q * range], range * sizeof(line), MPI_BYTE, q, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            else
                MPI_Recv(&eline[q * range], sizeof(line) * (range + ELI % comm_sz), MPI_BYTE, q, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

    if (my_rank == 0)
    {
        for (int q = 1; q < comm_sz; q++)
        {
            if (q != comm_sz - 1)
                MPI_Send(&eline[q * range], range * sizeof(line), MPI_BYTE, q, 2, MPI_COMM_WORLD);
            else
                MPI_Send(&eline[q * range], sizeof(line) * (range + ELI % comm_sz), MPI_BYTE, q, 2, MPI_COMM_WORLD);
        }
    }

    if (my_rank != 0)
    {
        MPI_Recv(&eline[r1], (r2 - r1 + 1) * sizeof(line), MPI_BYTE, 0, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    gettimeofday(&end1, NULL);
    long long time_use = ((end1.tv_sec - begin1.tv_sec) * 1000000 + end1.tv_usec - begin1.tv_usec);
    res();
    cout << "时间：" << time_use << endl;
    MPI_Finalize();
    return 0;
}
