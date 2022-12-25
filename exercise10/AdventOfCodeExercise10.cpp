#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <stdint.h>

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

std::vector<std::string> Split(const std::string& input, const char& commandDelimiter)
{
    std::vector<std::string> strings;
    std::istringstream f(input);
    std::string s;
    while(std::getline(f, s, commandDelimiter))
    {
        strings.push_back(s);
    }

    return strings;
}

class Instruction
{
public:
    Instruction() = default;
    ~Instruction() = default;

    virtual bool IsNoop() const
    {
        return false;
    }

    virtual bool IsAddX() const
    {
        return false;
    }

    virtual int GetInput() const
    {
        return m_value;
    }
private:
    int m_value;
};

class Noop : public Instruction
{
public:
    Noop()
        : Instruction(){}
    ~Noop() = default;

    bool IsNoop() const override
    {
        return true;
    }
};

class AddX : public Instruction
{
public:
    AddX(const int value)
        : m_value(value) {}
    ~AddX() = default;

    bool IsAddX() const override
    {
        return true;
    }

    const int GetInput()
    {
        return m_value;
    }

private:
    const int m_value;
};

class InstructionProcessor
{
public:
    InstructionProcessor()
        : m_registerValue(1)
        , m_currentInstruction(nullptr)
    {
    }
    ~InstructionProcessor() = default;

    std::shared_ptr<Instruction> ProcessInstruction(const std::string& instruction)
    {
        const auto strings = Split(instruction, ' ');
        const auto& instructionType = strings[0];

        if (instructionType == "noop")
        {
            m_currentInstruction = std::make_shared<Noop>();
        }
        else if (instructionType == "addx")
        {
            assert(strings.size() == 2);
            const auto input = std::stoi(strings[1]);
            m_currentInstruction = std::make_shared<AddX>(input);
        }

        return m_currentInstruction;
    }

    void ExecuteCurrentInstruction()
    {
        if (m_currentInstruction->IsAddX())
        {
            auto asAddX = std::dynamic_pointer_cast<AddX>(m_currentInstruction);
            const auto v = asAddX->GetInput();
            m_registerValue += asAddX->GetInput();
        }
    }

    int GetCurrentRegisterValue() const
    {
        return m_registerValue;
    }

private:
    int m_registerValue;
    std::shared_ptr<Instruction> m_currentInstruction;
};

// TODO: looks like the last line of pixels isn't quite right, but I
// can still read the answer for 2.
void AdventOfCodeExercise10()
{
    const auto lines = ReadTextFile("input_exercise_10.txt");
    int interestingCyclesCurrent = 20;
    const int interestingCyclesIncrement = 40;
    const int interestingCyclesEnd = 220;

    const auto lightPixel = "#";
    const auto darkPixel = ".";
    const auto spriteLength = 3;
    std::unordered_map<int, int> signalStrengths;

    InstructionProcessor processor;
    uint32_t cycle = 0;
    auto lineItr = lines.begin();
    uint32_t instructionCycleCount = 0;

    while (true)
    {
        // TODO: this can be made more efficient, but it's probably not trivial
        const auto& instructionType = processor.ProcessInstruction(*lineItr);
        if (instructionType->IsNoop())
        {
            lineItr++;
        }
        else if (instructionType->IsAddX())
        {
            if (instructionCycleCount == 2)
            {   
                processor.ExecuteCurrentInstruction();
                instructionCycleCount = 0;
                lineItr++;
            }

            instructionCycleCount++;
        }

        const auto currentRegisterValue = processor.GetCurrentRegisterValue();
        std::vector<int> spritePosition(3);
        spritePosition.push_back(currentRegisterValue);
        spritePosition.push_back(currentRegisterValue + 1);
        spritePosition.push_back(currentRegisterValue + 2);
    
        cycle++;
        const auto normalizedCycleNumber = cycle % 40;
        auto stringToUse =
            std::find(spritePosition.begin(), spritePosition.end(), normalizedCycleNumber) != spritePosition.end()
                ? lightPixel
                : darkPixel;
        
        std::cout << stringToUse;     

        if (normalizedCycleNumber == 0)
            std::cout << std::endl;


        if (cycle == interestingCyclesCurrent)
        {
            signalStrengths[cycle] = cycle * processor.GetCurrentRegisterValue();
            interestingCyclesCurrent += interestingCyclesIncrement;
        }

        if (lineItr == lines.end())
        {
            break;
        }
    }

    int totalPart1 = 0;
    
    std::cout << std::endl;
    for (const auto& signal : signalStrengths)
    {
        totalPart1 += signal.second;
    }

    std::cout << totalPart1 << std::endl;
}

int main()
{
    AdventOfCodeExercise10();
}