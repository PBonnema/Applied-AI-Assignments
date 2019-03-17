#pragma once
#include <cstddef>

class IPopMember;
template<std::size_t ...GROUPS>
class GAPopMember;

template<class TPopMember>
class EvolutionaryAlgorithm;

template<class TPopMember>
class SelectionStrategy;
template<class TPopMember>
class SelectionRoulette;
template<class TPopMember>
class SelectionSUS;
template<class TPopMember>
class SelectionTournament;
template<class TPopMember>
class SelectionTruncate;