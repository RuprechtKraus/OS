#include "MinimizeMealy.h"

int main(int argc, char* argv[])
try
{
	if (argc != 2)
	{
		std::cerr << "Expected one argument: <input file>" << std::endl;
		return 1;
	}

	std::ifstream file(argv[1]);

	if (!file.is_open())
	{
		std::cerr << "Cannot open input file" << std::endl;
		return 1;
	}

	MachineMatrix matrix;

	int statesCount, inputCount;
	file >> statesCount >> inputCount;

	InitializeMatrix(matrix, statesCount, inputCount);
	ReadMatrixFromFile(file, matrix, statesCount, inputCount);

	MachineMatrix minimizedMatrix = Minimize(matrix, statesCount, inputCount);
	WriteMachineMatrixToStream(minimizedMatrix, minimizedMatrix.size() - 1, inputCount);
}
catch (const std::exception& e)
{
	std::cerr << e.what() << std::endl;
}