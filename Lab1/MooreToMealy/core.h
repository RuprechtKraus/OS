#pragma once
#include <format>
#include <fstream>
#include <iostream>
#include <map>
#include <optional>
#include <vector>

struct Transition
{
	int state;
	int output;
};

using MachineMatrix = std::vector<std::vector<std::optional<Transition>>>;

MachineMatrix ReadFile(const std::string& filename);

void WriteMealyMachineToStream(const MachineMatrix& matrix, std::ostream& stream = std::cout);