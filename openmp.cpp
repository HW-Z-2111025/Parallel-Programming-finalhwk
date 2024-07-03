#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include <stdio.h>
#include <bits/stdc++.h>
#include <vector>
#include <sys/time.h>
#include <omp.h>
#define N 43577
#define ELI 54274

using namespace std;
const int thread_num = 4;
const int chunksize = 1;
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
// 被消元行eli1
line eli[ELI];

void cout1()
{
    cout << "*****消元子******" << N << endl;
    for (int i = 0; i < N; i++)
        cout << eliminer[i] << endl;

    cout << "*****被消元行****" << ELI << endl;
    for (int i = 0; i < ELI; i++)
        cout << eli[i].b << "  " << eli[i].bit << endl;
}

// 判断是否全部消去完成
bool check_null()
{
    for (int i = 0; i < ELI; i++)
    {
        if (eli[i].b == false)
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
    file.open("./data/10/消元子.txt", ios_base::in);


    if (!file.is_open())
    {
        cout << "打开失败" << endl;
    }
    string s;

    while (getline(file, s))
    {
        bool b = true;
        int x;
        // cout<<s<<endl;
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

    file.open("./data/10/被消元行.txt", ios_base::in);

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
        eli[x].b = false;

        while (st >> temp)
        {
            if (bo)
            {
                bo = false;
                eli[x].num = temp;
            }
            eli[x].bit.set(temp);
        }
        x++;
    }
    file.close();
    int rt = 0;
    int p = 100;

    cout << "start" << endl;
    struct timeval begin1, end1;
    gettimeofday(&begin1, NULL);

    int i; // first cycle
    int j; // secend cycle
    int k; // third cycle
    int l;
#pragma omp parallel num_threads(thread_num), private(i, j, k, l, rt)
    for (i = N - 1; i >= 0; i -= p)
    {
        if (i - p < 0)
        {
            cout << p << endl;
            p = i + 1;
        }

#pragma omp for schedule(static, chunksize)
        for (j = 0; j < ELI; j++)
        {
            if (eli[j].b == 0)
            {
                for (k = 0; k < p; k++)
                {
                    if (eli[j].bit[i - k] == 1)
                    {// 消去过程
                        eli[j].bit ^= eliminer[i - k];
                    }
                    if (eli[j].bit.none())
                    {
                        eli[j].b = true;
                        break;
                    }
                    else
                    {
                        for (l = eli[j].num; l >= 0; l--)
                        {
                            if (eli[j].bit[l] == 1)
                            {
                                eli[j].num = l;
                                break;
                            }
                        }
                        if (eliminer[eli[j].num].none())
                        {
                            eli[j].b = true;
                            break;
                        }
                    }
                }
            }
        }

#pragma omp single
        for (j = 0; j < ELI; j++)
        {
            if (eli[j].b == 1)
            {
                if (eliminer[eli[j].num].none())
                    eliminer[eli[j].num] ^= eli[j].bit;
                else
                {
                    eli[j].b = 0;
                    for (k = 0; k < p; k++)
                    {
                        if (eli[j].bit[i - k] == 1)
                        { // 消去过程
                            eli[j].bit ^= eliminer[i - k];
                        }
                        if (eli[j].bit.none())
                        {
                            eli[j].b = true;
                            cout << "null" << rt++ << endl;
                            break;
                        }
                        else
                        {
                            for (l = eli[j].num; l >= 0; l--)
                            {
                                if (eli[j].bit[l] == 1)
                                {
                                    eli[j].num = l;
                                    break;
                                }
                            }
                            if (eliminer[eli[j].num].none())
                            {
                                eliminer[eli[j].num] ^= eli[j].bit;
                                eli[j].b = true;
                                break;
                            }
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
    gettimeofday(&end1, NULL);
    long long time_use = ((end1.tv_sec - begin1.tv_sec) * 1000000 + end1.tv_usec - begin1.tv_usec);
    res();
    cout << "时间：" << time_use << endl;

    return 0;
}
