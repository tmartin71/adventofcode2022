
#include <iostream>
#include <sstream>
#include <fstream>

#include <string>
#include <vector>

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

class ListNode;

class Node
{
public:
    Node(std::shared_ptr<ListNode> parent = nullptr)
        : m_parent(parent)
    {}
    virtual ~Node() {}

    std::shared_ptr<ListNode> GetParent() const
    {
        return m_parent;
    }

private:
    std::shared_ptr<ListNode> m_parent;
};

class IntegerNode : public Node
{
public:
    IntegerNode(std::shared_ptr<ListNode> parent, const int v)
        : Node(parent)
        , m_value(v) {}
    ~IntegerNode() = default;

    const int GetValue()
    {
        return m_value;
    }

private:
    const int m_value;
};

class ListNode : public Node
{
public:
    ListNode(std::shared_ptr<ListNode> parent)
        : Node(parent) {}
    ~ListNode() = default;

    void InsertIntegerOrList(std::shared_ptr<Node> n)
    {
        m_contents.push_back(n);
    }

    const std::vector<std::shared_ptr<Node> >& GetContents() const
    {
        return m_contents;
    }

private:
    std::vector<std::shared_ptr<Node> > m_contents;
};

std::shared_ptr<IntegerNode> ParseInteger(std::shared_ptr<ListNode> parent, const std::string& integer)
{
    const auto v = std::stoi(integer);
    return std::make_shared<IntegerNode>(parent, v);
}

std::shared_ptr<ListNode> ParseList(std::shared_ptr<ListNode> parent, const std::string& contents)
{
    assert(contents.front() == '[');
    assert(contents.back() == ']');
    const std::string workingString(contents.begin() + 1, contents.end() - 1);

    if (workingString.empty())
    {
        return std::make_shared<ListNode>(parent);
    }

    auto l = std::make_shared<ListNode>(parent);
    int idx = 0;
    const auto workingStringSize = workingString.size();
    while (idx < workingStringSize)
    {
        // Parsing a sub-list
        if (workingString.at(idx) == '[')
        {
            // Find the closing bracket
            auto subIdx = idx + 1;
            auto len = 1;
            int unmatchedOpeningBrackets = 0;
            while (subIdx < workingStringSize)
            {
                // TODO: the problem is here - we find the wrong one
                // I think we might need to count opening brackets that we encounter along the way
                // And for each opening bracket we see, ignore a closing bracket
                // If we see a closing bracket and there is a comma following it
                // then this is the closing bracket we are looking for.
                if (workingString.at(subIdx) == '[')
                {
                    unmatchedOpeningBrackets++;
                }

                if (workingString.at(subIdx) == ']')
                {
                    if (unmatchedOpeningBrackets == 0)
                    {
                        ++len;
                        break;
                    }
                    
                    unmatchedOpeningBrackets--;
                }

                ++subIdx;
                ++len;
            }

            assert(subIdx >= idx);

            const auto subListStr = workingString.substr(idx, len);
            auto subList = ParseList(l, subListStr);

            l->InsertIntegerOrList(subList);
            idx += len;
        }
        else if (workingString.at(idx) == ',')
        {
            // We are at the comma between list entries
            ++idx;
            continue;
        }
        else
        {
            // This is an integer, find the ending character (a comma or the end of the list)
            auto substrIdx = workingString.find(',', idx);
            if (substrIdx < idx)
            {
                substrIdx = workingStringSize - 1;
            }

            assert(substrIdx >= idx);
            const auto len = substrIdx - idx;
            const auto intStr = workingString.substr(idx, len);

            l->InsertIntegerOrList(ParseInteger(l, intStr));
            idx += len;
        }
    }

    return l;
}

enum Outcome
{
    Right,
    Wrong,
    Inconclusive
};

