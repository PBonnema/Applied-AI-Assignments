#pragma once

#ifdef NDEBUG
#undef NDEBUG
#endif

class IPopMember;
class ANNPopMember;
template<class TPopMember>
class EvolutionaryAlgorithm;
template<class TPopMember>
class SelectionStrategy;