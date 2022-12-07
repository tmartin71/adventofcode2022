#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <stack>
#include <fstream>

std::vector<std::string> ReadTextFile(std::string input_filename)
{
    std::ifstream myfile;
    std::string line;

    std::vector<std::string> output;
    myfile.open(input_filename);
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

const uint32_t crateStringLength = 4;

void InsertRowIntoCrateStacks(const std::string& row, std::vector<std::stack<char> >& stacks)
{
    const uint32_t rowLength = row.length() + 1;
    for (auto i = 0; i < rowLength; ++i)
    {
        auto stackIndex = i / crateStringLength;
        if (row[i] == '[')
        {
            stacks[stackIndex].push(row[i+1]);
        }
    }
}

std::vector<std::stack<char> > BuildCrateStacksFromRows(const std::vector<std::string>& rows)
{
    const auto rowLength = rows[0].length() + 1;
    const auto numStacks = rowLength / crateStringLength;
    
    std::vector<std::vector<char> > cratesAsVectors(numStacks);
    for (const auto& row : rows)
    {
        for (auto i = 0; i < rowLength; ++i)
        {
            auto stackIndex = i / crateStringLength;
            if (row[i] == '[')
            {
                cratesAsVectors[stackIndex].push_back(row[i+1]);
            }
        }
    }

    std::vector<std::stack<char> > crateStacks(rowLength / crateStringLength);
    auto crateIndex = 0;
    for (auto v : cratesAsVectors)
    {
        std::reverse(v.begin(), v.end());
        for (auto crate : v)
        {
            crateStacks[crateIndex].push(crate);
        }
        crateIndex++;
    }

    return crateStacks;
}

struct CrateMoveInstruction
{
    uint32_t from;
    uint32_t to;
    uint32_t quantity;
};

const CrateMoveInstruction BuildInstructionFromRow(const std::string& row)
{
    std::vector<std::string> strings;
    std::istringstream f(row);
    std::string s;
    while(std::getline(f, s, ' '))
    {
        strings.push_back(s);
    }

    CrateMoveInstruction instruction;
    for (auto i = 0; i < strings.size(); ++i)
    {
        if (strings[i] == "move")
            instruction.quantity = std::stoi(strings[i+1]);
        else if (strings[i] == "from")
            instruction.from = std::stoi(strings[i+1]) - 1;
        else if (strings[i] == "to")
            instruction.to = std::stoi(strings[i+1]) - 1;
    }
    
    return instruction;
}

void CrateMover9000(const CrateMoveInstruction& instruction, std::vector<std::stack<char> >& crateStacks)
{
    for (auto i = 0; i < instruction.quantity; ++i)
    {
        crateStacks[instruction.to].push(crateStacks[instruction.from].top());
        crateStacks[instruction.from].pop();
    }
}

void CrateMover9001(const CrateMoveInstruction& instruction, std::vector<std::stack<char> >& crateStacks)
{
    std::vector<char> temp;
    for (auto i = 0; i < instruction.quantity; ++i)
    {
        temp.push_back(crateStacks[instruction.from].top());
        crateStacks[instruction.from].pop();
    }

    std::reverse(temp.begin(), temp.end());
    for (auto crate : temp)
    {
        crateStacks[instruction.to].push(crate);
    }
}

void Exercise5()
{
    std::string filename("input_exercise_5.txt");
    const auto inputRows = ReadTextFile(filename);
    
    std::vector<std::stack<char> > crateStacksPart1;
    std::vector<std::stack<char> > crateStacksPart2;
    std::vector<std::string> rows;
    bool parsingCrateStacks = true;
    for (const auto& row : inputRows)
    {   
        if (row.empty())
        {
            parsingCrateStacks = false;
            crateStacksPart1 = BuildCrateStacksFromRows(rows);
            crateStacksPart2 = crateStacksPart1;
            continue;
        }
        else if (row.at(0) == ' ')
        {
            // Skip the stack numbers (they start with a space)
            continue;
        }

        // The first "paragraph" of the input are the initial state of the stacks
        if (parsingCrateStacks)
        {
            rows.push_back(row);
        }
        else
        {
            const auto instruction = BuildInstructionFromRow(row);
            CrateMover9000(instruction, crateStacksPart1);
            CrateMover9001(instruction, crateStacksPart2);
        }
    }

    std::cout << "Part 1:" << std::endl;
    for (const auto& stack : crateStacksPart1)
    {
        const auto top = stack.top();
        std::cout << top;
    }

    std::cout << "\n" << std::endl;
    std::cout << "Part 2:" << std::endl;
    for (const auto& stack : crateStacksPart2)
    {
        const auto top = stack.top();
        std::cout << top;
    }
}

int main()
{
    Exercise5();
}