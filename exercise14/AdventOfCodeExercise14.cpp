#include <iostream>
#include <sstream>
#include <fstream>

#include <string>
#include <vector>
#include <set>

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

struct BoundingBox
{
    int xMin;
    int yMin;
    int xMax;
    int yMax;
};

class RockFormation
{
public:
    RockFormation(const std::string& inputStr)
    {
        std::vector<int> xCoords;
        std::vector<int> yCoords;

        //498,4 -> 498,6 -> 496,6
        const auto tokens = Split(inputStr, ' ');
        std::vector<std::pair<int, int> > rockFormationEndPoints;
        for (const auto& token : tokens)
        {
            if (token == "->")
                continue;
            
            const auto coordinateStrs = Split(token, ',');
            assert(coordinateStrs.size() == 2);

            const auto x = std::stoi(coordinateStrs[0]);
            const auto y = std::stoi(coordinateStrs[1]);

            xCoords.push_back(x);
            yCoords.push_back(y);            

            std::pair<int, int> coord(x, y);
            rockFormationEndPoints.push_back(coord);
        }

        m_bbox.xMax = *std::max_element(xCoords.begin(), xCoords.end());
        m_bbox.yMax = *std::max_element(yCoords.begin(), yCoords.end());
        m_bbox.xMin = *std::min_element(xCoords.begin(), xCoords.end());
        m_bbox.yMin = *std::min_element(yCoords.begin(), yCoords.end());

        for (auto i = 1; i < rockFormationEndPoints.size(); ++i)
        {
            const auto startCoord = rockFormationEndPoints[i - 1];
            const auto endCoord = rockFormationEndPoints[i];
            const auto rockSegment = DensifyRocks(startCoord, endCoord);
            m_rocks.insert(rockSegment.begin(), rockSegment.end());
        }
    }

    ~RockFormation() = default;

    const BoundingBox& GetBBox() const
    {
        return m_bbox;
    }

    const std::set<std::pair<int, int> >& GetRocks() const
    {
        return m_rocks;
    }

private:
    const std::vector<std::pair<int, int> > DensifyRocks(
        const std::pair<int, int>& firstEndpoint, const std::pair<int, int>& secondEndpoint)
    {
        std::vector<std::pair<int, int> > rockFormation;

        if (firstEndpoint.first == secondEndpoint.first)
        {
            const auto start = firstEndpoint.second < secondEndpoint.second ? firstEndpoint : secondEndpoint;
            const auto end = firstEndpoint.second < secondEndpoint.second ? secondEndpoint : firstEndpoint;
            assert(start.second < end.second);
            for (auto i = start.second; i < end.second + 1; ++i)
            {
                rockFormation.push_back(std::make_pair(firstEndpoint.first, i));
            }

            return rockFormation;
        }
        else if (firstEndpoint.second == secondEndpoint.second)
        {
            const auto start = firstEndpoint.first < secondEndpoint.first ? firstEndpoint : secondEndpoint;
            const auto end = firstEndpoint.first < secondEndpoint.first ? secondEndpoint : firstEndpoint;
            assert(start.first < end.first);
            for (auto i = start.first; i <= end.first; ++i)
            {
                rockFormation.push_back(std::make_pair(i, firstEndpoint.second));
            }

            return rockFormation;
        }
        else
        {   
            // If we hit this assert, we have lines that go diagonally
            assert(false);
        }
    }

    BoundingBox m_bbox;
    std::set<std::pair<int, int> > m_rocks;
};

// This works but it's pretty slow
// Perf improvements: set.count might be too slow. Another container might be better
// Filter the rock formations (based on bbox) before searching them
class Cave
{
public:
    Cave(const std::vector<RockFormation>& rockFormations)
        : m_rockFormations(rockFormations)
    {
        m_bottomOfLowestRockFormation = INT_MIN;
        for (const auto& formation : m_rockFormations)
        {
            auto bbox = formation.GetBBox();
            if (m_bottomOfLowestRockFormation <= bbox.yMax)
                m_bottomOfLowestRockFormation = bbox.yMax;
        }
    }

    ~Cave() = default;

    bool IsOccupiedBySand(const std::pair<int, int>& coord)
    {
        return m_sand.count(coord) > 0;
    }

    bool IsOccupedByRock(const std::pair<int, int>& coord)
    {
        if (coord.second == m_bottomOfLowestRockFormation + 2)
            return true;

        // I'm not sure if we need the rock formations to be grouped
        // together, so this isn't as efficient as it could be
        for (const auto& formation : m_rockFormations)
        {
            const auto& rocks = formation.GetRocks();
            if (rocks.count(coord) > 0)
                return true;
        }

        return false;
    }

    bool IsOccupied(const std::pair<int, int>& coord)
    {
        return IsOccupedByRock(coord) || IsOccupiedBySand(coord);
    }

    // True if the sand came to rest, false if the sand falls into the void
    bool DropSand()
    {
        const auto source = std::make_pair(500, 0);
        auto position = source;
        while (true)
        {
            // A unit of sand always falls down one step if possible.
            auto below = std::make_pair(position.first, position.second + 1);
            
            // If the tile immediately below is blocked (by rock or sand),
            // the unit of sand attempts to instead move diagonally one step down and to the left.
            if (IsOccupied(below))
            {
                auto belowLeft = std::make_pair(position.first - 1, position.second + 1);

                // If that tile is blocked, the unit of sand attempts to instead move diagonally 
                // one step down and to the right.
                if (IsOccupied(belowLeft))
                {
                    auto belowRight = std::make_pair(position.first + 1, position.second + 1);
                    if (IsOccupied(belowRight))
                    {
                        // If all three possible destinations are blocked,
                        // the unit of sand comes to rest and no longer moves,
                        m_sand.insert(position);
                        if (position == source)
                            return true;

                        return false;
                    }
                    else
                    {
                        position = belowRight;
                    }
                }
                else
                {
                    position = belowLeft;
                }
            }
            else
            {
                position = below;
            }
        }

        // We should never get here
        assert(false);
        return false;
    }

    const int CountSand() const
    {
        return m_sand.size();
    }

private:
    const std::vector<RockFormation> m_rockFormations;
    std::set<std::pair<int, int> > m_sand;

    // The y-coordinate of the bottom of the last rock formation
    int m_bottomOfLowestRockFormation;
};

void AdventOfCodeExercise14()
{
    const auto lines = ReadTextFile("input_exercise_14.txt");

    std::vector<RockFormation> rockFormations;
    rockFormations.reserve(lines.size());
    for (const auto& line : lines)
    {
        RockFormation r(line);
        rockFormations.push_back(r);
    }

    Cave cave(rockFormations);
    bool enterTheVoid = false;
    while (!enterTheVoid)
    {
        enterTheVoid = cave.DropSand();
    }

    std::cout << cave.CountSand() << std::endl;
}

int main()
{
    AdventOfCodeExercise14();
}