
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

class TaxicabBall
{
public:
    TaxicabBall(const std::pair<int, int>& center, int radius)
        : m_center(center)
        , m_radius(radius)
    {}

    ~TaxicabBall() = default;

    static int Distance(const std::pair<int, int> p, const std::pair<int, int> q)
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

private:
    const std::pair<int, int> m_center;
    const int m_radius;
};

int ParseLocationExpression(const std::string expr)
{
    const auto exprTokens = Split(expr, '=');
    assert(exprTokens.size() == 2);
    return std::stoi(exprTokens.at(1));
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

    bool Contains(int point) const
    {
        return point <= m_max && point >= m_min;
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

    bool AreRangesOverlapping(const Range& rhs) const
    {
        return rhs.Contains(GetMin())
            || rhs.Contains(GetMax())
            || Contains(rhs.GetMin())
            || Contains(rhs.GetMax());
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

const std::pair<std::vector<TaxicabBall>, std::set<std::pair<int, int> > > ParseInput()
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

        beacons.insert(beacon);
        const auto radius = TaxicabBall::Distance(sensor, beacon);
        balls.push_back(TaxicabBall(sensor, radius));
    }

    return std::make_pair(balls, beacons);
}

bool RangeComparison(const Range& lhs, const Range& rhs)
{
    return lhs.GetMin() < rhs.GetMin();
}

const std::vector<Range> ComputeIntersection(
    const std::vector<TaxicabBall>& balls,
    const std::set<std::pair<int, int> > beacons,
    int y)
{
    std::vector<Range> ballIntersections;
    ballIntersections.reserve(balls.size());

    for (const auto ball : balls)
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

int ComputeBeaconsInIntersection(
    const std::set<std::pair<int, int> >& beacons,
    const std::vector<Range>& intersection,
    int y)
{
    int beaconsInIntersection = 0;
    for (const auto& beacon : beacons)
    {
        if (beacon.second == y)
        {
            for (const auto& r : intersection)
            {
                if (r.Contains(beacon.first))
                    beaconsInIntersection++;
            }
        }
    }

    return beaconsInIntersection;
}

void AdventOfCodeExercise15_Part1()
{
    const auto ballsAndBeacons = ParseInput();
    const auto balls = ballsAndBeacons.first;
    const auto beacons = ballsAndBeacons.second;

    const int y = 2000000;
    auto intersection = ComputeIntersection(balls, beacons, y);
    assert(intersection.size() == 1);

    std::cout << "Part 1:" << std::endl;
    std::cout << intersection.at(0).Size() << std::endl;
}
// TODO: another approach could be to walk the perimeter
// of each ball and find the points that aren't contained in any
void AdventOfCodeExercise15_Part2()
{
    const auto ballsAndBeacons = ParseInput();
    const auto balls = ballsAndBeacons.first;
    const auto beacons = ballsAndBeacons.second;

    const auto xMin = 0;
    const auto xMax = 4000000;
    const auto yMin = 0;
    const auto yMax = 4000000;

    int xCoordOfBeacon;
    int yCoordOfBeacon;
    for (auto i = yMin; i < yMax + 1; ++i)
    {
        auto intersection = ComputeIntersection(balls, beacons, i);

        for (auto& range : intersection)
        {
            range.Clip(xMin, xMax);
        }

        if (intersection.size() == 1)
        {
            if (intersection.at(0).IsEmpty())
                continue;

            assert(intersection.at(0).Size() == xMax - xMin);
        }
        else if (intersection.size() == 2)
        {
            std::sort(intersection.begin(), intersection.end(), RangeComparison);
            xCoordOfBeacon = intersection.at(0).GetMax() + 1;
            yCoordOfBeacon = i;
            break;
            //std::cout << "(" << xCoordOfBeacon << "," << yCoordOfBeacon << ")" << std::endl;
        }
        else
        {
            assert(false);
        }
    }

    // TODO: this still isn't working and it's not clear why
    const long long int output = xCoordOfBeacon * 4000000 + yCoordOfBeacon;
    std::cout << output << std::endl;
}

int main()
{
    AdventOfCodeExercise15_Part2();
}