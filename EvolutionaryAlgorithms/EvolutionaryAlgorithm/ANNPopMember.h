//#pragma once
//#include "fwd_decl.h"
//#include "IPopMember.h"
//#include "NeuralNetwork\NeuralNetwork.h"
//
//class ANNPopMember : public IPopMember
//{
//public:
//	ANNPopMember(const NeuralNetwork & network);
//	ANNPopMember(NeuralNetwork && network);
//	virtual ~ANNPopMember() = default;
//
//	void crossover(const IPopMember & parentA, const IPopMember & parentB) override;
//	void mutate() override;
//	void print(std::ostream & str) const override;
//
//	virtual double getChangeChance() const = 0;
//	virtual double getChangeRate() const = 0;
//
//	//////////////////////////
//	// Data access
//	NeuralNetwork & getNetwork();
//
//protected:
//	//virtual bool mutateNeuronWeights(Neuron & neuron);
//
//	NeuralNetwork network;
//};