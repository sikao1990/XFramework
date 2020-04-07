#ifndef __ForceConvert_H__
#define __ForceConvert_H__

template <typename ToType,typename FromType>
void ForceConvert(ToType& addr,FromType f)//强制类型转换
{
    union 
    {
		FromType _f;
		ToType   _t;
    }ut;

    ut._f 	= f;
    addr 	= ut._t;
}

#endif	//ForceConvert.h
