#include "core.h"
#include <iostream>

int main(int argc, char* argv[])
try
{
	if (argc != 2)
	{
		std::cout << "Expected arguments: <input file>" << std::endl;
	}

	MachineMatrix matrix{ ReadFile(argv[1]) };
	WriteMealyMachineToStream(matrix);
}
catch (const std::exception& e)
{
	std::cerr << e.what() << std::endl;
}