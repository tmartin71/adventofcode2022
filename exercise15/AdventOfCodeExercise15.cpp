
#include <iostream>
#include <sstream>
#include <fstream>

#include <string>
#include <vector>
#include <stack>
#include <math.h>
#include <set>

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

std::string Trim(const std::string& str,
                 const std::string& whitespace = " \t")
{
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
        return ""; // no content

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}


class Range
{
public:
    Range(bool isEmpty, int min = 0, int max = 0)
        : m_min(min)
        , m_max(max)
        , m_isEmpty(isEmpty)
    {}

    ~Range() = default;

    int GetMin() const
    {
        return m_min;
    }

    int GetMax() const
    {
        return m_max;
    }

    void Expand(const Range& input)
    {
        if (input.GetMax() > m_max)
            m_max = input.GetMax();
    }

    int Size() const
    {
        const auto size = m_max - m_min;
        assert(size > 0);
        return size;
    }

    void Clip(int minExtent, int maxExtent)
    {
        if (minExtent > m_min)
            m_min = minExtent;

        if (maxExtent < m_max)
            m_max = maxExtent;

        assert(m_min <= m_max);
    }

    bool IsEmpty() const
    {
        return m_isEmpty;
    }

private:
    int m_min;
    int m_max;
    bool m_isEmpty;
};

class Line
{
public:
    Line(const std::pair<int, int> p, const std::pair<int, int> q)
    {
        double px = static_cast<double>(p.first);
        double py = static_cast<double>(p.second);
        double qx = static_cast<double>(q.first);
        double qy = static_cast<double>(q.second);

        const auto denom = px - qx;
        assert(denom != 0);
        m_slope = (py - qy) / denom;

        m_intercept = -m_slope * px + py;
    }

    ~Line() = default;

    double GetSlope() const
    {
        return m_slope;
    }

    double GetIntercept() const
    {
        return m_intercept;
    }

    bool Intersect(const Line& other, std::pair<int, int>& point /* out */) const
    {
        // Solve the 2x2 system
        const double a = -1 * GetSlope();
        const double b = 1;
        const double c = -1 * other.GetSlope();
        const double d = 1;

        const auto det = a * d - b * c;
        if (det == 0)
            return false;

        // Multiply matrix inverse by the intercept
        point.first = static_cast<int>((d / det) * GetIntercept() - (b / det) * other.GetIntercept());
        point.second = static_cast<int>((-c / det) * GetIntercept() + (a / det) * other.GetIntercept());
        
        return true;
    }

private:
    double m_slope;
    double m_intercept;
};

class TaxicabBall
{
public:
    TaxicabBall(const std::pair<int, int>& center, int radius)
        : m_center(center)
        , m_radius(radius)
    {
        SetLines();
    }

    TaxicabBall(const std::pair<int, int>& sensor, const std::pair<int, int>& beacon)
        : m_center(sensor)
    {
        m_radius = Distance(sensor, beacon);
        SetLines();
    }

    ~TaxicabBall() = default;

    static int Distance(const std::pair<int, int>& p, const std::pair<int, int>& q)
    {
        return abs(p.first - q.first) + abs(p.second - q.second);
    }

    const int GetRadius() const
    {
        return m_radius;
    }

    const std::pair<int, int>& GetCenter() const
    {
        return m_center;
    }

    bool Contains(const std::pair<int, int>& point) const
    {
        return Distance(m_center, point) <= m_radius;
    }

    bool operator==(const TaxicabBall& other) const
    {
        return other.m_center == m_center && other.m_radius == m_radius;
    }

    void SetLines()
    {
        const auto N = std::make_pair(m_center.first, m_center.second - m_radius);
        const auto E = std::make_pair(m_center.first + m_radius, m_center.second);
        const auto S = std::make_pair(m_center.first, m_center.second + m_radius);
        const auto W = std::make_pair(m_center.first - m_radius, m_center.second);

        m_lines.push_back(Line(N, E));
        m_lines.push_back(Line(E, S));
        m_lines.push_back(Line(S, W));
        m_lines.push_back(Line(W, N));
    }

    const std::vector<Line>& GetLines() const
    {
        return m_lines;
    }

private:
    const std::pair<int, int> m_center;
    int m_radius;
    std::vector<Line> m_lines;
};

int ParseLocationExpression(const std::string expr)
{
    const auto exprTokens = Split(expr, '=');
    assert(exprTokens.size() == 2);
    return std::stoi(exprTokens.at(1));
}

const Range BallHorizontalLineIntersection(const TaxicabBall& ball, int y)
{
    Range intersection(true /* isEmpty */);

    const auto& center = ball.GetCenter();
    const auto radius = ball.GetRadius();

    const auto cx = center.first;
    const auto cy = center.second;

    // No intersection
    if (y < cy - radius || y > cy + radius)
    {
        return intersection;
    }

    const auto dy = abs(y - cy);
    const auto dx = radius - dy;
    auto xmin = cx - dx;
    auto xmax = cx + dx;
    assert(xmin <= xmax);
    
    return Range(false, xmin, xmax);
}

