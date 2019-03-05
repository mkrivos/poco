//
// LOB.h
//
// Library: SQL
// Package: SQLCore
// Module:  LOB
//
// Definition of the LOB class.
//
// Copyright (c) 2006, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// SPDX-License-Identifier:	BSL-1.0
//


#ifndef SQL_LOB_INCLUDED
#define SQL_LOB_INCLUDED


#include "Poco/SQL/SQL.h"
#include "Poco/SharedPtr.h"
#include "Poco/Dynamic/VarHolder.h"
#include "Poco/Exception.h"
#include <vector>
#include <algorithm>


namespace Poco {
namespace SQL {


template <typename T>
class LOB
	/// Representation of a Large OBject.
	///
	/// A LOB can hold arbitrary data.
	/// The maximum size depends on the underlying database.
	///
	/// The LOBInputStream and LOBOutputStream classes provide
	/// a convenient way to access the data in a LOB.
{
public:
	typedef typename std::vector<T>::const_iterator Iterator;
	typedef T ValueType;
	typedef typename std::vector<T> Container;
	typedef Poco::SharedPtr<Container> ContentPtr;

	LOB(): _pContent(new std::vector<T>())
		/// Creates an empty LOB.
	{
	}

	LOB(const std::vector<T>& rContent):
		_pContent(new std::vector<T>(rContent))
		/// Creates the LOB, content is deep-copied.
	{
	}

	LOB(const T* const pContent, std::size_t objectSize):
		_pContent(new std::vector<T>(pContent, pContent + objectSize))
		/// Creates the LOB by deep-copying pContent.
	{
	}

	LOB(const std::basic_string<T>& rContent):
		_pContent(new std::vector<T>(rContent.begin(), rContent.end()))
		/// Creates a LOB from a string.
	{
	}

	LOB(const LOB& other): _pContent(other._pContent)
		/// Creates a LOB by copying another one.
	{
	}

	~LOB()
		/// Destroys the LOB.
	{
	}

	LOB& operator = (const LOB& other)
		/// Assignment operator.
	{
		LOB tmp(other);
		swap(tmp);
		return *this;
	}

	bool operator == (const LOB& other) const
		/// Compares for equality LOB by value.
	{
		return *_pContent == *other._pContent;
	}

	bool operator != (const LOB& other) const
		/// Compares for inequality LOB by value.
	{
		return *_pContent != *other._pContent;
	}

	void swap(LOB& other)
		/// Swaps the LOB with another one.
	{
		using std::swap;
		swap(_pContent, other._pContent);
	}

	const std::vector<T>& content() const
		/// Returns the content.
	{
		return *_pContent;
	}

	const T* rawContent() const
		/// Returns the raw content.
		///
		/// If the LOB is empty, returns NULL.
	{
		if (_pContent->empty())
			return 0;
		else
			return &(*_pContent)[0];
	}

	void assignVal(std::size_t count, const T& val)
		/// Assigns raw content to internal storage.
	{
		ContentPtr tmp = new Container(count, val);
		_pContent.swap(tmp);
	}

	void assignRaw(const T* ptr, std::size_t count)
		/// Assigns raw content to internal storage.
	{
		poco_assert_dbg (ptr);
		LOB tmp(ptr, count);
		swap(tmp);
	}

	void appendRaw(const T* pChar, std::size_t count)
		/// Assigns raw content to internal storage.
	{
		poco_assert_dbg (pChar);
		_pContent->insert(_pContent->end(), pChar, pChar+count);
	}

	void clear(bool doCompact = false)
		/// Clears the content of the blob.
		/// If doCompact is true, trims the excess capacity.
	{
		_pContent->clear();
		if (doCompact) compact();
	}

	void compact()
		/// Trims the internal storage excess capacity.
	{
		std::vector<T>(*_pContent).swap(*_pContent);
	}

	Iterator begin() const
	{
		return _pContent->begin();
	}

	Iterator end() const
	{
		return _pContent->end();
	}

	std::size_t size() const
		/// Returns the size of the LOB in bytes.
	{
		return static_cast<std::size_t>(_pContent->size());
	}

private:
	ContentPtr _pContent;
};


typedef LOB<unsigned char> BLOB;
typedef LOB<char> CLOB;


//
// inlines
//

template <typename T>
inline void swap(LOB<T>& b1, LOB<T>& b2)
{
	b1.swap(b2);
}


} } // namespace Poco::SQL


namespace std
{
	template<>
	inline void swap<Poco::SQL::BLOB>(Poco::SQL::BLOB& b1,
		Poco::SQL::BLOB& b2)
		/// Full template specialization of std:::swap for BLOB
	{
		b1.swap(b2);
	}

	template<>
	inline void swap<Poco::SQL::CLOB>(Poco::SQL::CLOB& c1,
		Poco::SQL::CLOB& c2)
		/// Full template specialization of std:::swap for CLOB
	{
		c1.swap(c2);
	}
}


//
// VarHolderImpl<LOB>
//


namespace Poco {
namespace Dynamic {


template <>
class VarHolderImpl<Poco::SQL::BLOB>: public VarHolder
{
public:
	VarHolderImpl(const Poco::SQL::BLOB& val): _val(val)
	{
	}

	~VarHolderImpl()
	{
	}
	
	const std::type_info& type() const
	{
		return typeid(Poco::SQL::BLOB);
	}

	void convert(std::string& val) const
	{
		val.assign(_val.begin(), _val.end());
	}

	VarHolder* clone(Placeholder<VarHolder>* pVarHolder = 0) const
	{
		return cloneHolder(pVarHolder, _val);
	}
	
	const Poco::SQL::BLOB& value() const
	{
		return _val;
	}

private:
	VarHolderImpl();
	Poco::SQL::BLOB _val;
};


template <>
class VarHolderImpl<Poco::SQL::CLOB>: public VarHolder
{
public:
	VarHolderImpl(const Poco::SQL::CLOB& val): _val(val)
	{
	}

	~VarHolderImpl()
	{
	}
	
	const std::type_info& type() const
	{
		return typeid(Poco::SQL::CLOB);
	}

	void convert(std::string& val) const
	{
		val.assign(_val.begin(), _val.end());
	}

	VarHolder* clone(Placeholder<VarHolder>* pVarHolder = 0) const
	{
		return cloneHolder(pVarHolder, _val);
	}
	
	const Poco::SQL::CLOB& value() const
	{
		return _val;
	}

private:
	VarHolderImpl();
	Poco::SQL::CLOB _val;
};


} } // namespace Poco::Dynamic


#endif // Data_LOB_INCLUDED
