//#include "pch.h"
//#include "ANNPopMember.h"
//#include "NeuralNetwork\NeuralNetwork.h"
//
//ANNPopMember::ANNPopMember(const NeuralNetwork & network)
//	: IPopMember{}
//	, network { network }
//{
//}
//
//ANNPopMember::ANNPopMember(NeuralNetwork && network)
//	: IPopMember{}
//	, network{ std::move(network) }
//{
//}
//
//void ANNPopMember::crossover(const IPopMember & parentA, const IPopMember & parentB)
//{
//	assert(false);
//}
//
//void ANNPopMember::mutate()
//{
//	network.mutate(getChangeChance(), getChangeRate());
//	assert(false);
//}
//
//void ANNPopMember::print(std::ostream & str) const
//{
//	assert(false);
//	str << network;
//}
//
////////////////////////////
//// Data access
//NeuralNetwork & ANNPopMember::getNetwork()
//{
//	return network;
//}
//
////////////////////////////
//// Protected
////bool ANNPopMember::mutateNeuronWeights(Neuron & neuron)
////{
////	bool changed = false;
////	//if (std::abs(NeuralNetwork::uniformDblDist(NeuralNetwork::randomGen)) < getChangeChance())
////	//{
////	//	double change = NeuralNetwork::uniformDblDist(NeuralNetwork::randomGen) * getChangeRate();
////	//	neuron.setBias(std::clamp(neuron.getBias() + change, -1.0, 1.0));
////	//	changed = true;
////	//}
////
////	//for (double & weight : neuron.getInputWeights())
////	//{
////	//	if (std::abs(NeuralNetwork::uniformDblDist(NeuralNetwork::randomGen)) < getChangeChance())
////	//	{
////	//		double change = NeuralNetwork::uniformDblDist(NeuralNetwork::randomGen) * getChangeRate();
////	//		weight = std::clamp(weight + change, -1.0, 1.0);
////	//		changed = true;
////	//	}
////	//}
////	return changed;
////}