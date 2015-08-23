//
// XMLStreamSerializer.cpp
//
// $Id$
//
// Library: XML
// Package: XML
// Module:  XMLStreamSerializer
//
// Definition of the XMLStreamSerializer class.
//
// Copyright (c) 2004-2015, Applied Informatics Software Engineering GmbH.
// and Contributors.
//
// SPDX-License-Identifier:	BSL-1.0
// copyright : Copyright (c) 2013-2014 Code Synthesis Tools CC
// license   : MIT; see accompanying LICENSE file


#include "Poco/XML/XMLStreamSerializer.h"
#include "Poco/XML/XMLStreamSerializerException.h"
#include <new>     // std::bad_alloc
#include <cstring> // std::strlen


using namespace std;


namespace Poco
{
namespace XML
{


extern "C" genxStatus genx_write(void* p, constUtf8 us)
{
	// It would have been easier to throw the exception directly,
	// however, the Genx code is most likely not exception safe.
	//
	ostream* os(static_cast<ostream*>(p));
	const char* s(reinterpret_cast<const char*>(us));
	os->write(s, static_cast<streamsize>(strlen(s)));
	return os->good() ? GENX_SUCCESS : GENX_IO_ERROR;
}


extern "C" genxStatus genx_write_bound(void* p, constUtf8 start, constUtf8 end)
{
	ostream* os(static_cast<ostream*>(p));
	const char* s(reinterpret_cast<const char*>(start));
	streamsize n(static_cast<streamsize>(end - start));
	os->write(s, n);
	return os->good() ? GENX_SUCCESS : GENX_IO_ERROR;
}


extern "C" genxStatus genx_flush(void* p)
{
	ostream* os(static_cast<ostream*>(p));
	os->flush();
	return os->good() ? GENX_SUCCESS : GENX_IO_ERROR;
}


XMLStreamSerializer::~XMLStreamSerializer()
{
	if (_writer != 0)
		genxDispose (_writer);
}


XMLStreamSerializer::XMLStreamSerializer(ostream& os, const string& oname, unsigned short ind) :
		_outputStream(os),
		_lastStreamState(os.exceptions()),
		_outputName(oname),
		_depth(0)
{
	// Temporarily disable exceptions on the stream.
	//
	_outputStream.exceptions(ostream::goodbit);

	// Allocate the XMLStreamSerializer. Make sure nothing else can throw after
	// this call since otherwise we will leak it.
	//
	_writer = genxNew(0, 0, 0);

	if (_writer == 0)
		throw bad_alloc();

	genxSetUserData(_writer, &_outputStream);

	if (ind != 0)
		genxSetPrettyPrint(_writer, ind);

	_sender.send = &genx_write;
	_sender.sendBounded = &genx_write_bound;
	_sender.flush = &genx_flush;

	if (genxStatus e = genxStartDocSender(_writer, &_sender))
	{
		string m(genxGetErrorMessage(_writer, e));
		genxDispose (_writer);
		throw XMLStreamSerializerException(oname, m);
	}
}


void XMLStreamSerializer::handleError(genxStatus e) const
{
	switch (e)
	{
	case GENX_ALLOC_FAILED:
		throw bad_alloc();
	case GENX_IO_ERROR:
		// Restoring the original exception state should trigger the
		// exception. If it doesn't (e.g., because the user didn't
		// configure the stream to throw), then fall back to the
		// serialiation exception.
		//
		_outputStream.exceptions(_lastStreamState);
		// Fall through.
	default:
		throw XMLStreamSerializerException(_outputName, genxGetErrorMessage(_writer, e));
	}
}


void XMLStreamSerializer::startElement(const string& ns, const string& name)
{
	if (genxStatus e = genxStartElementLiteral(_writer, reinterpret_cast<constUtf8>(ns.empty() ? 0 : ns.c_str()), reinterpret_cast<constUtf8>(name.c_str())))
		handleError(e);

	_depth++;
}


void XMLStreamSerializer::endElement()
{
	if (genxStatus e = genxEndElement(_writer))
		handleError(e);

	// Call EndDocument() if we are past the root element.
	//
	if (--_depth == 0)
	{
		if (genxStatus e = genxEndDocument(_writer))
			handleError(e);

		// Also restore the original exception state on the stream.
		//
		_outputStream.exceptions(_lastStreamState);
	}
}


void XMLStreamSerializer::endElement (const string& ns, const string& name)
{
  constUtf8 cns, cn;
  genxStatus e;
  if ((e = genxGetCurrentElement (_writer, &cns, &cn)) ||
      reinterpret_cast<const char*> (cn) != name ||
      (cns == 0 ? !ns.empty () : reinterpret_cast<const char*> (cns) != ns))
  {
    handleError (e != GENX_SUCCESS ? e : GENX_SEQUENCE_ERROR);
  }

  endElement ();
}


void XMLStreamSerializer::element(const string& ns, const string& n, const string& v)
{
	startElement(ns, n);
	element(v);
}


void XMLStreamSerializer::startAttribute(const string& ns, const string& name)
{
	if (genxStatus e = genxStartAttributeLiteral(_writer, reinterpret_cast<constUtf8>(ns.empty() ? 0 : ns.c_str()), reinterpret_cast<constUtf8>(name.c_str())))
		handleError(e);
}


const std::string& XMLStreamSerializer::outputName() const
{
	return _outputName;
}


void XMLStreamSerializer::endAttribute()
{
	if (genxStatus e = genxEndAttribute(_writer))
		handleError(e);
}


void XMLStreamSerializer::endAttribute (const string& ns, const string& name)
{
  constUtf8 cns, cn;
  genxStatus e;
  if ((e = genxGetCurrentAttribute (_writer, &cns, &cn)) ||
      reinterpret_cast<const char*> (cn) != name ||
      (cns == 0 ? !ns.empty () : reinterpret_cast<const char*> (cns) != ns))
  {
    handleError (e != GENX_SUCCESS ? e : GENX_SEQUENCE_ERROR);
  }

  endAttribute ();
}


void XMLStreamSerializer::attribute(const string& ns, const string& name, const string& value)
{
	if (genxStatus e = genxAddAttributeLiteral(_writer, reinterpret_cast<constUtf8>(ns.empty() ? 0 : ns.c_str()), reinterpret_cast<constUtf8>(name.c_str()),
			reinterpret_cast<constUtf8>(value.c_str())))
		handleError(e);
}


void XMLStreamSerializer::characters(const string& value)
{
	if (genxStatus e = genxAddCountedText(_writer, reinterpret_cast<constUtf8>(value.c_str()), value.size()))
		handleError(e);
}


void XMLStreamSerializer::namespaceDecl(const string& ns, const string& p)
{
	if (genxStatus e =
			ns.empty() && p.empty() ?
					genxUnsetDefaultNamespace(_writer) :
					genxAddNamespaceLiteral(_writer, reinterpret_cast<constUtf8>(ns.c_str()), reinterpret_cast<constUtf8>(p.c_str())))
		handleError(e);
}


void XMLStreamSerializer::xmlDecl(const string& ver, const string& enc, const string& stl)
{
	if (genxStatus e = genxXmlDeclaration(_writer, reinterpret_cast<constUtf8>(ver.c_str()), (enc.empty() ? 0 : reinterpret_cast<constUtf8>(enc.c_str())),
			(stl.empty() ? 0 : reinterpret_cast<constUtf8>(stl.c_str()))))
		handleError(e);
}


void XMLStreamSerializer::doctypeDecl (const string& re,
              const string& pi,
              const string& si,
              const string& is)
{
  if (genxStatus e = genxDoctypeDeclaration (
        _writer,
        reinterpret_cast<constUtf8> (re.c_str ()),
        (pi.empty () ? 0 : reinterpret_cast<constUtf8> (pi.c_str ())),
        (si.empty () ? 0 : reinterpret_cast<constUtf8> (si.c_str ())),
        (is.empty () ? 0 : reinterpret_cast<constUtf8> (is.c_str ()))))
    handleError (e);
}


bool XMLStreamSerializer::lookupNamespacePrefix(const string& ns, string& p) const
{
	// Currently Genx will create a namespace mapping if one doesn't
	// already exist.
	//
	genxStatus e;
	genxNamespace gns(genxDeclareNamespace(_writer, reinterpret_cast<constUtf8>(ns.c_str()), 0, &e));

	if (e != GENX_SUCCESS)
		handleError(e);

	p = reinterpret_cast<const char*>(genxGetNamespacePrefix(gns));
	return true;
}


QName XMLStreamSerializer::currentElement () const
{
  constUtf8 ns, n;
  if (genxStatus e = genxGetCurrentElement (_writer, &ns, &n))
    handleError (e);

  return QName (ns != 0 ? reinterpret_cast<const char*> (ns) : "", reinterpret_cast<const char*> (n));
}


QName XMLStreamSerializer::currentAttribute () const
{
  constUtf8 ns, n;
  if (genxStatus e = genxGetCurrentAttribute (_writer, &ns, &n))
    handleError (e);

  return QName (ns != 0 ? reinterpret_cast<const char*> (ns) : "", reinterpret_cast<const char*> (n));
}


void XMLStreamSerializer::suspendIndentation ()
{
  if (genxStatus e = genxSuspendPrettyPrint (_writer))
    handleError (e);
}


void XMLStreamSerializer::resumeIndentation ()
{
  if (genxStatus e = genxResumePrettyPrint (_writer))
    handleError (e);
}


size_t XMLStreamSerializer::indentationSuspended () const
{
  return static_cast<size_t> (genxPrettyPrintSuspended (_writer));
}


}
}
