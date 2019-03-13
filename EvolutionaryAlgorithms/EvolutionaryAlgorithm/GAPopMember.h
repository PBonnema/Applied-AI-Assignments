#pragma once
#include "IPopMember.h"
#include "Util.h"

#include <bitset>
#include <array>
#include <random>
#include <execution>
#include <utility>

template<auto... t>
constexpr auto static_sum()
{
	return (t + ...);
}

template<std::size_t ...GROUPS>
class GAPopMember : public IPopMember
{
private:
	static const std::uniform_real_distribution<long double> uniformDblDist;

public:
	static constexpr std::size_t BIT_COUNT = static_sum<GROUPS...>(); // The use of static_sum here is necessary due to a suspected compiler bug. https://stackoverflow.com/questions/55130682/using-fold-expression-to-initialize-static-constexpr-class-data-member-doesnt-c
	using Bits = std::bitset<BIT_COUNT>;

	GAPopMember();
	virtual ~GAPopMember() = default;

	void crossover(const IPopMember & parentA, const IPopMember & parentB) override;
	void mutate() override;
	void print(std::ostream & str) const override;

	virtual double getChangeChance() const = 0;
	const Bits & getBits() const;

private:
	long double random() const;

	Bits bits;
};

template<std::size_t ...GROUPS>
const std::uniform_real_distribution<long double> GAPopMember<GROUPS...> ::uniformDblDist{ 0, 1 };

template<std::size_t ...GROUPS>
inline GAPopMember<GROUPS...>::GAPopMember()
	: bits{ randomizedBits<BIT_COUNT>() }
{
}

template<std::size_t ...GROUPS>
inline void GAPopMember<GROUPS...>::crossover(const IPopMember & parentA, const IPopMember & parentB)
{
	const Bits & bitsA = dynamic_cast<const GAPopMember<GROUPS...>&>(parentA).getBits();
	const Bits & bitsB = dynamic_cast<const GAPopMember<GROUPS...>&>(parentB).getBits();
	bool useB = false;
	int position = BIT_COUNT;
	// For each group, create a mask, shift the mask in the right position, bitwise-and the parents bits with the mask
	// while alternating between the parents. Then bitwise-or the results together.
	bits = (((Bits((1 << GROUPS) - 1) << (position -= GROUPS)) & ((useB = !useB) ? bitsA : bitsB)) | ...);
}

template<std::size_t ...GROUPS>
inline void GAPopMember<GROUPS...>::mutate()
{
	double changeChance = getChangeChance();
	for (int i = 0; i < BIT_COUNT; ++i)
	{
		if (random() <= changeChance)
		{
			bits[i].flip();
		}
	}
}

template<std::size_t ...GROUPS>
inline void GAPopMember<GROUPS...>::print(std::ostream & str) const
{
	str << bits.to_string();
}

template<std::size_t ...GROUPS>
inline const typename GAPopMember<GROUPS...>::Bits & GAPopMember<GROUPS...>::getBits() const
{
	return bits;
}

//////////////////////////
// Private
template<std::size_t ...GROUPS>
inline long double GAPopMember<GROUPS...>::random() const
{
	return uniformDblDist(RANDOM_ENG);
}