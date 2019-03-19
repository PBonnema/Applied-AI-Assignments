#pragma once
#include "IPopMember.h"
#include "Util.h"

#include <bitset>
#include <random>

/// <summary>
/// A convenience class that generalizes the concept of a genetic algorithm.
/// This class partially implements IPopMember and represents population members that use bitstrings internally as the genotype.
/// It takes care of printing to a stream, crossover and mutation.
/// </summary>
/// <remarks>
/// This class statically keeps track of the significant groups of bits in its bitstring.
/// A group of bits will never be split or partially inherited from a parent during crossover.
/// The GROUPS template argument contains the size of the groups as the number of bits from left to right.
/// GAPopMember<3, 4, 5> will contain 3 groups where the 3 left-most bits will be the first group.
/// </remarks>
template<std::size_t ...GROUPS>
class GAPopMember : public IPopMember
{
	static_assert(sizeof...(GROUPS) > 1);
	static_assert([](bool i) { return i; }(((GROUPS > 0) && ...)));// The use of lambda here is necessary due to a compiler bug. https://stackoverflow.com/questions/55130682/using-fold-expression-to-initialize-static-constexpr-class-data-member-doesnt-c

private:
	static const std::poisson_distribution<std::size_t> poissonIntDist;
	static const std::uniform_int_distribution<std::size_t> uniformIntDist;

public:
	/// <summary>
	/// The sum of all the group sizes.
	/// </summary>
	static constexpr std::size_t BIT_COUNT = [](int i) { return i; }((GROUPS + ...)); // The use of lambda here is necessary due to a compiler bug. https://stackoverflow.com/questions/55130682/using-fold-expression-to-initialize-static-constexpr-class-data-member-doesnt-c
	using Bits = std::bitset<BIT_COUNT>;

	GAPopMember();
	virtual ~GAPopMember() = default;

	/// <summary>
	/// Applies crossover using the 2 given parents where the current object is the child. Its current bitstring will be discarded.
	/// </summary>
	/// <remarks>
	/// The resulting bitstring will be constructed by appending consecutive groups from the parents,
	/// alternating between the parents and starting at parentA.
	/// </remarks>
	void crossover(const IPopMember & parentA, const IPopMember & parentB) override;

	/// <summary>
	/// Randomly flips bits. Each bit has an equal chance to flip. This chance is gotten from a call to getFlipChance().
	/// </summary>
	void mutate() override;
	void print(std::ostream & str) const override;

	/// <summary>
	/// Unpacks the bitstring into bitsets, 1 for each bitgroup.
	/// </summary>
	/// <returns> Returns the bits of the groups in the same order as they were specified.</returns>
	constexpr std::tuple<std::bitset<GROUPS>...> unpack() const;

	/// <returns> Must return the flip chance to be used during mutation.</returns>
	virtual double getFlipChance() const = 0;
	const Bits & getBits() const;

private:
	std::size_t randomPoisson(double mean) const;
	std::size_t randomUniformInt(std::size_t min, std::size_t max) const;

	Bits bits;
};

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
	// The Poisson distribution models the chance for each number of flips to occur given a mean number of flips.
	// Our mean number of flips equals the flip chance per bit times the number of bits.
	std::size_t flipCount = randomPoisson(getFlipChance() * BIT_COUNT);
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
inline constexpr std::tuple<std::bitset<GROUPS>...> GAPopMember<GROUPS...>::unpack() const
{
	int position = BIT_COUNT;
	// For each group, create a mask, shift the bitstring in the right position, bitwise-and our bits with the mask.
	return { std::bitset<GROUPS>{ (1 << GROUPS) - 1 } & std::bitset<GROUPS>{ (bits >> (position -= GROUPS)).to_ullong() }... };
}

template<std::size_t ...GROUPS>
inline const typename GAPopMember<GROUPS...>::Bits & GAPopMember<GROUPS...>::getBits() const
{
	return bits;
}

///////////////////
// Private
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