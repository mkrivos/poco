//
// QName.cpp
//
// $Id$
//
// Library: XML
// Package: XML
// Module:  QName
//
// Definition of the QName class.
//
// Copyright (c) 2004-2015, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// SPDX-License-Identifier:	BSL-1.0
// copyright : Copyright (c) 2009-2013 Code Synthesis Tools CC
// license   : MIT; see accompanying LICENSE file

#include "Poco/XML/QName.h"
#include <ostream>

using namespace std;

namespace Poco
{
namespace XML
{

QName::QName()
{
}


QName::QName(const std::string& name) :
	_name(name)
{
}


QName::QName(const std::string& ns, const std::string& name) :
	_ns(ns),
	_name(name)
{
}


QName::QName(const std::string& ns, const std::string& name, const std::string& prefix) :
	_ns(ns),
	_name(name),
	_prefix(prefix)
{
}


string QName::string() const
{
	std::string r;
	if (!_ns.empty())
	{
		r += _ns;
		r += '#';
	}

	r += _name;
	return r;
}

ostream& operator<<(ostream& os, const QName& qn)
{
	return os << qn.string();
}

}
}
