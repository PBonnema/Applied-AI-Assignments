#include "WingDesignPopMember.h"

#include <memory>

double WingDesignPopMember::evaluateFitness()
{
	auto[A, B, C, D] = toPhenotype();
	return (A - B) * (A - B) + (C + D) * (C + D) - (A - 30) * (A - 30) * (A - 30) - (C - 40) * (C - 40) * (C - 40);
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
	return 0.8 / BIT_COUNT;
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