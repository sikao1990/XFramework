#ifndef __XAllocator_H__
#define __XAllocator_H__
#include <new>        // for placement new
#include <cstddef>    // for ptrdiff_t, size_t
#include <cstdlib>    // for exit()
#include <climits>    // for UINT_MAX
#include <iostream>    // for cerr
#include "MemMgr.h"

template <class T>
class XAllocator {
public:
	typedef T			value_type;
	typedef T*          pointer;
	typedef const T*    const_pointer;
	typedef T&          reference;
	typedef const T&    const_reference;
	typedef size_t      size_type;
	typedef ptrdiff_t   difference_type;

	template <class U>
	struct rebind {
		typedef XAllocator<U> other;
	};

	XAllocator()
	{
		mgr.Init(sizeof(T));
	}
	XAllocator(const XAllocator&)
	{
		mgr.Init(sizeof(T));
	}
	template <class U> XAllocator(const XAllocator<U>&)
	{
		mgr.Init(sizeof(T));
	}
	~XAllocator()
	{
	}
	
	pointer address(reference x) {return (pointer)&x;}
	const_pointer address(const_reference x) const {
		return (const_pointer)&x;
	}

	pointer allocate(size_type n, const void *hint=0) {
		return (pointer)mgr.Alloc(n);
	}

	void deallocate(pointer p, size_type n) {
		mgr.Free(p,n);
	}

	size_type max_size() const {return size_type(UINT_MAX / sizeof(T));}

	void construct(pointer p, const T& x) {
		_construct(p, x);
	}

	void destroy(pointer p) {
		_destroy(p);
	}
private:	
	template <class T1, class T2>
	inline void _construct(T1* p, const T2& value) {
		new(p) T1(value);
	}

	template <class U>
	inline void _destroy(U* p) {
		p->~U();
	}
private:
	MemMgr mgr;
};

#endif