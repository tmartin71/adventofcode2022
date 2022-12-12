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
        m_matrix.reserve(numRows);
    }

    ~TreeMatrix() = default;

    void InsertRow(const std::string& row)
    {
        std::vector<uint32_t> values;

        const auto numCols = row.size();
        values.reserve(numCols);
        for (auto columnNumber = 0; columnNumber < numCols; ++columnNumber)
        {
            values.push_back(std::stoi(row.substr(columnNumber, 1)));
        }

        m_matrix.push_back(values);
    }

    const std::vector<std::vector<uint32_t> >& GetMatrix() const
    {
        return m_matrix;
    }

private:
    std::vector<std::vector<uint32_t> > m_matrix;
};

bool CheckVisibleAlongRow(
    const std::vector<std::vector<uint32_t> >& matrix,
    const uint32_t treeRow,
    const uint32_t treeCol,
    const uint32_t colStart,
    const uint32_t colEnd,
    const uint32_t treeHeight)
{
    for (auto i = colStart; i < colEnd; ++i)
    {
        if (i == treeCol)
            continue;

        if (matrix[treeRow][i] >= treeHeight)
            return false;
    }

    return true;
}

bool CheckVisibleAlongColumn(
    const std::vector<std::vector<uint32_t> >& matrix,
    const uint32_t treeRow,
    const uint32_t treeCol,
    const uint32_t rowStart,
    const uint32_t rowEnd,
    const uint32_t treeHeight)
{
    for (auto i = rowStart; i < rowEnd; ++i)
    {
        if (i == treeRow)
            continue;

        if (matrix[i][treeCol] >= treeHeight)
            return false;
    }

    return true;
}

bool CheckVisible(
    const std::vector<std::vector<uint32_t> >& matrix, const uint32_t row, uint32_t col)
{
    const auto treeHeight = matrix[row][col];
    const auto numRows = matrix.size();
    assert(numRows > 0);

    const auto numCols = matrix[0].size();

    // TODO: note these conditions should not be hit while looping over interior nodes.
    if (row == 0 || row == numRows - 1)
        return true;

    if (col == 0 || col == numCols - 1)
        return true;

    // Check up
    if (CheckVisibleAlongColumn(matrix, row, col, 0, row, treeHeight))
        return true;

    // Check down
    if (CheckVisibleAlongColumn(matrix, row, col, row, numRows, treeHeight))
        return true;

    // Check left
    if (CheckVisibleAlongRow(matrix, row, col, 0, col, treeHeight))
        return true;

    // Check right
    if (CheckVisibleAlongRow(matrix, row, col, col, numCols, treeHeight))
        return true;

    return false;
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

    const auto& m = matrix.GetMatrix();
    
    uint32_t totalPart1 = 0;

    for (auto i = 0; i < numRows; ++i)
    {
        for (auto j = 0; j < numCols; ++j)
        {
            if (CheckVisible(m, i, j))
            {
                totalPart1++;
            }
            else
            {
                std::cout << i << "," << j << std::endl;
            }
        }
    }

    std::cout << totalPart1 << std::endl;
}

int main()
{
    AdventOfCodeExercise8();
}
