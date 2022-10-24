#pragma once
#include <algorithm>
#include <fstream>
#include <iostream>
#include <ranges>
#include <string>
#include <unordered_map>
#include <vector>

struct Transition
{
	int state;
	int output;
};

using MachineMatrix = std::vector<std::vector<Transition>>;

// Первый элемент - группа
// Второй элемент - состояние
using GroupTransitionColumn = std::tuple<int, int, std::vector<Transition>>;
using GroupTransitionTable = std::vector<GroupTransitionColumn>;

#pragma region Declarations
MachineMatrix Minimize(const MachineMatrix& matrix, int rows, int cols);

void InitializeMatrix(MachineMatrix& matrix, int rows, int cols);

void ReadMatrixFromFile(std::ifstream& file, MachineMatrix& dest, int rows, int cols);

GroupTransitionTable StepZero(const MachineMatrix& matrix, int rows, int cols);

GroupTransitionTable StepOne(const MachineMatrix& matrix,
	const GroupTransitionTable& previousGroups, int rows, int cols);

MachineMatrix CreateMachineFromGroupTable(const MachineMatrix& originalMatrix,
	const GroupTransitionTable& groupTable, int rows, int cols);

void WriteMachineMatrixToStream(const MachineMatrix& matrix,
	int rows, int cols, std::ostream& os = std::cout);
#pragma endregion Declarations

#pragma region Implementations
MachineMatrix Minimize(const MachineMatrix& matrix, int rows, int cols)
{
	GroupTransitionTable groups = StepZero(matrix, rows + 1, cols);

	GroupTransitionTable prevGroups = groups;
	GroupTransitionTable newGroups;

	int prevGroupsCount = std::get<0>(*(prevGroups.end() - 1));
	int newGroupsCount = 0;

	while (newGroupsCount != prevGroupsCount)
	{
		newGroups = StepOne(matrix, prevGroups, rows + 1, cols);
		prevGroupsCount = newGroupsCount;
		newGroupsCount = std::get<0>(*(newGroups.end() - 1));
		prevGroups = newGroups;
	}

	return CreateMachineFromGroupTable(matrix, newGroups, rows + 1, cols);
}

void InitializeMatrix(MachineMatrix& matrix, int rows, int cols)
{
	matrix.reserve(static_cast<size_t>(rows + 1));
	matrix.resize(static_cast<size_t>(rows + 1));

	for (auto& row : matrix)
	{
		row.reserve(cols);
		row.resize(cols);
	}

	for (size_t i = 0; i < cols; i++)
	{
		matrix[rows][i].state = rows;
		matrix[rows][i].output = -1;
	}
}

void ReadMatrixFromFile(std::ifstream& file, MachineMatrix& dest, int rows, int cols)
{
	std::string token;

	for (size_t i = 0; i < rows; i++)
	{
		for (size_t j = 0; j < cols; j++)
		{
			file >> token;

			if (token != "-")
			{
				dest[i][j].state = std::stoi(token);
				file >> token;
				dest[i][j].output = std::stoi(token);
			}
			else
			{
				dest[i][j].state = rows;
				dest[i][j].output = -1;
			}
		}
	}
}

#pragma warning(disable : 26800)
GroupTransitionTable StepZero(const MachineMatrix& matrix, int rows, int cols)
{
	GroupTransitionTable groupsTable;
	GroupTransitionColumn transitionColumn;
	std::unordered_map<std::string, int> outputGroup;
	std::string output;
	int nextGroupIndex = 0;
	int groupIndex = 0;

	for (size_t i = 0; i < rows; i++)
	{
		for (size_t j = 0; j < cols; j++)
		{
			output += std::to_string(matrix[i][j].output);
			std::get<2>(transitionColumn).push_back(matrix[i][j]);
		}

		if (outputGroup.find(output) == outputGroup.end())
		{
			groupIndex = ++nextGroupIndex;
			outputGroup[output] = nextGroupIndex;
		}
		else
		{
			groupIndex = outputGroup[output];
		}

		std::get<0>(transitionColumn) = groupIndex;
		std::get<1>(transitionColumn) = static_cast<int>(i);

		output = "";
		groupsTable.push_back(std::move(transitionColumn));
	}

	std::ranges::sort(groupsTable, [](const auto& left, const auto& right) {
		return std::get<0>(left) < std::get<0>(right);
	});

	return groupsTable;
}

GroupTransitionTable StepOne(const MachineMatrix& matrix,
	const GroupTransitionTable& previousGroups, int rows, int cols)
{
	GroupTransitionTable groupsTable;
	GroupTransitionColumn groupColumn;
	std::unordered_map<std::string, int> transitionGroup;
	std::string groups;
	int nextGroupIndex = 0;
	int groupIndex = 0;
	int state = -1;
	int newState = -1;
	int formerGroup = -1;
	int newGroup = -1;

	for (size_t i = 0; i < rows; i++)
	{
		state = static_cast<int>(std::get<1>(previousGroups[i]));
		formerGroup = std::get<0>(previousGroups[i]);
		groups += std::to_string(formerGroup);

		std::get<1>(groupColumn) = state;

		for (size_t j = 0; j < cols; j++)
		{
			newState = std::get<2>(previousGroups[i])[j].state;

			auto it = std::ranges::find_if(previousGroups, [newState](const auto& column) {
				return std::get<1>(column) == newState;
			});

			newGroup = std::get<0>(*it);
			groups += std::to_string(newGroup);

			std::get<2>(groupColumn).push_back(matrix[state][j]);
		}

		if (transitionGroup.find(groups) == transitionGroup.end())
		{
			groupIndex = ++nextGroupIndex;
			transitionGroup[groups] = nextGroupIndex;
		}
		else
		{
			groupIndex = transitionGroup[groups];
		}

		groups = "";

		std::get<0>(groupColumn) = groupIndex;
		groupsTable.push_back(std::move(groupColumn));
	}

	std::ranges::sort(groupsTable, [](const auto& left, const auto& right) {
		return std::get<0>(left) < std::get<0>(right);
	});

	return groupsTable;
}
#pragma warning(default : 26800)

MachineMatrix CreateMachineFromGroupTable(const MachineMatrix& originalMatrix,
	const GroupTransitionTable& groupTable, int rows, int cols)
{
	MachineMatrix matrix;
	int state = -1;
	int group = -1;
	int currentGroup = -1;
	int currentState = -1;
	int previousGroup = -1;

	for (size_t i = 0; i < groupTable.size(); i++)
	{
		currentGroup = std::get<0>(groupTable[i]);
		currentState = std::get<1>(groupTable[i]);

		if (currentGroup != previousGroup)
		{
			matrix.emplace_back();

			for (size_t j = 0; j < cols; j++)
			{
				state = originalMatrix[currentState][j].state;

				auto it = std::ranges::find_if(groupTable, [&originalMatrix, state](const auto& column) {
					return std::get<1>(column) == state;
				});
				group = std::get<0>(*it);

				matrix[matrix.size() - 1].emplace_back(group - 1, originalMatrix[currentState][j].output);
			}

			previousGroup = currentGroup;
		}
	}

	return matrix;
}

void WriteMachineMatrixToStream(const MachineMatrix& matrix,
	int rows, int cols, std::ostream& os)
{
	for (size_t i = 0; i < rows; i++)
	{
		for (size_t j = 0; j < cols; j++)
		{
			if (matrix[i][j].state != rows)
			{
				os << matrix[i][j].state << " " << matrix[i][j].output;
			}
			else
			{
				os << "-";
			}

			os << " ";
		}

		os << std::endl;
	}
}
#pragma endregion Implementations