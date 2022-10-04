#include "core.h"
#include <sstream>

namespace
{
MachineMatrix CreateMatrix(size_t rows, size_t cols);

std::pair<size_t, size_t> ReadHeader(std::ifstream& file);

MachineMatrix ReadMatrix(std::ifstream& file, size_t rows, size_t cols);

std::string GetActionToken(std::stringstream& sstream);

Transition GetStateOutputPair(const std::string& token);
} // namespace

MachineMatrix ReadFile(const std::string& filename)
{
	std::ifstream file(filename);

	if (!file.is_open())
	{
		throw std::runtime_error("Unable to open file " + filename);
	}

	auto [k, m] = ReadHeader(file);

	return ReadMatrix(file, k, m);
}

void AddMooreStates(MachineMatrix& matrix, const TransitionSet& transitions)
{
	size_t rows{ matrix.size() };
	size_t cols{ matrix[0].size() };

	int nextIndex{ 0 };

	for (const auto& transition : transitions)
	{
		for (size_t i = 0; i < rows; i++)
		{
			for (size_t j = 0; j < cols; j++)
			{
				if (matrix[i][j].has_value() && matrix[i][j] == transition)
				{
					matrix[i][j].value().mooreState = nextIndex;
				}
			}
		}

		nextIndex++;
	}
}

TransitionSet CreateTransitionSet(const MachineMatrix& matrix)
{
	std::set<Transition, TransitionLessComparator> transitions;

	for (const auto& vec : matrix)
	{
		for (const auto& transition : vec)
		{
			if (transition.has_value())
			{
				transitions.insert(transition.value());
			}
		}
	}

	return transitions;
}

void WriteMooreMachineToStream(
	const MachineMatrix& matrix,
	const TransitionSet& transitions,
	std::ostream& stream)
{
	size_t cols{ matrix[0].size() };

	for (const auto& transition : transitions)
	{
		stream << "Y" << transition.output << " ";

		for (size_t j = 0; j < cols; j++)
		{
			if (matrix[transition.state][j].has_value()
				&& matrix[transition.state][j].value().mooreState != -1)
			{
				stream << "q" << matrix[transition.state][j].value().mooreState << " ";
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

	std::stringstream ss;
	std::string line;
	std::string token;

	for (int i = 0; i < rows; i++)
	{
		std::getline(file, line);
		ss.str(line);

		for (size_t j = 0; j < cols; j++)
		{
			token = GetActionToken(ss);
			if (token != "-")
			{
				matrix[i][j] = GetStateOutputPair(token);
			}
		}

		ss.clear();
		file >> std::ws;
	}

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

std::string GetActionToken(std::stringstream& sstream)
{
	std::string token;
	std::getline(sstream, token, ',');
	sstream >> std::ws;

	return token;
}

Transition GetStateOutputPair(const std::string& token)
{
	std::stringstream ss(token);
	int s{}, y{};

	ss.get();
	ss >> s;
	ss >> std::ws;
	ss.get();
	ss >> y;

	return { s, y };
}
} // namespace