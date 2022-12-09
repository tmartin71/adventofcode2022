#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <set>
#include "../AdventOfCodeUtils.h"

uint32_t FindPositionOfPacketMarker(const std::string& line, const uint32_t sequenceLength)
{
    for (auto i = 0; i < line.length(); ++i)
    {
        std::set<char> charSequence;
        for (auto j = i; j < i + sequenceLength; ++j)
        {
            charSequence.insert(line[j]);
        }

        if (charSequence.size() == sequenceLength)
        {
            return i + sequenceLength;
        }
    }
}

void Exercise6()
{
    const auto inputLine = AdventOfCodeUtils::ReadTextFile("input_exercise_6.txt")[0];

    const uint32_t sequenceLengthPart1 = 4;
    const uint32_t sequenceLengthPart2 = 14;
    const auto positionPart1 = FindPositionOfPacketMarker(inputLine, sequenceLengthPart1);
    const auto positionPart2 = FindPositionOfPacketMarker(inputLine, sequenceLengthPart2);

    std::cout << "Part 1:" << std::endl;
    std::cout << positionPart1 << std::endl;
    std::cout << "Part 2:" << std::endl;
    std::cout << positionPart2 << std::endl;
}

int main()
{
    Exercise6();
}