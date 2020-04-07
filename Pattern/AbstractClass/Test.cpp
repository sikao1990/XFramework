#include <iostream>
#include <stdio.h>
#include "TComData.h"
#include "TMethodClass.h"

class Player : public ClassProxy<TComData>
{
public:
	bool Init(){
		AbstractClass* pNewMethod = new TMethodClass(m_comData);
		pNewMethod->Init();
		m_methods.push_back(pNewMethod);
		return true;
	}
};

int main()
{
	Player	player;
	player.Init();
	player.InterfaceMethod(2);
	printf("m_a:%d,m_b:%d:m_c:%s\n",player.m_comData.m_a,player.m_comData.m_b,player.m_comData.m_c.c_str());
	return 0;
}
