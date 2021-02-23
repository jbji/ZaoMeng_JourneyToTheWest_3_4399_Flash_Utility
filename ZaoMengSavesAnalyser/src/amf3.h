//
// amf3.h
//
// The MIT License(MIT)
// 
// Copyright(c) 2016 Daisy <daisy at spitfire dot pw>
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef AMFLIB_H
#define	AMFLIB_H

#pragma once

#include <sstream>
#include <map>
#include <vector>
#include <memory>
#include <memory.h>

void ByteSwap5(unsigned char * b, int n);

class amf3object;
class amf3array;
class amf3objectmap;

enum Amf3TypeCode
{
	Undefined = 0,
	Null = 1,
	False = 2,
	True = 3,
	Integer = 4,
	Number = 5,
	String = 6,
	LegacyXmlDocument = 7,
	Date = 8,
	Array = 9,
	Object = 10,
	Xml = 11,
	ByteArray = 12
};
enum Flags
{
	Inline = 1,
	InlineClassDef = 2,
	Externalizable = 4,
	Dynamic = 8
};

template <typename T>
class amf3reflist
{
public:
	amf3reflist(void) {};
	~amf3reflist(void)
	{
		properties.clear();
		propnames.clear();
	};

	void AddObj(T obj)
	{
		properties.push_back(obj);
	}

	void AddObj(std::string key, T obj)
	{
		propnames.push_back(key);
		properties.push_back(obj);
	}

	T & GetObj(std::string key)
	{
		for (unsigned int i = 0; i < propnames.size(); ++i)
		{
			if (propnames[i] == key)
				return properties.at(i);
		}
		throw "Key: " + key + " does not exist in object";
	}

	T & GetObj(int offset)
	{
		return properties.at(offset);
	}

	bool Exists(std::string key)
	{
		for (unsigned int i = 0; i < propnames.size(); ++i)
		{
			if (propnames[i] == key)
				return true;
		}
		return false;
	}

	std::vector<std::string> propnames;
	std::vector<T> properties;
};
class amf3classdef
{
public:
	amf3classdef(void);
	amf3classdef(std::string name, std::vector<std::string> & properties, bool dynamic, bool externalizable);
	amf3classdef(const amf3classdef & classdef);
	~amf3classdef(void);

	std::string name;
	bool dynamic;
	bool externalizable;
	std::vector<std::string> properties;
	bool IsEqual(amf3classdef & obj) const
	{
		if (name != obj.name || dynamic != obj.dynamic || externalizable != obj.externalizable || (properties.size() != obj.properties.size()))
			return false;

		if (properties.size() == obj.properties.size())
		{
			for (uint32_t i = 0; i < properties.size(); ++i)
			{
				if (properties.at(i) != obj.properties.at(i))
					return false;
			}
		}
		else
		{
			return false;
		}
		return true;
	}
};
class amf3object
{
public:
	~amf3object(void);

	Amf3TypeCode type;

	static bool IsEqual(amf3object & obj, amf3object & obj2);

	static bool IsNEqual(amf3object & obj, amf3object & obj2)
	{
		return !(amf3object::IsEqual(obj, obj2));
	}
	std::string text;
	std::string date;
	union {
		bool booltest;
		int integer;
		double number;
	} _value;
	std::shared_ptr<amf3array> _array;
	std::shared_ptr<amf3objectmap> _object;

	const char * c_str() const
	{
		return text.c_str();
	}

	amf3object();
	amf3object(const amf3object &val);

	amf3object(const bool &val);
	amf3object(const int8_t &val);
	amf3object(const int16_t &val);
	amf3object(const int32_t &val);
	//amf3object3(const int &val);
	amf3object(const int64_t &val);
	amf3object(const uint8_t &val);
	amf3object(const uint16_t &val);
	amf3object(const uint32_t &val);
	amf3object(const uint64_t &val);
	amf3object(const double &val);
	void InternalCopy(const amf3object &val);
	void InternalCopy(const amf3array &val);

	amf3object & operator=(const amf3object &val);
	amf3object & operator=(const amf3array &val);
	amf3object & operator=(const bool &val);
	amf3object & operator=(const int8_t &val);
	amf3object & operator=(const int16_t &val);
	amf3object & operator=(const int32_t &val);
	//amf3object3 & operator=(const int &val);
	amf3object & operator=(const int64_t &val);
	amf3object & operator=(const uint8_t &val);
	amf3object & operator=(const uint16_t &val);
	amf3object & operator=(const uint32_t &val);
	amf3object & operator=(const uint64_t &val);
	amf3object & operator=(const double &val);

	//Variant & operator=(const Timestamp &val);

	amf3object & operator=(const char *pVal);
	amf3object & operator=(const std::string &val);

	operator Amf3TypeCode();
	operator bool();
	operator int8_t();
	operator int16_t();
	operator int32_t();
	//operator int();
	operator int64_t();
	operator uint8_t();
	operator uint16_t();
	operator uint32_t();
	operator uint64_t();
	operator double();
	//operator Timestamp();
	//operator char*();
	//operator const char*();
	operator std::string();
	operator amf3array*() const;
	operator amf3array&();
	void Reset(bool isUndefined = false);

	std::string temp;

	//amf3object & operator[](const string &key);
	amf3object & operator[](const char *key);
	amf3object & operator[](const double &key);
	amf3object & operator[](const uint32_t &key);
	amf3object & operator[](const int32_t &key);
	amf3object & operator[](amf3object &key);
	amf3object & GetValue(std::string key, bool caseSensitive);

