#pragma once
#include <stdint.h>
class IDGenerator
{
public:
	IDGenerator() :m_ID(0) {};
	static IDGenerator& Instance();
	const uint32_t Next();
private:
	uint32_t m_ID;
};

IDGenerator& IDGenerator::Instance()
{
	static IDGenerator Generator;
	return Generator;
}

const uint32_t IDGenerator::Next()
{
	return m_ID++;
}

