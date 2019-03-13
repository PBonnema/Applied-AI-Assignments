#pragma once
#include "fwd_decl.h"
#include "Util.h"

#include <vector>
#include <execution>
#include <cassert>
#include <random>
#include <algorithm>

///////////////////
// SelectionStrategy
template<class TPopMember>
class SelectionStrategy
{
public:
	virtual ~SelectionStrategy() = default;

	virtual std::vector<TPopMember> select(const std::vector<TPopMember> & population, int membersToSelect) const = 0;
};

///////////////////
// SelectionTournament
template<class TPopMember>
class SelectionTournament : public SelectionStrategy<TPopMember>
{
public:
	virtual ~SelectionTournament() = default;

	virtual std::vector<TPopMember> select(const std::vector<TPopMember> & population, int membersToSelect) const override;
};

template<class TPopMember>
inline std::vector<TPopMember> SelectionTournament<TPopMember>::select(const std::vector<TPopMember> & population, int membersToSelect) const
{
}

///////////////////
// SelectionTruncate
template<class TPopMember>
class SelectionTruncate : public SelectionStrategy<TPopMember>
{
public:
	SelectionTruncate(double fraction);
	virtual ~SelectionTruncate() = default;

	virtual std::vector<TPopMember> select(const std::vector<TPopMember> & population, int membersToSelect) const override;

private:
	double fraction;
};

template<class TPopMember>
inline SelectionTruncate<TPopMember>::SelectionTruncate(double fraction)
	: SelectionStrategy<TPopMember>{}
	, fraction{ fraction }
{
}

template<class TPopMember>
inline std::vector<TPopMember> SelectionTruncate<TPopMember>::select(const std::vector<TPopMember> & population, int membersToSelect) const
{
	std::vector<TPopMember> selectees;
	selectees.reserve(membersToSelect);
	std::size_t maxIndex = std::max(static_cast<std::size_t>(population.size() * fraction), 1ull);
	for (int i = 0; i < membersToSelect; i++)
	{
		selectees.push_back(population[i % maxIndex]);
	}
	std::shuffle(selectees.begin(), selectees.end(), RANDOM_ENG);
	return selectees;
}

///////////////////
// SelectionRoulette
template<class TPopMember>
class SelectionRoulette : public SelectionStrategy<TPopMember>
{
private:
	static const std::uniform_real_distribution<long double> uniformDblDist;

public:
	virtual ~SelectionRoulette() = default;

	virtual std::vector<TPopMember> select(const std::vector<TPopMember> & population, int membersToSelect) const override;

private:
	long double random() const;
};

template<class TPopMember>
const std::uniform_real_distribution<long double> SelectionRoulette<TPopMember>::uniformDblDist{ 0, 1 };

template<class TPopMember>
inline std::vector<TPopMember> SelectionRoulette<TPopMember>::select(const std::vector<TPopMember> & population, int membersToSelect) const
{
	// Calculate the cumulative fitness values and put them in a vector
	std::vector<double> cumulativeNormalizedFitness(population.size());
	// Use lowestFitness for normalization if it is negative.
	// Multiply it by 2 so that the worst member won't end up with a fitness 0 zero, giving it a chance to be selected.
	double lowestFitness = std::min(population.back().getFitness() * 2, 0.0);
	//TODO profile par_unseq
	std::transform_inclusive_scan(std::execution::par_unseq,
		population.cbegin(), population.cend(), cumulativeNormalizedFitness.begin(),
		std::plus<>(),
		[lowestFitness](const TPopMember & member) { return member.getFitness() - lowestFitness; });

	double wheelSize = cumulativeNormalizedFitness.back();

	// Now do the actual roulette to select enough members
	std::vector<TPopMember> selectees;
	selectees.reserve(membersToSelect);
	for (int i = 0; i < membersToSelect; i++)
	{
		double pickedFitness = random() * wheelSize;
		auto selecteeIt = std::lower_bound(cumulativeNormalizedFitness.cbegin(), cumulativeNormalizedFitness.cend(), pickedFitness);
		std::size_t indexSelectee = selecteeIt - cumulativeNormalizedFitness.cbegin();
		selectees.push_back(population[indexSelectee]);
	}

	return selectees;
}

///////////////////
// Private
template<class TPopMember>
inline long double SelectionRoulette<TPopMember>::random() const
{
	return uniformDblDist(RANDOM_ENG);
}

///////////////////
// SelectionSUS
template<class TPopMember>
class SelectionSUS : public SelectionStrategy<TPopMember>
{
public:
	virtual ~SelectionSUS() = default;

	virtual std::vector<TPopMember> select(const std::vector<TPopMember> & population, int membersToSelect) const override;
};

template<class TPopMember>
inline std::vector<TPopMember> SelectionSUS<TPopMember>::select(const std::vector<TPopMember> & population, int membersToSelect) const
{
}