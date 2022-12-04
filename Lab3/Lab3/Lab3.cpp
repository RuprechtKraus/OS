#include <fstream>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <vector>

struct Row
{
	Row() = default;

	Row(size_t size)
	{
		content.resize(size);
	}

	int shortName = -1;
	std::vector<int> fullName;
	std::vector<std::vector<int>> content;
};

using Table = std::vector<Row>;

std::tuple<int, int, Table> Read(std::istream& input);
std::vector<int> Split(const std::string& str, char delim);

std::map<int, std::vector<int>> CreateEClosures(const Table& table);
std::vector<int> EClose(const Table& table, int state);

int main()
{
	std::ifstream input("input.txt");

	if (!input.is_open())
	{
		std::cerr << "Unable to open input file" << std::endl;
		return 1;
	}

	auto [countState, countSymbol, baseTable] = Read(input);
	auto eClosures = CreateEClosures(baseTable);
	std::queue<std::vector<int>> q;
	q.push(eClosures[0]);
	std::vector<int> nextStates;
	std::vector<int> cellStates;
	std::set<std::vector<int>> visited;
	int index = 0;

	Table newTable;

	while (!q.empty())
	{
		nextStates = q.front();
		q.pop();

		if (visited.contains(nextStates))
		{
			continue;
		}

		newTable.emplace_back(countSymbol);
		newTable.back().fullName = nextStates;
		newTable.back().shortName = index++;

		for (size_t j = 0; j < countSymbol; j++)
		{
			std::set<int> v;

			for (int s : nextStates)
			{
				cellStates = baseTable[s].content[j];

				for (int ss : cellStates)
				{
					std::vector<int> g = eClosures[ss];
					std::ranges::copy(g, std::inserter(v, v.end()));
				}
			}

			std::copy(v.begin(), v.end(), std::back_inserter(newTable.back().content[j]));

			if (!newTable.back().content[j].empty())
			{
				q.push(newTable.back().content[j]);
			}
		}

		visited.insert(nextStates);
	}

	for (size_t i = 0; i < newTable.size(); i++)
	{
		for (size_t j = 0; j < newTable[i].content.size(); j++)
		{
			auto& cell = newTable[i].content[j];
			auto it = std::ranges::find_if(newTable, [&cell](const auto& row) {
				return row.fullName == cell;
			});

			if (it != newTable.end())
			{
				std::cout << it->shortName << " "; 
			}
			else
			{
				std::cout << "- ";
			}
		}

		std::cout << std::endl;
	}
}

std::tuple<int, int, Table> Read(std::istream& input)
{
	int countState, countSymbol;
	input >> countState >> countSymbol;

	Table table;
	table.resize(static_cast<size_t>(countState));

	for (auto& row : table)
	{
		row.content.resize(static_cast<size_t>(countSymbol) + 1);
	}

	std::string token;

	for (int i = 0; i < countState; i++)
	{
		for (int j = 0; j < countSymbol + 1; j++)
		{
			input >> token;
			table[i].shortName = i;

			if (token != "-")
			{
				table[i].content[j] = Split(token, ',');
			}
		}
	}

	return { countState, countSymbol, table };
}

std::vector<int> Split(const std::string& str, char delim)
{
	std::stringstream ss(str);
	std::string item;
	std::vector<int> elems;

	while (std::getline(ss, item, delim))
	{
		elems.push_back(stoi(item));
	}

	return elems;
}

std::map<int, std::vector<int>> CreateEClosures(const Table& table)
{
	std::map<int, std::vector<int>> result;
	for (size_t i = 0; i < table.size(); i++)
	{
		result[i] = EClose(table, i);
	}
	return result;
}

std::vector<int> EClose(const Table& table, int state)
{
	std::vector<int> result;
	std::set<int> visited;
	std::queue<int> q;
	std::vector<int> nextStates;
	int currState;
	q.push(state);

	while (!q.empty())
	{
		currState = q.front();
		q.pop();

		if (!visited.contains(currState))
		{
			result.push_back(currState);
			visited.insert(currState);

			nextStates = table[currState].content.back();
			for (auto& i : nextStates)
			{
				q.push(i);
			}
			nextStates.clear();
		}
	}

	std::ranges::sort(result);
	return result;
}