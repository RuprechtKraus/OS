#pragma once
#include "Transition.h"
#include <fstream>
#include <iostream>
#include <optional>
#include <set>
#include <string>
#include <vector>

using MachineMatrix = std::vector<std::vector<std::optional<Transition>>>;
using TransitionSet = std::set<Transition, TransitionLessComparator>;

MachineMatrix ReadFile(const std::string& filename);

TransitionSet CreateTransitionSet(const MachineMatrix& matrix);

void AddMooreStates(MachineMatrix& matrix, const TransitionSet& transitions);

void WriteMooreMachineToStream(
	const MachineMatrix& matrix,
	const TransitionSet& transitions,
	std::ostream& stream = std::cout);