const std::vector<TaxicabBall> ParseInput()
{
    const auto lines = ReadTextFile("input_exercise_15.txt");

    std::vector<TaxicabBall> balls;
    std::set<std::pair<int, int> > beacons;
    for (const auto& line : lines)
    {
        // Sensor at x=2, y=18: closest beacon is at x=-2, y=15
        const auto sensorAndBeacon = Split(line, ':');
        assert(sensorAndBeacon.size() == 2);

        const auto sensorTokens = Split(sensorAndBeacon[0], ' ');

        // Sensor at x=2, y=18
        std::pair<int, int> sensor;
        for (const auto& sensorToken : sensorTokens)
        {
            assert(sensorToken.size() > 0);
            if (sensorToken.at(0) == 'x')
            {
                sensor.first = ParseLocationExpression(sensorToken);
            }

            if (sensorToken.at(0) == 'y')
            {
                sensor.second = ParseLocationExpression(sensorToken);
            }
        }

        const auto beaconStr = Trim(sensorAndBeacon[1]);

        // closest beacon is at x=-2, y=15
        const auto beaconTokens = Split(beaconStr, ' ');
        std::pair<int, int> beacon;
        for (const auto& beaconToken : beaconTokens)
        {
            assert(beaconToken.size() > 0);
            if (beaconToken.at(0) == 'x')
            {
                beacon.first = ParseLocationExpression(beaconToken);
            }
            
            if (beaconToken.at(0) == 'y')
            {
                beacon.second = ParseLocationExpression(beaconToken);
            }
        }

        const auto radius = TaxicabBall::Distance(sensor, beacon);
        balls.push_back(TaxicabBall(sensor, radius));
    }

    return balls;
}

bool RangeComparison(const Range& lhs, const Range& rhs)
{
    return lhs.GetMin() < rhs.GetMin();
}

const std::vector<Range> ComputeIntersection(
    const std::vector<TaxicabBall>& balls,
    int y)
{
    std::vector<Range> ballIntersections;
    ballIntersections.reserve(balls.size());

    for (const auto& ball : balls)
    {
        const auto intersection = BallHorizontalLineIntersection(ball, y);
        
        if (!intersection.IsEmpty())
            ballIntersections.push_back(BallHorizontalLineIntersection(ball, y));
    }

    std::sort(ballIntersections.begin(), ballIntersections.end(), RangeComparison);

    std::stack<Range> mergedIntersections;
    for (const auto& range : ballIntersections)
    {
        if (mergedIntersections.empty())
        {
            mergedIntersections.push(range);
            continue;
        }

        if (range.GetMin() > mergedIntersections.top().GetMax())
        {
            mergedIntersections.push(range);
        }
        else
        {
            mergedIntersections.top().Expand(range);
        }
    }

    std::vector<Range> asVector;
    asVector.reserve(mergedIntersections.size());
    
    while (!mergedIntersections.empty())
    {
        asVector.push_back(mergedIntersections.top());
        mergedIntersections.pop();
    }

    return asVector;
}

void AdventOfCodeExercise15_Part1()
{
    const auto balls = ParseInput();

    const int y = 2000000;
    auto intersection = ComputeIntersection(balls, y);
    assert(intersection.size() == 1);

    std::cout << "Part 1:" << std::endl;
    std::cout << intersection.at(0).Size() << std::endl;
}

void AdventOfCodeExercise15_Part2()
{
    const auto balls = ParseInput();

    const auto xMin = 0;
    const auto xMax = 4000000;
    const auto yMin = 0;
    const auto yMax = 4000000;

    int xCoordOfBeacon;
    int yCoordOfBeacon;

    for (auto i = yMin; i < yMax + 1; ++i)
    {
        auto intersection = ComputeIntersection(balls, i);

        for (auto& range : intersection)
        {
            range.Clip(xMin, xMax);
        }

        if (intersection.size() == 2)
        {
            std::sort(intersection.begin(), intersection.end(), RangeComparison);
            xCoordOfBeacon = intersection.at(0).GetMax() + 1;
            yCoordOfBeacon = i;
            std::cout << "(" << xCoordOfBeacon << "," << yCoordOfBeacon << ")" << std::endl;
            break;
        }
    }

    const auto output = static_cast<long int>(xCoordOfBeacon) * 4000000 + static_cast<long int>(yCoordOfBeacon);
    std::cout << output << std::endl;
}

// This approach is super fast, but it currently fails to find the solution point
// Not worth debugging at this point, but I'll keep it around as a reference for later.
void AdventOfCodeExercise15_Part2_LinearAlgebra()
{
    const auto balls = ParseInput();

    const auto xMin = 0;
    const auto xMax = 4000000;
    const auto yMin = 0;
    const auto yMax = 4000000;

    int xCoordOfBeacon;
    int yCoordOfBeacon;

    std::set<std::pair<int, int> > intersectionPoints;
    for (const auto& ballOuter : balls)
    {
        for (const auto& ballInner : balls)
        {
            if (ballOuter == ballInner)
            {
                continue;
            }
            
            for (const auto& lineOuter : ballOuter.GetLines())
            {
                for (const auto& lineInner : ballInner.GetLines())
                {
                    std::pair<int, int> intersection;
                    const auto intersectExists = lineOuter.Intersect(lineInner, intersection);
                    if (intersectExists)
                    {
                        intersectionPoints.insert(intersection);
                    }
                }
            }
        }
    }

    for (const auto& p : intersectionPoints)
    {
        if (p.first < xMin || p.first > xMax || p.second < yMin || p.second > yMax)
            continue;
        
        bool containedInBall = false;
        for (const auto& ball : balls)
        {
            if (ball.Contains(p))
            {
                containedInBall = true;
                break;
            }
        }

        if (!containedInBall)
        {
            std::cout << p.first << ", " << p.second << std::endl;
            break;
        }
    }
}

int main()
{
    AdventOfCodeExercise15_Part1();
    AdventOfCodeExercise15_Part2();
}