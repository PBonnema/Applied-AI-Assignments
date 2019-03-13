#pragma once
#include "EvolutionaryAlgorithm\fwd_decl.h"
#include "EvolutionaryAlgorithm\GAPopMember.h"

#include <iosfwd>

class WingDesignPopMember : public GAPopMember<6, 6, 6, 6>
{
public:
	virtual ~WingDesignPopMember() = default;

	double evaluateFitness() override;
	void print(std::ostream & str) const override;

	double getChangeChance() const override;

private:
	std::tuple<long, long, long, long> toPhenotype() const;

	// Polymorphic assignment operators
	WingDesignPopMember & assignToMe(const IPopMember & other) override;
	WingDesignPopMember & assignToMe(IPopMember && other) override;
};
