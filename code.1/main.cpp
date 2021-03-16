/*
    Created BY 谢子飏 19307130037
*/
#include <queue>
#include <string>
#include <random>
#include <string>
#include <ctime>
#include "LinearTable.h"
#define TimerFunc() Timer __timer__;


class Timer
{
public:
    Timer():s_time(clock())
    {
        printf("--------------- Start ----------------\n");
    };
    ~Timer()
    {
        double seg = clock() - s_time;
        printf("Used Time: %f\n", 1000*(seg) / (double)CLOCKS_PER_SEC);
        printf("--------------- End -----------------\n");
    };
private:
    clock_t s_time;
};

template<typename T>
void ra_speed_test(T lt, std::string __name__)
{
    std::default_random_engine e;
    std::uniform_int_distribution<> u(-1e5 + 1,1e5);
    const int N = 1e7;
    printf("Test: %s - %d\n", __name__.c_str(), N);
    
    for(int i = 1; i <= N; ++i)
    {
        int r = u(e);
        if(u(e) > 0)
            lt.push_back(r);
        else
            lt.push_front(r);
    }
    TimerFunc();
    for(int i = 0; i < lt.size(); ++i)
        lt[i];
}

template<typename T>
void push_speed_test(T lt, std::string __name__)
{
    std::default_random_engine e;
    std::uniform_int_distribution<> u(-1e5 + 1,1e5);
    const int N = 1e7;
    printf("Test: %s - %d\n", __name__.c_str(), N);
    
    TimerFunc();
    for(int i = 1; i <= N; ++i)
    {
        int r = u(e);
        if(u(e) > 0)
            lt.push_back(r);
        else
            lt.push_front(r);
    }
}

template<typename T>
void pop_speed_test(T lt, std::string __name__)
{
    std::default_random_engine e;
    std::uniform_int_distribution<> u(-1e5 + 1,1e5);
    const int N = 1e7;
    printf("Test: %s - %d\n", __name__.c_str(), N);
    
    for(int i = 1; i <= N; ++i)
    {
        int r = u(e);
        if(u(e) > 0)
            lt.push_back(r);
        else
            lt.push_front(r);
    }
    TimerFunc();
    for(int i = 1; i < N; ++i)
    {
        if(u(e) > 0)
            lt.pop_back();
        else
            lt.pop_front();
    }
}

template<typename T>
void insert_speed_test(T lt, std::string __name__)
{
    std::default_random_engine e;
    std::uniform_int_distribution<> u(-1e5 + 1,1e5);
    const int N = 1e7, M = 1e4;
    printf("Test: %s - Base: %d Insert: %d\n", __name__.c_str(), N, M);
    
    for(int i = 1; i <= N; ++i)
    {
        int r = u(e);
        if(u(e) > 0)
            lt.push_back(r);
        else
            lt.push_front(r);
    }
    
    std::uniform_int_distribution<> u2(0, 1e7 - 1);
    
    TimerFunc();
    for(int i = 1; i <= M; ++i)
        lt.insert(lt.begin() + u2(e), i);
}

template<typename T>
void batch_insert_speed_test(T lt, std::string __name__)
{
    std::default_random_engine e;
    std::uniform_int_distribution<> u(-1e5 + 1,1e5);
    const int N = 1e7, M = 1e5;
    printf("Test: %s - Base: %d Insert: %d\n", __name__.c_str(), N, M);
    
    for(int i = 1; i <= N; ++i)
    {
        int r = u(e);
        if(u(e) > 0)
            lt.push_back(r);
        else
            lt.push_front(r);
    }
    
    std::uniform_int_distribution<> u2(0, 1e7 - 1);
    auto it = lt.begin() + u2(e);
    TimerFunc();
    lt.insert(it, M, 100);
}

int main()
{
    std::deque<int> realDeque;
    DS::LinearTable myDeque;
    // TODO add tester
}
