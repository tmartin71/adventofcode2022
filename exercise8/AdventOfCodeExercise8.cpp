#include <iostream>
#include <sstream>
#include <fstream>

#include <string>
#include <vector>
#include <assert.h>

std::vector<std::string> ReadTextFile(std::string inputFilename)
{
    std::ifstream myfile;
    std::string line;

    std::vector<std::string> output;
    myfile.open(inputFilename);
    if (myfile.is_open())
    {
        while (getline(myfile,line))
        {
            output.push_back(std::string(line));
        }

        myfile.close();
    }
    return output;
}

class TreeMatrix
{
public:
    TreeMatrix(const uint32_t numRows)
    {
        m_rows.reserve(numRows);
    }

    ~TreeMatrix() = default;

    void InsertRow(const std::string& row)
    {
        std::vector<uint32_t> values;

        const auto numCols = row.size();
        m_cols.reserve(numCols);
        values.reserve(numCols);
        for (auto columnNumber = 0; columnNumber < numCols; ++columnNumber)
        {
            const uint32_t v = std::stoi(row.substr(columnNumber, 1));
            values.push_back(v);
            if (m_cols[columnNumber].empty())
            {
                std::vector<uint32_t> newCol;
                newCol.push_back(v);
                m_cols.push_back(newCol);
            }
            else
            {
                m_cols[columnNumber].push_back(v);
            }
        }

        m_rows.push_back(values);
    }

    const std::vector<uint32_t>& GetColumn(const uint32_t colIndex) const
    {
        return m_cols[colIndex];
    }

    const std::vector<uint32_t>& GetRow(const uint32_t rowIndex) const
    {
        return m_rows[rowIndex];
    }

    const uint32_t& GetTreeHeight(const uint32_t row, const uint32_t col) const
    {
        return m_rows[row][col];
    }

private:
    std::vector<std::vector<uint32_t> > m_rows;
    std::vector<std::vector<uint32_t> > m_cols;
};

std::pair<bool, uint32_t> CheckVisibleAlongRowOrColumn(
    const std::vector<uint32_t>& rowOrColumn,
    const uint32_t treeRowOrColumn,
    const uint32_t treeHeight)
{
    bool isVisibleFromAbove = true;
    uint32_t scenicScoreAbove = 0;
    const auto numRowsOrCols = rowOrColumn.size();

    // Check above
    // Note we must use int here. Not doing so results in the loop always bailing immediately
    // because unsigned 0 minus 1 is a large positive number
    for (int i = treeRowOrColumn; i > -1; --i)
    {
        if (i == treeRowOrColumn)
            continue;

        scenicScoreAbove++;
        if (rowOrColumn[i] >= treeHeight)
        {
            isVisibleFromAbove = false;
            break;
        }
    }

    // Reset isVisible when searching in the other direction
    bool isVisibleFromBelow = true;
    uint32_t scenicScoreBelow = 0;
    for (int i = treeRowOrColumn; i < numRowsOrCols; ++i)
    {
        if (i == treeRowOrColumn)
            continue;

        ++scenicScoreBelow;

        if (rowOrColumn[i] >= treeHeight)
        {
            isVisibleFromBelow = false;
            break;
        }
    }

    return std::make_pair(isVisibleFromAbove || isVisibleFromBelow, scenicScoreAbove * scenicScoreBelow);
}

std::pair<bool, uint32_t> CheckVisible(
    const TreeMatrix& matrix, const uint32_t row, uint32_t col)
{
    const auto treeHeight = matrix.GetTreeHeight(row, col);

    // Check up and down
    const auto column = matrix.GetColumn(col);
    const auto visibleAndScenicScoreAlongColumn = CheckVisibleAlongRowOrColumn(column, row, treeHeight);

    // Check left and right
    const auto visibleAndScenicScoreAlongRow = CheckVisibleAlongRowOrColumn(matrix.GetRow(row), col, treeHeight);
    const auto scenicScoreTotal = visibleAndScenicScoreAlongColumn.second * visibleAndScenicScoreAlongRow.second;
    return std::make_pair(
        visibleAndScenicScoreAlongColumn.first || visibleAndScenicScoreAlongRow.first, scenicScoreTotal);
}

void AdventOfCodeExercise8()
{
    const auto lines = ReadTextFile("input_exercise_8.txt");
    const auto numRows = lines.size();
    assert(lines.size() > 0);

    const auto numCols = lines[0].size();
    TreeMatrix matrix(numRows);
    for (auto i = 0; i < lines.size(); ++i)
    {
        matrix.InsertRow(lines[i]);
    }
    
    uint32_t totalPart1 = 0;
    uint32_t totalPart2 = 0;
    for (auto i = 0; i < numRows; ++i)
    {
        for (auto j = 0; j < numCols; ++j)
        {
            const auto visibleAndScenicScore = CheckVisible(matrix, i, j);
            if (visibleAndScenicScore.first)
            {
                totalPart1++;
            }

            totalPart2 = std::max(totalPart2, visibleAndScenicScore.second);
        }
    }

    std::cout << totalPart1 << std::endl;
    std::cout << totalPart2 << std::endl;
}

int main()
{
    AdventOfCodeExercise8();
}
