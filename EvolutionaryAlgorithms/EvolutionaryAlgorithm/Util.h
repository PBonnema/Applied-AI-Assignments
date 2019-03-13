#pragma once

#include <random>
#include <bitset>

inline std::mt19937_64 RANDOM_ENG{ std::random_device{}() }; //Standard mersenne_twister_engine
template<std::size_t N>
inline std::bitset<N> randomizedBits()
{
	static std::independent_bits_engine<decltype(RANDOM_ENG), N, unsigned long> bitEngine{ RANDOM_ENG };
	return std::bitset<N>{ bitEngine() };
}