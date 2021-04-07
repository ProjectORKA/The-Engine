#pragma once

#include "Basics.hpp"

//neurons are connected to each other
//reward strengthens neuron

using Connection = Index;

struct Neuron {
	Vector<Connection> connections;
	Float health;
	Float activation;
};