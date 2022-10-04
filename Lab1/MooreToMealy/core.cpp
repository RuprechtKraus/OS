#include "core.h"

namespace
{
std::pair<size_t, size_t> ReadHeader(std::ifstream& file);

MachineMatrix CreateMatrix(size_t rows, size_t cols);

MachineMatrix ReadMatrix(std::ifstream& file, size_t rows, size_t cols);

void MapOutputsToStatesInTransitions(MachineMatrix& matrix, std::map<int, int>& outputs);
}

MachineMatrix ReadFile(const std::string& filename)
{
	std::ifstream file(filename);

	if (!file.is_open())
	{
		throw std::runtime_error("Unable to open file " + filename);
	}

	auto [rows, cols] = ReadHeader(file);

	return ReadMatrix(file, rows, cols);
}

void WriteMealyMachineToStream(const MachineMatrix& matrix, std::ostream& stream)
{
	size_t rows{ matrix.size() };
	size_t cols{ matrix[0].size() };

	for (size_t i = 0; i < rows; i++)
	{
		for (size_t j = 0; j < cols; j++)
		{
			if (matrix[i][j].has_value())
			{
				const Transition& t = matrix[i][j].value();
				stream << std::format("S{} Y{} ", t.state, t.output);
			}
			else
			{
				stream << "- ";
			}
		}

		stream << std::endl;
	}
}

namespace
{
std::pair<size_t, size_t> ReadHeader(std::ifstream& file)
{
	size_t k{}, m{};

	file >> k >> m;
	file.get();

	return std::pair(k, m);
}

MachineMatrix ReadMatrix(std::ifstream& file, size_t rows, size_t cols)
{
	MachineMatrix matrix{ CreateMatrix(rows, cols) };
	std::map<int, int> outputs;

	int state{};
	int output{};

	for (size_t i = 0; i < rows; i++)
	{
		file.get();
		file >> output;
		file >> std::ws;

		outputs[static_cast<int>(i)] = output;

		for (size_t j = 0; j < cols; j++)
		{
			if (file.get() == '-')
			{
				file >> std::ws;
				continue;
			}

			file >> state;
			file >> std::ws;

			matrix[i][j] = Transition{ state, -1 };
		}
	}

	MapOutputsToStatesInTransitions(matrix, outputs);

	return matrix;
}

MachineMatrix CreateMatrix(size_t rows, size_t cols)
{
	MachineMatrix matrix;

	matrix.reserve(rows);
	matrix.resize(rows);

	for (auto& cell : matrix)
	{
		cell.reserve(cols);
		cell.resize(cols);
	}

	return matrix;
}

void MapOutputsToStatesInTransitions(MachineMatrix& matrix, std::map<int, int>& outputs)
{
	size_t rows{ matrix.size() };
	size_t cols{ matrix[0].size() };

	for (size_t i = 0; i < rows; i++)
	{
		for (size_t j = 0; j < cols; j++)
		{
			if (matrix[i][j].has_value())
			{
				matrix[i][j].value().output = outputs[matrix[i][j].value().state];
			}
		}
	}
}
} // namespace