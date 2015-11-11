#pragma once
#include <string>
class TestList
{
public:
	typedef void* Test;

	Test* m_Head; //0x0000 
	Test* m_Tail; //0x0008 

};//Size=0x0010

struct MemberInfoFlags
{
	unsigned short m_FlagBits; //0x0000 

};//Size=0x0002

class ModuleInfo
{
public:
	char* m_ModuleName; //0x0000 
	ModuleInfo* m_NextModule; //0x0008 
	TestList* m_TestList; //0x0010 

};//Size=0x0018

enum BasicTypesEnum
{
	kTypeCode_Void,
	kTypeCode_DbObject,
	kTypeCode_ValueType,
	kTypeCode_Class,
	kTypeCode_Array,
	kTypeCode_FixedArray,
	kTypeCode_String,
	kTypeCode_CString,
	kTypeCode_Enum,
	kTypeCode_FileRef,
	kTypeCode_Boolean,
	kTypeCode_Int8,
	kTypeCode_Uint8,
	kTypeCode_Int16,
	kTypeCode_Uint16,
	kTypeCode_Int32,
	kTypeCode_Uint32,
	kTypeCode_Int64,
	kTypeCode_Uint64,
	kTypeCode_Float32,
	kTypeCode_Float64,
	kTypeCode_Guid,
	kTypeCode_SHA1,
	kTypeCode_ResourceRef,
	kTypeCode_BasicTypeCount
};

class MemberInfo
{
public:
#pragma pack(push, 2)
	struct MemberInfoData
	{
		char* m_Name; //0x0000
		MemberInfoFlags m_Flags; //0x0008

	};//Size=0x000A
#pragma pack(pop)

	MemberInfoData* m_InfoData; //0x0000

	BasicTypesEnum GetTypeCode();
	std::string GetTypeName();
	MemberInfoData* GetMemberInfoData();

};//Size=0x0008

BasicTypesEnum MemberInfo::GetTypeCode()
{
	MemberInfo::MemberInfoData* memberInfoData = GetMemberInfoData();
	if (memberInfoData)
	{
		return (BasicTypesEnum)((memberInfoData->m_Flags.m_FlagBits & 0x1F0) >> 0x4);
	}
	return kTypeCode_BasicTypeCount;
}

std::string MemberInfo::GetTypeName()
{
	switch (GetTypeCode())
	{
	case kTypeCode_Void: return "Void";
	case kTypeCode_DbObject: return "DbObject";
	case kTypeCode_ValueType: return "ValueType";
	case kTypeCode_Class: return "Class";
	case kTypeCode_Array: return "Array";
	case kTypeCode_FixedArray: return "FixedArray";
	case kTypeCode_String: return "String";
	case kTypeCode_CString: return "CString";
	case kTypeCode_Enum: return "Enum";
	case kTypeCode_FileRef: return "FileRef";
	case kTypeCode_Boolean: return "Boolean";
	case kTypeCode_Int8: return "Int8";
	case kTypeCode_Uint8: return "Uint8";
	case kTypeCode_Int16: return "Int16";
	case kTypeCode_Uint16: return "Uint16";
	case kTypeCode_Int32: return "Int32";
	case kTypeCode_Uint32: return "Uint32";
	case kTypeCode_Int64: return "Int64";
	case kTypeCode_Uint64: return "Uint64";
	case kTypeCode_Float32: return "Float32";
	case kTypeCode_Float64: return "Float64";
	case kTypeCode_Guid: return "Guid";
	case kTypeCode_SHA1: return "SHA1";
	case kTypeCode_ResourceRef: return "ResourceRef";
	default:
		char buffer[32];
		sprintf_s(buffer, "Undefined[%i]", GetTypeCode());
		return buffer;
	}
}

MemberInfo::MemberInfoData* MemberInfo::GetMemberInfoData()
{
	return ((MemberInfoData*)m_InfoData);
}

class TypeInfo : public MemberInfo
{
public:
	struct TypeInfoData : MemberInfoData
	{
		unsigned short m_TotalSize; //0x000A 
#if defined(_WIN64)
		char _0x000C[4];
#endif
		ModuleInfo* m_Module; //0x0010 
		unsigned char m_Alignment; //0x0018 
		unsigned char m_FieldCount; //0x0019 
#if defined(_WIN64)
		char _0x001A[6];
#else
		char _0x001A[2];
#endif

	};//Size=0x0020