	bool operator==(const amf3object variant) const;
	bool operator==(const char * str) const;
	bool operator==(char * str) const;
	bool operator==(const std::string str) const;
	bool operator!=(const amf3object variant) const;
	bool operator==(const Amf3TypeCode type) const;
	bool operator!=(const Amf3TypeCode type) const;

};
class amf3parser
{
public:
	amf3parser(char * stream);
	~amf3parser(void);

	amf3object  ReadNextObject(void);

	int ReadInteger(void);
	double ReadNumber(void);
	std::string ReadString(void);
	std::string ReadDate(void);
	amf3array * ReadArray(void);
	amf3objectmap * ReadAMF3Object(void);

	amf3reflist<std::string> strlist;
	amf3reflist<std::string> internalstrlist;
	amf3reflist<amf3object> objectlist;
	amf3reflist<amf3object> encapslist;
	amf3reflist<amf3classdef> deflist;

	char * stream;
	int position;
};
class amf3writer
{
public:
	amf3writer(char * stream);
	~amf3writer(void);

	bool CheckObjectTable(const amf3object & obj)
	{
		// TODO: Fix object tables
		// 		map<int, amf3object>::const_iterator iter;
		// 		iter = objectTable.begin();
		// 		for (uint32_t i = 0; i < objectTable.size(); ++iter, ++i)
		// 		{
		// 			if (amf3object::IsEqual((amf3object&)(iter->second), obj))
		// 			{
		// 				TypelessWrite(i << 1);
		// 				return true;
		// 			}
		// 		}
		return false;
	}
	void Write(Amf3TypeCode type);
	void Write(const amf3object & obj);
	void Write(short integer);
	void Write(char integer);
	void Write(unsigned short integer);
	void Write(unsigned char integer);
	void Write(unsigned int integer);
	void Write(int integer);
	void Write(double number);
	void Write(std::string str);
	void TypelessWrite(uint32_t integer);
	void TypelessWrite(int integer);
	void TypelessWrite(double number);
	void TypelessWrite(std::string str);
	void WriteDictionary(amf3reflist<amf3object> * reflist);
	void Write(amf3array * _array, const amf3object & obj);
	void Write(amf3objectmap * _object, const amf3object & obj);
	void TypelessWrite(amf3array * _array, const amf3object & obj);
	void TypelessWrite(amf3objectmap * _object, const amf3object & obj);

	amf3reflist<std::string> strlist;
	amf3reflist<amf3object> objectlist;
	amf3reflist<amf3object> encapslist;
	amf3reflist<amf3classdef> deflist;
	std::map<int, amf3object> objectTable;
	std::map<int, std::string> stringTable;
	std::map<int, amf3classdef> classdefTable;

	char * stream;
	int position;
};
class amf3objectmap
{
public:
	amf3objectmap(amf3classdef & classdef): flags(0)
	{
		selfdel = true;
		this->classdef = amf3classdef(classdef);
		anoncd = false;
	}

	amf3objectmap(amf3objectmap & objectmap): flags(0)
	{
		anoncd = false;
		classdef = objectmap.classdef;
		for (uint32_t i = 0; i < objectmap.properties.properties.size(); ++i)
		{
			properties.AddObj(objectmap.properties.propnames.at(i), objectmap.properties.GetObj(i));
		}
		selfdel = false;
	}
	amf3objectmap(): flags(0)
	{
		anoncd = true;
		selfdel = true;
	}
	~amf3objectmap(void);

	amf3object & Get(std::string key);
	void Add(std::string key, amf3object & obj);
	bool Exists(std::string key);

	bool IsEqual(amf3objectmap * obj)
	{
		for (uint32_t i = 0; i < properties.properties.size(); ++i)
		{
			if (amf3object::IsNEqual(properties.properties.at(i), obj->properties.properties.at(i)))
				return false;
		}
		return true;
	}

	bool anoncd;
	amf3classdef classdef;
	amf3reflist<amf3object> properties;
	int flags;
	bool selfdel;
};

class amf3array
{
public:
	amf3array(void);
	~amf3array(void);

	std::vector<amf3object>::iterator begin() { return dense.begin(); }
	std::vector<amf3object>::iterator end() { return dense.end(); }

	amf3object & operator[](const std::string key);
	amf3object & operator[](const uint32_t &key);

	void Add(amf3object & obj);
	void Add(std::string key, amf3object & obj);
	amf3object & Get(int id);
	amf3object & Get(std::string key);
	bool IsEqual(amf3array * obj)
	{
		if (this->type == 1)
		{
			if (this->dense.size() != obj->dense.size())
				return false;
			for (uint32_t i = 0; i < this->dense.size(); ++i)
			{
				if (amf3object::IsNEqual(this->dense.at(i), obj->dense.at(i)))
					return false;
			}
		}
		else if (this->type == 2)
		{
			if (this->associative.properties.size() != obj->associative.properties.size())
				return false;
			for (uint32_t i = 0; i < this->associative.properties.size(); ++i)
			{
				if (amf3object::IsNEqual(this->associative.properties.at(i), obj->associative.properties.at(i)))
					return false;
			}
		}
		return true;
	}


	amf3reflist<amf3object> associative;
	std::vector<amf3object> dense;

	char type;
};

#endif	/* AMFLIB_H */

