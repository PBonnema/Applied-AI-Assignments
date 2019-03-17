#pragma once
#include <iosfwd>

class IPopMember
{
public:
	IPopMember() = default;
	virtual ~IPopMember() = default;

	friend std::ostream & operator<<(std::ostream & str, const IPopMember & member);

	void evaluate();
	virtual double evaluateFitness() = 0;
	virtual void crossover(const IPopMember & parentA, const IPopMember & parentB) = 0;
	virtual void mutate() = 0;
	virtual void print(std::ostream & str) const = 0;

	double getFitness() const;

private:
	double fitness;
};