	TypeInfo* m_Next; //0x0008
	unsigned short m_RuntimeId; //0x0010
	unsigned short m_Flags; //0x0012
#if defined(_WIN64)
	char _0x0014[4];
#endif

	TypeInfoData* GetTypeInfoData();
	static TypeInfo* GetFirst()
	{
		return *(TypeInfo**)OFFSET_FIRSTTYPEINFO;
	}
};//Size=0x0018

TypeInfo::TypeInfoData* TypeInfo::GetTypeInfoData()
{
	return ((TypeInfoData*)m_InfoData);
}

class FieldInfo : public MemberInfo
{
public:
	struct FieldInfoData : MemberInfo::MemberInfoData
	{
		unsigned short m_FieldOffset; //0x000A
#if defined(_WIN64)
		char _0x000C[4];
#endif
		TypeInfo* m_FieldTypePtr; //0x0010

	};//Size=0x0018

	virtual TypeInfo* GetDeclaringType();
	virtual unsigned short GetFieldIndex();

	TypeInfo* m_DeclaringType; //0x0010 
	unsigned short m_FieldIndex; //0x0018 
	unsigned short m_AttributeMask; //0x001A 
#if defined(_WIN64)
	char _0x001C[4];
#endif

	FieldInfoData* GetFieldInfoData();

};//Size=0x0020

FieldInfo::FieldInfoData* FieldInfo::GetFieldInfoData()
{
	return ((FieldInfoData*)m_InfoData);
}

class ClassInfo : public TypeInfo
{
public:
	struct ClassInfoData : TypeInfo::TypeInfoData
	{
		ClassInfo* m_SuperClass; //0x0020 
		void* m_CreateFun; //0x0028 
		FieldInfo::FieldInfoData* m_Fields; //0x0030     

	};//Size=0x0038

	ClassInfo* m_Super; //0x0018 
	void* m_DefaultInstance; //0x0020 
	unsigned short m_ClassId; //0x0028 
	unsigned short m_LastSubClassId; //0x002A 
#if defined(_WIN64)
	char _0x002C[4];
#endif
	ClassInfo* m_FirstDerivedClass; //0x0030 
	ClassInfo* m_NextSiblingClass; //0x0038 
	FieldInfo** m_FieldInfos; //0x0040 
	unsigned int m_TotalFieldCount; //0x0048 
#if defined(_WIN64)
	char _0x004C[4];
#endif

	ClassInfoData* GetClassInfoData();

};//Size=0x0050

ClassInfo::ClassInfoData* ClassInfo::GetClassInfoData()
{
	return ((ClassInfoData*)m_InfoData);
}

class ArrayTypeInfo : public TypeInfo
{
public:
	struct ArrayTypeInfoData : TypeInfo::TypeInfoData
	{
		TypeInfo* m_ElementType; //0x0020

	};//Size=0x0028

	ArrayTypeInfoData* GetArrayTypeInfoData();

};//Size=0x0018

ArrayTypeInfo::ArrayTypeInfoData* ArrayTypeInfo::GetArrayTypeInfoData()
{
	return ((ArrayTypeInfoData*)m_InfoData);
}

class EnumFieldInfo : public TypeInfo
{
public:
	struct EnumFieldInfoData : TypeInfo::TypeInfoData
	{
		FieldInfo::FieldInfoData* m_Fields; //0x0020     

	};//Size=0x0028

	FieldInfo** m_FieldInfos; //0x0018

	EnumFieldInfoData* GetEnumInfoData();

};//Size=0x0020

EnumFieldInfo::EnumFieldInfoData* EnumFieldInfo::GetEnumInfoData()
{
	return ((EnumFieldInfoData*)m_InfoData);
}

class ValueTypeInfo : public TypeInfo
{
public:
	struct ValueTypeInfoData : TypeInfo::TypeInfoData
	{

		void* m_DefaultValue;
		FieldInfo::FieldInfoData* m_Fields; //0x0020     

	};//Size=0x0028

	FieldInfo** m_FieldInfos; //0x0018

	ValueTypeInfoData* GetValueInfoData();

};//Size=0x0020

ValueTypeInfo::ValueTypeInfoData* ValueTypeInfo::GetValueInfoData()
{
	return ((ValueTypeInfoData*)m_InfoData);
}

class ITypedObject
{
public:
	virtual TypeInfo* GetType();
};//Size=0x0008