Outcome CompareLists(const std::shared_ptr<ListNode> lhsRoot, const std::shared_ptr<ListNode> rhsRoot)
{
    auto lhsContents = lhsRoot->GetContents();
    auto rhsContents = rhsRoot->GetContents();

    for (auto rhsIt = rhsContents.begin(), lhsIt = lhsContents.begin();
        rhsIt != rhsContents.end() && lhsIt != lhsContents.end(); ++rhsIt, ++lhsIt)
    {
        auto lhsPtr = *lhsIt;
        auto rhsPtr = *rhsIt;

        std::shared_ptr<ListNode> lhsAsList = dynamic_pointer_cast<ListNode>(lhsPtr);
        std::shared_ptr<IntegerNode> lhsAsInt = dynamic_pointer_cast<IntegerNode>(lhsPtr);
        std::shared_ptr<ListNode> rhsAsList = dynamic_pointer_cast<ListNode>(rhsPtr);
        std::shared_ptr<IntegerNode> rhsAsInt = dynamic_pointer_cast<IntegerNode>(rhsPtr);
        
        if (lhsAsList && rhsAsList)
        {
            /*If both values are lists, compare the first value of each list,
            then the second value, and so on.
            If the left list runs out of items first, the inputs are in the right order.
            If the right list runs out of items first, the inputs are not in the right order.
            If the lists are the same length and no comparison makes a decision about the order,
            continue checking the next part of the input.*/
            const auto outcome = CompareLists(lhsAsList, rhsAsList);
            if (outcome == Outcome::Inconclusive)
            {
                continue;
            }

            return outcome;
        }
        else if (lhsAsInt && rhsAsInt)
        {
            auto lhs = lhsAsInt->GetValue();
            auto rhs = rhsAsInt->GetValue();
            if (lhs == rhs)
            {
                continue;
            }

            if (lhs < rhs)
            {
                return Outcome::Right;
            }
            else if (lhs > rhs)
            {
                return Outcome::Wrong;
            }
        }
        else if (lhsAsInt && rhsAsList)
        {
            auto parent = lhsAsInt->GetParent();
            auto asList = std::make_shared<ListNode>(parent);
            asList->InsertIntegerOrList(lhsAsInt);
            const auto outcome = CompareLists(asList, rhsAsList);
            if (outcome == Outcome::Inconclusive)
            {
                continue;
            }

            return outcome;
        }
        else if (lhsAsList && rhsAsInt)
        {
            auto parent = rhsAsInt->GetParent();
            auto asList = std::make_shared<ListNode>(parent);
            asList->InsertIntegerOrList(rhsAsInt);
            const auto outcome = CompareLists(lhsAsList, asList);
            if (outcome == Outcome::Inconclusive)
            {
                continue;
            }

            return outcome;
        }
    }

    if (lhsContents.size() < rhsContents.size())
    {
        return Outcome::Right;
    }
    else if (lhsContents.size() > rhsContents.size())
    {
        return Outcome::Wrong;
    }

    return Outcome::Inconclusive;
}

// TODO:
// For the sample inputs, I get a different answer depending on whether or not I break
// while we're comparing expressions. This is quite confusing. Maybe it's a lifetime issue?
// For the actual input, parsing fails because of some crazy bracket combinations on the first line
void AdventOfCodeExercise13()
{
    auto lines = ReadTextFile("input_exercise_13.txt");
    
    // Insert divider packets
    const auto dividerPacket1 = "[[2]]";
    const auto dividerPacket2 = "[[6]]";
    lines.push_back(dividerPacket1);
    lines.push_back(dividerPacket2);

    const auto numRows = lines.size();
    assert(lines.size() > 0);

    uint32_t lineCount = 0;
    std::vector<std::shared_ptr<ListNode> > lhsExpressions;
    std::vector<std::shared_ptr<ListNode> > rhsExpressions;
    for (const auto& line : lines)
    {
        if (lineCount == 0)
        {
            lhsExpressions.push_back(ParseList(nullptr, line));
            lineCount++;
        }
        else if (lineCount == 1)
        {
            rhsExpressions.push_back(ParseList(nullptr, line));
            lineCount++;
        }
        else
        {
            lineCount = 0;
        }
    }

    assert(lhsExpressions.size() == rhsExpressions.size());

    std::vector<Outcome> outcomes;
    outcomes.reserve(lhsExpressions.size());
    std::vector<uint32_t> indicesOfCorrectlyOrderedPackets;
    for (auto i = 0; i < lhsExpressions.size(); ++i)
    {
        const auto outcome = CompareLists(lhsExpressions[i], rhsExpressions[i]);
        switch (outcome)
        {
            case Outcome::Right:
            {
                indicesOfCorrectlyOrderedPackets.push_back(i + 1);
                break;
            }
            default:
            {
                continue;
            }

        }
    }


    auto totalPart1 = 0;
    for (const auto& i : indicesOfCorrectlyOrderedPackets)
    {
        totalPart1 += i;
        std::cout << i << std::endl;
    }

    std::cout << totalPart1 << std::endl;
}

int main()
{
    AdventOfCodeExercise13();
}