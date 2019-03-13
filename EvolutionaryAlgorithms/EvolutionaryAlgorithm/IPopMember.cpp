#include "pch.h"
#include "IPopMember.h"

#include <ostream>

IPopMember & IPopMember::assign(IPopMember & other)
{
	fitness = other.fitness;
	return assignToMe(other);
}

void IPopMember::evaluate()
{
	fitness = evaluateFitness();
}

double IPopMember::getFitness() const
{
	return fitness;
}

std::ostream & operator<<(std::ostream & str, const IPopMember & member)
{
	member.print(str);
	return str;
}
