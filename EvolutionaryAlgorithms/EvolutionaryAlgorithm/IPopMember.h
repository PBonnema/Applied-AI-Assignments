#pragma once
#include <iosfwd>

/// <summary>
/// A convenience interface class that defines the necessary members that PopMember classes must implement
/// to be able to be used by the evolutionary algorithm. However, it is not mandatory to derive from this class.
/// </summary>
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

	/// <returns> Will simply returned the cached fitness. Call evaluate() to recalculate the fitness.</returns>
	double getFitness() const;

private:
	double fitness;
};