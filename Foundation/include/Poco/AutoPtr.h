//
// AutoPtr.h
//
// Library: Foundation
// Package: Core
// Module:  AutoPtr
//
// Definition of the AutoPtr template class.
//
// Copyright (c) 2004-2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// SPDX-License-Identifier:	BSL-1.0
//


#ifndef Foundation_AutoPtr_INCLUDED
#define Foundation_AutoPtr_INCLUDED


#include "Poco/RefPtr.h"


#ifndef POCO_PTR_CHECKING
	#ifdef _DEBUG
		#define POCO_PTR_CHECKING 0
	#else
		#define POCO_PTR_CHECKING 1
	#endif
#endif

namespace Poco {


template <class C>
class LegacyAutoPtr
	/// LegacyAutoPtr is a "smart" pointer for classes implementing
	/// reference counting based garbage collection.
	/// To be usable with the LegacyAutoPtr template, a class must
	/// implement the following behaviour:
	/// A class must maintain a reference count.
	/// The constructors of the object initialize the reference
	/// count to one.
	/// The class must implement a public duplicate() method:
	///     void duplicate();
	/// that increments the reference count by one.
	/// The class must implement a public release() method:
	///     void release()
	/// that decrements the reference count by one, and,
	/// if the reference count reaches zero, deletes the
	/// object.
	///
	/// LegacyAutoPtr works in the following way:
	/// If an LegacyAutoPtr is assigned an ordinary pointer to
	/// an object (via the constructor or the assignment operator),
	/// it takes ownership of the object and the object's reference
	/// count remains unchanged.
	/// If the LegacyAutoPtr is assigned another LegacyAutoPtr, the
	/// object's reference count is incremented by one by
	/// calling duplicate() on its object.
	/// The destructor of LegacyAutoPtr calls release() on its
	/// object.
	/// LegacyAutoPtr supports dereferencing with both the ->
	/// and the * operator. An attempt to dereference a null
	/// LegacyAutoPtr results in a NullPointerException being thrown.
	/// LegacyAutoPtr also implements all relational operators.
	/// Note that LegacyAutoPtr allows casting of its encapsulated data types.
{
public:
	LegacyAutoPtr(): _ptr(0)
	{
	}

	/*explicit*/ LegacyAutoPtr(C* ptr): _ptr(ptr)
	{
	}

	LegacyAutoPtr(C* ptr, bool shared): _ptr(ptr)
	{
		if (shared && _ptr) _ptr->duplicate();
	}

	LegacyAutoPtr(const LegacyAutoPtr& ptr): _ptr(ptr._ptr)
	{
		if (_ptr) _ptr->duplicate();
	}

	LegacyAutoPtr(LegacyAutoPtr&& ptr) : _ptr(std::move(ptr._ptr))
	{
		ptr._ptr = nullptr;
	}

	template <class Other>
	LegacyAutoPtr(const LegacyAutoPtr<Other>& ptr): _ptr(const_cast<Other*>(ptr.get()))
	{
		if (_ptr) _ptr->duplicate();
	}

	~LegacyAutoPtr()
	{
		if (_ptr) _ptr->release();
	}

	LegacyAutoPtr& assign(C* ptr)
	{
		if (_ptr != ptr)
		{
			if (_ptr) _ptr->release();
			_ptr = ptr;
		}
		return *this;
	}

	LegacyAutoPtr& assign(C* ptr, bool shared)
	{
		if (_ptr != ptr)
		{
			if (_ptr) _ptr->release();
			_ptr = ptr;
			if (shared && _ptr) _ptr->duplicate();
		}
		return *this;
	}

	LegacyAutoPtr& assign(const LegacyAutoPtr& ptr)
	{
		if (&ptr != this)
		{
			if (_ptr) _ptr->release();
			_ptr = ptr._ptr;
			if (_ptr) _ptr->duplicate();
		}
		return *this;
	}

	template <class Other>
	LegacyAutoPtr& assign(const LegacyAutoPtr<Other>& ptr)
	{
		if (ptr.get() != _ptr)
		{
			if (_ptr) _ptr->release();
			_ptr = const_cast<Other*>(ptr.get());
			if (_ptr) _ptr->duplicate();
		}
		return *this;
	}

	void reset()
	{
		if (_ptr)
		{
			_ptr->release();
			_ptr = 0;
		}
	}

	void reset(C* ptr)
	{
		assign(ptr);
	}

	void reset(C* ptr, bool shared)
	{
		assign(ptr, shared);
	}

	void reset(const LegacyAutoPtr& ptr)
	{
		assign(ptr);
	}

	template <class Other>
	void reset(const LegacyAutoPtr<Other>& ptr)
	{
		assign<Other>(ptr);
	}

	LegacyAutoPtr& operator = (C* ptr)
	{
		return assign(ptr);
	}

	LegacyAutoPtr& operator = (const LegacyAutoPtr& ptr)
	{
		return assign(ptr);
	}

	template <class Other>
	LegacyAutoPtr& operator = (const LegacyAutoPtr<Other>& ptr)
	{
		return assign<Other>(ptr);
	}

	LegacyAutoPtr& operator = (LegacyAutoPtr&& ptr)
	{
		if (&ptr == this) return *this;
		if (_ptr) _ptr->release();
		_ptr = ptr._ptr;
		ptr._ptr = nullptr;
		return *this;
	}

	void swap(LegacyAutoPtr& ptr)
	{
		std::swap(_ptr, ptr._ptr);
	}

	template <class Other>
	LegacyAutoPtr<Other> cast() const
		/// Casts the LegacyAutoPtr via a dynamic cast to the given type.
		/// Returns an LegacyAutoPtr containing NULL if the cast fails.
		/// Example: (assume class Sub: public Super)
		///    LegacyAutoPtr<Super> super(new Sub());
		///    LegacyAutoPtr<Sub> sub = super.cast<Sub>();
		///    poco_assert (sub.get());
	{
		Other* pOther = dynamic_cast<Other*>(_ptr);
		return LegacyAutoPtr<Other>(pOther, true);
	}

	template <class Other>
	LegacyAutoPtr<Other> unsafeCast() const
		/// Casts the LegacyAutoPtr via a static cast to the given type.
		/// Example: (assume class Sub: public Super)
		///    LegacyAutoPtr<Super> super(new Sub());
		///    LegacyAutoPtr<Sub> sub = super.unsafeCast<Sub>();
		///    poco_assert (sub.get());
	{
		Other* pOther = static_cast<Other*>(_ptr);
		return LegacyAutoPtr<Other>(pOther, true);
	}

	inline C* operator -> ()
	{
#if POCO_PTR_CHECKING
		if (_ptr)
			return _ptr;
		else
			throw NullPointerException();
#else
		return _ptr;
#endif
	}

	inline const C* operator -> () const
	{
#if POCO_PTR_CHECKING
		if (_ptr)
			return _ptr;
		else
			throw NullPointerException();
#else
		return _ptr;
#endif
	}

	inline C& operator * ()
	{
#if POCO_PTR_CHECKING
		if (_ptr)
			return *_ptr;
		else
			throw NullPointerException();
#else
		return *_ptr;
#endif
	}

	inline const C& operator * () const
	{
#if POCO_PTR_CHECKING
		if (_ptr)
			return *_ptr;
		else
			throw NullPointerException();
#else
		return *_ptr;
#endif
	}

	inline C* get()
	{
		return _ptr;
	}

	inline const C* get() const
	{
		return _ptr;
	}

	inline operator C* ()
	{
		return _ptr;
	}
	
	inline operator const C* () const
	{
		return _ptr;
	}
	
	inline bool operator ! () const
	{
		return _ptr == 0;
	}

	inline bool isNull() const
	{
		return _ptr == 0;
	}
	
	inline C* duplicate()
	{
		if (_ptr) _ptr->duplicate();
		return _ptr;
	}

	inline bool operator == (const LegacyAutoPtr& ptr) const
	{
		return _ptr == ptr._ptr;
	}

	inline bool operator == (const C* ptr) const
	{
		return _ptr == ptr;
	}

	inline bool operator == (C* ptr) const
	{
		return _ptr == ptr;
	}

	inline bool operator != (const LegacyAutoPtr& ptr) const
	{
		return _ptr != ptr._ptr;
	}

	inline bool operator != (const C* ptr) const
	{
		return _ptr != ptr;
	}

	inline bool operator != (C* ptr) const
	{
		return _ptr != ptr;
	}

	inline bool operator < (const LegacyAutoPtr& ptr) const
	{
		return _ptr < ptr._ptr;
	}

	inline bool operator < (const C* ptr) const
	{
		return _ptr < ptr;
	}

	inline bool operator < (C* ptr) const
	{
		return _ptr < ptr;
	}

	inline bool operator <= (const LegacyAutoPtr& ptr) const
	{
		return _ptr <= ptr._ptr;
	}

	inline bool operator <= (const C* ptr) const
	{
		return _ptr <= ptr;
	}

	inline bool operator <= (C* ptr) const
	{
		return _ptr <= ptr;
	}

	inline bool operator > (const LegacyAutoPtr& ptr) const
	{
		return _ptr > ptr._ptr;
	}

	inline bool operator > (const C* ptr) const
	{
		return _ptr > ptr;
	}

	inline bool operator > (C* ptr) const
	{
		return _ptr > ptr;
	}

	inline bool operator >= (const LegacyAutoPtr& ptr) const
	{
		return _ptr >= ptr._ptr;
	}

	inline bool operator >= (const C* ptr) const
	{
		return _ptr >= ptr;
	}

	inline bool operator >= (C* ptr) const
	{
		return _ptr >= ptr;
	}

private:
	C* _ptr;
};


template <class C>
inline void swap(AutoPtr<C>& p1, AutoPtr<C>& p2)
{
	p1.swap(p2);
}


template<class T>
using AutoPtr = RefPtr<T>;


} // namespace Poco


#endif // Foundation_AutoPtr_INCLUDED
