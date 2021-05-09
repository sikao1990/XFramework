#include <stdio.h>
#include "../../XPublic.h"
#include <list>
#include <unistd.h>

using namespace std;

//Production and consumption
class ProductConsum
{
public:
    ProductConsum():m_tmp(0){
        th[0] = new ThreadImpl<ProductConsum>(this,&ProductConsum::Product);
        th[1] = new ThreadImpl<ProductConsum>(this,&ProductConsum::Consumor);
    }
    ~ProductConsum(){
        delete th[0];
        delete th[1];
    }
    void Product(){
        while(1){
            m_ctrl.Lock();
            while(m_lst.size()>10)m_ctrl.WaitProduce();
            m_lst.push_back(++m_tmp);
            printf("Product:%d\n",m_tmp);
            m_ctrl.WakeConsume(false);     
            sleep(1);
            m_ctrl.Unlock();
            
        }
    }
    void Consumor(){
        while(1){
            m_ctrl.Lock();
            while(m_lst.empty())m_ctrl.WaitConsume();
            int val = m_lst.front();
            m_lst.pop_front();     
            printf("Consumor %d\n",val);            
            m_ctrl.WakeProduce(false);
            sleep(1);
            m_ctrl.Unlock();
        }
    }
    void startWork(){
        th[0]->start();
        th[1]->start();
    }
private:
    ThreadCtrlEx m_ctrl;
    list<int> m_lst;
    ThreadPri* th[2];
    int m_tmp;
};
int main()
{
    ProductConsum logic;
    logic.startWork();
    while(1){
        sleep(1);
    }
    return 0;
}