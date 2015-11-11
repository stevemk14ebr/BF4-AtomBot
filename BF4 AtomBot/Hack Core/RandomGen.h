#pragma once
#ifndef RANDOMGEN_H
#define RANDOMGEN_H
#include <random>
#include <chrono>
#include <string>
#include <algorithm>
#include <sstream>
#include <locale>
#include <iomanip>

class RandomGen
{
public:
	RandomGen();
	~RandomGen();
	std::string GetTimeSeed(std::string Format = "%Y-%m-%d");

	//For integer types
	template<typename T>
	typename std::enable_if<std::is_integral<T>::value, T>::type GetRandom(T min, T max, bool UseDateSeed = true);

	//For decimal types
	template<typename T>
	typename std::enable_if<std::is_floating_point<T>::value, T>::type GetRandom(T min, T max, bool UseDateSeed = true);

	//For boolean type
	bool GetRandom(bool UseDateSeed = true);
private:
	std::mt19937 m_TrueRandMersenne32;
	std::mt19937 m_DateSeedMersenne32;
};

RandomGen::RandomGen()
{
	std::string dateseed = GetTimeSeed();
	std::seed_seq dateseedseq(dateseed.begin(), dateseed.end());
	m_DateSeedMersenne32 = std::mt19937(dateseedseq);

	uint_least32_t trueseed[std::mt19937::state_size];
	std::random_device randDevice;
	std::generate_n(trueseed, std::mt19937::state_size, std::ref(randDevice));
	std::seed_seq trueseedseq(std::begin(trueseed), std::end(trueseed));
	m_TrueRandMersenne32 = std::mt19937(trueseedseq);
}

RandomGen::~RandomGen()
{

}
std::string RandomGen::GetTimeSeed(std::string Format)
{
	std::stringstream ss;
	auto now = std::chrono::system_clock::now();
	auto now_c = std::chrono::system_clock::to_time_t(now);
	std::tm time;
	localtime_s(&time, &now_c);
	ss << std::put_time(&time, Format.c_str());
	return ss.str();
}

template<typename T>
typename std::enable_if<std::is_integral<T>::value, T>::type RandomGen::GetRandom(T min, T max, bool UseDateSeed)
{
	std::uniform_int_distribution<T> RandDist(min, max);

	if (UseDateSeed)
		return RandDist(m_DateSeedMersenne32);
	else
		return RandDist(m_TrueRandMersenne32);
}

template<typename T>
typename std::enable_if<std::is_floating_point<T>::value, T>::type RandomGen::GetRandom(T min, T max, bool UseDateSeed)
{
	std::uniform_real_distribution<T> RandDist(min, max);

	if (UseDateSeed)
		return RandDist(m_DateSeedMersenne32);
	else
		return RandDist(m_TrueRandMersenne32);
}

bool RandomGen::GetRandom(bool UseDateSeed)
{
	std::bernoulli_distribution RandDist;
	if (UseDateSeed)
		return RandDist(m_DateSeedMersenne32);
	else
		return RandDist(m_TrueRandMersenne32);
}
#endif