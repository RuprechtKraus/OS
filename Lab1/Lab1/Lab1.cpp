#include "core.h"
#include <iostream>

int main(int argc, char* argv[])
try
{
	if (argc != 2)
	{
		std::cout << "Expected arguments: <input file>" << std::endl;
		return 0;
	}

	MachineMatrix matrix{ ReadFile(std::string(argv[1])) };
	TransitionSet transitions{ CreateTransitionSet(matrix) };

	AddMooreStates(matrix, transitions);
	WriteMooreMachineToStream(matrix, transitions);
}
catch (const std::exception& e)
{
	std::cerr << e.what() << std::endl;
}