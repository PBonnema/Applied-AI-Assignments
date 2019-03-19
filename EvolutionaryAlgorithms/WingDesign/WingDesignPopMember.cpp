#include "WingDesignPopMember.h"

double WingDesignPopMember::evaluateFitness()
{
	auto[A, B, C, D] = toPhenotype();
	// This is the function that we're trying to maximize. It turns out that using std::pow is slow.
	return (A - B) * (A - B) + (C + D) * (C + D) - (A - 30) * (A - 30) * (A - 30) - (C - 40) * (C - 40) * (C - 40);
}

void WingDesignPopMember::print(std::ostream & str) const
{
	auto[A, B, C, D] = toPhenotype();
	str << "A=" << A << "; B=" << B << "; C=" << C << "; D=" << D << "\t(";
	GAPopMember::print(str);
	str << ")";
}

double WingDesignPopMember::getFlipChance() const
{
	// Use some value that is proportional to the number of bits.
	return 0.8 / BIT_COUNT;
}

///////////////////
// Private
std::tuple<long, long, long, long> WingDesignPopMember::toPhenotype() const
{
	auto[A, B, C, D] = unpack();
	return {
		A.to_ulong(),
		B.to_ulong(),
		C.to_ulong(),
		D.to_ulong()
	};
}