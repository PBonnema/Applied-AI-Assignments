#pragma once
#include "IPopMember.h"
#include "Util.h"

#include <bitset>
#include <array>
#include <random>
#include <execution>
#include <utility>

template<std::size_t ...GROUPS>
class GAPopMember : public IPopMember
{
private:
	static const std::uniform_real_distribution<long double> uniformDblDist;
	static const std::poisson_distribution<std::size_t> poissonIntDist;
	static const std::uniform_int_distribution<std::size_t> uniformIntDist;

public:
	static constexpr std::size_t BIT_COUNT = [](int i) { return i; }((GROUPS + ...)); // The use of lambda here is necessary due to a suspected compiler bug. https://stackoverflow.com/questions/55130682/using-fold-expression-to-initialize-static-constexpr-class-data-member-doesnt-c
	using Bits = std::bitset<BIT_COUNT>;

	GAPopMember();
	virtual ~GAPopMember() = default;

	void crossover(const IPopMember & parentA, const IPopMember & parentB) override;
	void mutate() override;
	void print(std::ostream & str) const override;

	virtual double getChangeChance() const = 0;
	const Bits & getBits() const;

private:
	std::size_t randomPoisson(double mean) const;
	std::size_t randomUniformInt(std::size_t min, std::size_t max) const;

	Bits bits;
};

template<std::size_t ...GROUPS>
const std::uniform_real_distribution<long double> GAPopMember<GROUPS...>::uniformDblDist{ 0, 1 };
template<std::size_t ...GROUPS>
const std::poisson_distribution<std::size_t> GAPopMember<GROUPS...>::poissonIntDist;
template<std::size_t ...GROUPS>
const std::uniform_int_distribution<std::size_t> GAPopMember<GROUPS...>::uniformIntDist;

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
	bits = (((Bits{ (1 << GROUPS) - 1 } << (position -= GROUPS)) & ((useB = !useB) ? bitsA : bitsB)) | ...);
}

template<std::size_t ...GROUPS>
inline void GAPopMember<GROUPS...>::mutate()
{
	std::size_t flipCount = randomPoisson(getChangeChance() * BIT_COUNT);
	for (int i = 0; i < flipCount; ++i)
	{
		bits[randomUniformInt(0, BIT_COUNT - 1)].flip();
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

template<std::size_t ...GROUPS>
inline std::size_t GAPopMember<GROUPS...>::randomPoisson(double mean) const
{
	return poissonIntDist(RANDOM_ENG, decltype(poissonIntDist)::param_type{ mean });
}

template<std::size_t ...GROUPS>
inline std::size_t GAPopMember<GROUPS...>::randomUniformInt(std::size_t min, std::size_t max) const
{
	return uniformIntDist(RANDOM_ENG, decltype(uniformIntDist)::param_type{ min, max });
}

//////////////////////////
// Private