#include <iostream>
#include <sstream>
#include <fstream>

#include <string>
#include <vector>
#include <set>
#include <assert.h>
#include <math.h>

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

enum Direction
{
    Up,
    Down,
    Left,
    Right
};

int sign(const int i)
{
    if (i > 0)
        return 1;
    else if (i < 0)
        return -1;
    else
        return 0;
}

class CoordinatePair
{
public:
    CoordinatePair(const int x = 0, const int y = 0)
        : m_x(x)
        , m_y(y)
    {}

    CoordinatePair(const CoordinatePair& rhs)
    {
        m_x = rhs.m_x;
        m_y = rhs.m_y;
    }

    bool operator==(const CoordinatePair& rhs)
    {
        return m_x == rhs.m_x && m_y == rhs.m_y;
    }

    void operator=(const CoordinatePair& rhs)
    {
        m_x = rhs.GetX();
        m_y = rhs.GetY();
    }

    const int GetX() const
    {
        return m_x;
    }

    const int GetY() const
    {
        return m_y;
    }

    std::pair<int, int> AsPair() const
    {
        return std::make_pair(m_x, m_y);
    }

    void MoveTo(const int x, const int y)
    {
        m_x = x;
        m_y = y;
    }

    void Normalize()
    {
        m_x = sign(m_x);
        m_y = sign(m_y);
    }

private:
    int m_x;
    int m_y;
};

class Rope
{
public:
    Rope(const uint32_t numKnots)
    {
        assert(numKnots >= 2);

        m_knots.reserve(numKnots);
        for (auto i = 0; i < numKnots; ++i)
            m_knots.push_back(CoordinatePair());

        RecordTailPosition(*GetTailItr());
    }

    ~Rope() = default;

    void Move(const Direction direction, const uint32_t steps)
    {
        for (auto i = 0; i < steps; ++i)
        {
            auto previousKnotItr = m_knots.begin();
            auto currentKnotItr = m_knots.begin();
            auto nextKnotItr = m_knots.begin() + 1;
            while (nextKnotItr != m_knots.end())
            {
                bool isFirstKnotHead = currentKnotItr == m_knots.begin();
                bool isSecondKnotTail = nextKnotItr == GetTailItr();

                MoveFirstKnotRelativeToPreviousKnot(direction, *previousKnotItr, isFirstKnotHead, *currentKnotItr);
                MoveSecondRelativeToFirst(direction, *currentKnotItr, isSecondKnotTail, *nextKnotItr);

                previousKnotItr = currentKnotItr;
                currentKnotItr++;
                nextKnotItr++;
            }         
        }
    }

    const std::set<std::pair<int, int> >& GetVisited()
    {
        return m_visited;
    }

private:
    const std::vector<CoordinatePair>::iterator GetTailItr()
    {
        return m_knots.end() - 1;
    }

    static const CoordinatePair ComputeVectorBetweenTwoKnots(const CoordinatePair& firstKnot, const CoordinatePair& secondKnot)
    {
        return CoordinatePair(firstKnot.GetX() - secondKnot.GetX(), firstKnot.GetY() - secondKnot.GetY());
    }

    static bool TwoKnotsAreTouching(const CoordinatePair& vectorBetweenHeadAndTail)
    {
        return abs(vectorBetweenHeadAndTail.GetX()) <= 1 && abs(vectorBetweenHeadAndTail.GetY()) <= 1;
    }

    void RecordTailPosition(const CoordinatePair& tail)
    {
        m_visited.insert(tail.AsPair());
    }

    void MoveFirstKnotRelativeToPreviousKnot(
        const Direction direction,
        const CoordinatePair& previousKnot,
        bool isHead,
        CoordinatePair& currentKnot)
    {
        if (!isHead 
            && TwoKnotsAreTouching(ComputeVectorBetweenTwoKnots(previousKnot, currentKnot)))
        {
            return;
        }

        const auto currentPositionX = currentKnot.GetX();
        const auto currentPositionY = currentKnot.GetY();

        switch (direction)
        {
        case Up:
        {
            currentKnot = CoordinatePair(currentPositionX, currentPositionY + 1);
            break;
        }
        case Down:
        {
            currentKnot = CoordinatePair(currentPositionX, currentPositionY - 1);
            break;
        }
        case Left:
        {
            currentKnot = CoordinatePair(currentPositionX - 1, currentPositionY);
            break;
        }
        case Right:
        {
            currentKnot = CoordinatePair(currentPositionX + 1, currentPositionY);
            break;
        }
        default:
        {
            assert(false);
            break;
        }
        }
    }

    void MoveSecondRelativeToFirst(
        const Direction direction,
        const CoordinatePair& currentKnot,
        const bool isTail,
        CoordinatePair& nextKnot)
    {
        auto v = ComputeVectorBetweenTwoKnots(currentKnot, nextKnot);
        if (TwoKnotsAreTouching(v))
            return;

        // Normalize so that we only move one space in any direction
        v.Normalize();

        nextKnot = CoordinatePair(nextKnot.GetX() + v.GetX(), nextKnot.GetY() + v.GetY());

        if (isTail)
            RecordTailPosition(nextKnot);
    }

    std::vector<CoordinatePair> m_knots;
    std::set<std::pair<int, int> > m_visited;
};

const Direction StringToDirection(const std::string& s)
{
    Direction d;
        if (s == "D")
            d = Direction::Down;
        else if (s == "U")
            d = Direction::Up;
        else if (s == "L")
            d = Direction::Left;
        else if (s == "R")
            d = Direction::Right;
        else
            assert(false);

    return d;
}

void AdventOfCodeExercise9()
{
    const auto lines = ReadTextFile("input_exercise_9.txt");

    Rope rPart1(2);
    Rope rPart2(10);
    std::set<std::pair<int,int> > visited;
    for (const auto& line : lines)
    {
        const auto strings = Split(line, ' ');
        const auto direction = StringToDirection(strings[0]);
        const auto steps = std::stoi(strings[1]);
        rPart1.Move(direction, steps);
        rPart2.Move(direction, steps);
    }
    
    std::cout << rPart1.GetVisited().size() << std::endl;
    std::cout << rPart2.GetVisited().size() << std::endl;
}

int main()
{
    AdventOfCodeExercise9();
}