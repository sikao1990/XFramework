#ifndef __Singleton_H__
#define __Singleton_H__

template < typename T >
class Singleton
{
	Singleton(const Singleton&){}
	Singleton& operator=(const Singleton&){ return *this;}
protected:
	Singleton(){}
public:
	static T& Instance(){
		static T obj;
		return obj;
	}
};


#endif	// Singleton.h
