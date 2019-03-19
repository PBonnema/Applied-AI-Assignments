#pragma once
#include "EvolutionaryAlgorithm\fwd_decl.h"
#include "EvolutionaryAlgorithm\GAPopMember.h"

#include <iosfwd>

/// <summary>
/// This class models the population member for the Wing Design problem by inheriting from GAPopMember.
/// Its fitness is the lift of the design it represents.
/// </summary>
/// <remarks>
/// This class defines a group for each parameter for the equation that calculates lift.
/// The groups represent A, B, C and D in their respective order.
/// </remarks>
class WingDesignPopMember : public GAPopMember<6, 6, 6, 6>
{
public:
	virtual ~WingDesignPopMember() = default;

	double evaluateFitness() override;
	void print(std::ostream & str) const override;

	double getFlipChance() const override;

private:
	std::tuple<long, long, long, long> toPhenotype() const;
};
