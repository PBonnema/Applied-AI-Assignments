#include "WingDesignPopMember.h"

#include <memory>

double WingDesignPopMember::evaluateFitness()
{
	auto[A, B, C, D] = toPhenotype();
	return std::pow(A - B, 2) + std::pow(C + D, 2) - std::pow(A - 30, 3) - std::pow(C - 40, 3);
}

void WingDesignPopMember::print(std::ostream & str) const
{
	auto[A, B, C, D] = toPhenotype();
	str << "A=" << A << "; B=" << B << "; C=" << C << "; D=" << D << "\t(";
	GAPopMember::print(str);
	str << ")";
}

double WingDesignPopMember::getChangeChance() const
{
	return 1.1 / BIT_COUNT;
}

std::tuple<long, long, long, long> WingDesignPopMember::toPhenotype() const
{
	const Bits & bits = getBits();
	Bits mask{ "111111" };
	long A = static_cast<long>((bits >> 18 & mask).to_ulong());
	long B = static_cast<long>((bits >> 12 & mask).to_ulong());
	long C = static_cast<long>((bits >> 6 & mask).to_ulong());
	long D = static_cast<long>((bits & mask).to_ulong());
	return { A, B, C, D };
}

WingDesignPopMember & WingDesignPopMember::assignToMe(const IPopMember & other)
{
	return *this = dynamic_cast<const WingDesignPopMember&>(other);
}

WingDesignPopMember & WingDesignPopMember::assignToMe(IPopMember && other)
{
	return *this = dynamic_cast<WingDesignPopMember&&>(std::move(other));
}