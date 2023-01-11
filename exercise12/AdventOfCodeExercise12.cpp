#include <iostream>
#include <sstream>
#include <fstream>

#include <string>
#include <vector>
#include <set>
#include <unordered_map>
#include <utility>
#include <math.h>

std::vector<std::string> ReadTextFile(std::string inputFilename)
{
    std::ifstream myfile;
    std::string line;

    std::vector<std::string> output;
    myfile.open(inputFilename);
    if (myfile.is_open())
    {
        while (getline(myfile, line))
        {
            output.push_back(std::string(line));
        }

        myfile.close();
    }
    return output;
}

struct pair_hash
{
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2> &p) const
    {
        auto h1 = std::hash<T1>()(p.first);
        auto h2 = std::hash<T2>()(p.second);

        // Mainly for demonstration purposes, i.e. works but is overly simple
        // In the real world, use sth. like boost.hash_combine
        return h1 ^ h2;
    }
};

const uint32_t AlphaToHeight(const char &s)
{
    const auto u = std::toupper(s);
    return static_cast<int>(u - 'A' + 1);
}

class Maze
{
public:
    Maze(const uint32_t numRows)
    {
        m_rows.reserve(numRows);
    }

    ~Maze() = default;

    void InsertRow(const std::string &row, const uint32_t rowNumber)
    {
        std::vector<uint32_t> values;

        const auto numCols = row.size();
        m_size.first++;
        if (numCols == 0)
            m_size.first = numCols;

        m_size.second = numCols;
        values.reserve(numCols);
        for (auto columnNumber = 0; columnNumber < numCols; ++columnNumber)
        {
            uint32_t v = 0;
            if (row[columnNumber] == 'S' || row[columnNumber] == 'a')
            {
                m_startCandidates.push_back(std::make_pair(rowNumber, columnNumber));

                // Start position has height 'a' (1)
                values.push_back(1);
                continue;
            }
            else if (row[columnNumber] == 'E')
            {
                // End position has height 'z' (26)
                m_end = std::make_pair(rowNumber, columnNumber);
                values.push_back(26);
                continue;
            }

            v = AlphaToHeight(row[columnNumber]);
            values.push_back(v);
        }

        m_rows.push_back(values);
    }

    const std::vector<uint32_t> &GetRow(const uint32_t rowIndex) const
    {
        return m_rows[rowIndex];
    }

    const uint32_t &operator()(const uint32_t row, const uint32_t col) const
    {
        return m_rows[row][col];
    }

    const std::vector<std::pair<uint32_t, uint32_t>> GetStartCandidates() const
    {
        return m_startCandidates;
    }

    const std::pair<uint32_t, uint32_t> GetEnd() const
    {
        return m_end;
    }

    const std::vector<std::pair<uint32_t, uint32_t>> GetNeighbors(
        const std::pair<uint32_t, uint32_t> node) const
    {
        const auto numRows = m_size.first;
        const auto numCols = m_size.second;
        std::vector<std::pair<uint32_t, uint32_t>> points;
        const auto currentHeight = (*this)(node.first, node.second);

        for (int i = -1; i < 2; ++i)
        {
            for (int j = -1; j < 2; ++j)
            {
                const int rowIdx = node.first + i;
                const int colIdx = node.second + j;

                if (rowIdx == node.first && colIdx == node.second)
                    continue;

                if (rowIdx < 0 || rowIdx > numRows - 1 || colIdx < 0 || colIdx > numCols - 1)
                    continue;

                // We can't move along the diagonal
                if (abs(i) == 1 && abs(j) == 1)
                    continue;

                // Only consider neighboring nodes that are at most 1 higher then the current node
                if (static_cast<int>((*this)(rowIdx, colIdx)) - static_cast<int>(currentHeight) > 1)
                    continue;

                points.push_back(std::make_pair(rowIdx, colIdx));
            }
        }

        return points;
    }

private:
    std::vector<std::vector<uint32_t>> m_rows;
    std::vector<std::pair<uint32_t, uint32_t>> m_startCandidates;
    std::pair<uint32_t, uint32_t> m_end;
    std::pair<uint32_t, uint32_t> m_size;
};

const std::pair<uint32_t, uint32_t> FindMinDistanceNode(
    const std::set<std::pair<uint32_t, uint32_t>> &nodesToExplore,
    const std::unordered_map<std::pair<uint32_t, uint32_t>, uint32_t, pair_hash> &distances)
{
    std::pair<uint32_t, uint32_t> minNode;
    uint32_t minDist = INT32_MAX;
    for (const auto &node : nodesToExplore)
    {
        const auto d = distances.find(node);
        if (d != distances.end() && d->second < minDist)
        {
            minDist = d->second;
            minNode = node;
        }
    }

    return minNode;
}

std::unordered_map<std::pair<uint32_t, uint32_t>, uint32_t, pair_hash> BFSMaze(
    const Maze &maze, const std::pair<uint32_t, uint32_t> &start)
{
    std::set<std::pair<uint32_t, uint32_t>> visited;
    std::set<std::pair<uint32_t, uint32_t>> nodesToExplore;
    std::unordered_map<std::pair<uint32_t, uint32_t>, uint32_t, pair_hash> distances;

    distances[start] = 0;

    nodesToExplore.insert(start);
    while (!nodesToExplore.empty())
    {
        const auto node = FindMinDistanceNode(nodesToExplore, distances);
        nodesToExplore.erase(node);

        if (distances.find(node) == distances.end())
            distances[node] = INT32_MAX;

        visited.insert(node);
        if (node == maze.GetEnd())
            return distances;

        const auto neighbors = maze.GetNeighbors(node);
        for (const auto &n : neighbors)
        {
            if (visited.count(n) > 0)
                continue;

            if (distances.find(n) == distances.end())
                distances[n] = INT32_MAX;

            const uint32_t weightOfNextEdge = 1;
            if (distances[node] != INT32_MAX && distances[node] + weightOfNextEdge < distances[n])
                distances[n] = distances[node] + weightOfNextEdge;

            nodesToExplore.insert(n);
        }
    }

    return distances;
}

void AdventOfCodeExercise12()
{
    const auto lines = ReadTextFile("input_exercise_12.txt");
    const auto numRows = lines.size();
    assert(lines.size() > 0);

    const auto numCols = lines[0].size();
    Maze maze(numRows);
    for (auto i = 0; i < lines.size(); ++i)
    {
        maze.InsertRow(lines[i], i);
    }

    uint32_t minDistance = INT32_MAX;
    for (const auto &start : maze.GetStartCandidates())
    {
        const auto distances = BFSMaze(maze, start);
        if (distances.find(maze.GetEnd()) != distances.end())
            minDistance = std::min(minDistance, distances.at(maze.GetEnd()));
    }

    std::cout << minDistance << std::endl;
}

int main()
{
    AdventOfCodeExercise12();
}