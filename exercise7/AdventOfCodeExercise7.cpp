#include <iostream>
#include <sstream>
#include <fstream>

#include <string>
#include <vector>
#include <unordered_map>
#include <assert.h>
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

const char commandDelimiter = ' ';

class FileSystemNode;
class DirNode;
class FileNode;

class NodeVisitor
{
public:
    NodeVisitor(std::shared_ptr<const DirNode> root)
        : m_root(root) {}
    
    virtual ~NodeVisitor() = default;

    void Visit();

    virtual void VisitDir(const DirNode& dir) = 0;
    virtual void VisitFile(const FileNode& fileNode) = 0;
private:
    std::shared_ptr<const DirNode> m_root;
};

class FileSystemNode
{
public:
    FileSystemNode(const std::string name, std::shared_ptr<DirNode> parent)
        : m_name(name)
        , m_parent(parent)
    {}
        
    virtual ~FileSystemNode() {};

    virtual void Accept(NodeVisitor& c) const = 0;

    const std::string& GetName() const
    {
        return m_name;
    }

    std::shared_ptr<const DirNode> GetParent() const
    {
        return m_parent;
    }

private:
    const std::string m_name;
    std::shared_ptr<DirNode> m_parent;
};

class DirNode : public FileSystemNode
{
public:
    DirNode(const std::string name, std::shared_ptr<DirNode> parent)
        : FileSystemNode(name, parent)
        , m_parent(parent) {}

    ~DirNode() = default;

    void Accept(NodeVisitor& c) const override
    {
        c.VisitDir(*this);
    }

    void InsertFileOrSubdirecory(
        const std::string& name, std::shared_ptr<FileSystemNode> fileOrSubdirectory)
    {
        m_contents[name] = fileOrSubdirectory;
    }

    const std::unordered_map<std::string, std::shared_ptr<FileSystemNode> >& GetContents() const
    {
        return m_contents;
    }

    std::shared_ptr<DirNode> GetParent() const
    {
        return m_parent;
    }

private:
    std::unordered_map<std::string, std::shared_ptr<FileSystemNode> > m_contents;
    std::shared_ptr<DirNode> m_parent;
};

class FileNode : public FileSystemNode
{
public:
    FileNode(const std::string& filename, std::shared_ptr<DirNode> parent, const uint32_t size)
        : FileSystemNode(filename, parent) 
        , m_size(size)
    {}

    ~FileNode() = default;

    void Accept(NodeVisitor& c) const override
    {
        c.VisitFile(*this);
    }

    const uint32_t GetSize() const
    {
        return m_size;
    }

private:
    const uint32_t m_size;
};

void NodeVisitor::Visit()
{
    m_root->Accept(*this);
}

class FileSystemNodeFactory
{
public:
    FileSystemNodeFactory() = default;
    ~FileSystemNodeFactory() = default;

    static std::shared_ptr<FileSystemNode> Create(
        const std::string& input, std::shared_ptr<DirNode> parent)
    {
        const auto inputStrings = Split(input, ' ');

        assert(inputStrings.size() == 2);

        if (inputStrings[0] == "dir")
        {   
            std::string fullyQualifiedPath = parent->GetName() + inputStrings[1] + "/";
            return std::make_shared<DirNode>(fullyQualifiedPath, parent);
        }
        else
        {
            const auto fileSize = std::stoi(inputStrings[0]);
            return std::make_shared<FileNode>(inputStrings[1], parent, fileSize);
        }
    }
};

class DirectorySizeComputer : public NodeVisitor
{
public:
    static std::unordered_map<std::string, uint32_t> ComputeDirectoriesAndSizes(
        std::shared_ptr<const DirNode> root)
    {
        DirectorySizeComputer computer(root);
        computer.Visit();

        return computer.GetDirectoriesAndSizes();
    }

private:
    DirectorySizeComputer(std::shared_ptr<const DirNode> root)
        : NodeVisitor(root) {}

    ~DirectorySizeComputer() = default;

    void VisitDir(const DirNode& n) override
    {
        for (const auto& fileOrDir : n.GetContents())
        {
            fileOrDir.second->Accept(*this);
        }
    }

    void VisitFile(const FileNode& n) override
    {   
        auto parentDir = n.GetParent();
        const auto fileSize = n.GetSize();
        while (parentDir)
        {
            m_directorySizes[parentDir->GetName()] += fileSize;
            parentDir = parentDir->GetParent();
        }
    }

    const std::unordered_map<std::string, uint32_t>& GetDirectoriesAndSizes() const
    {
        return m_directorySizes;
    }

private:
    std::unordered_map<std::string, uint32_t> m_directorySizes;
};

struct CommandInOut
{
    std::string input;
    std::vector<std::string> output;
};

class CommandParser
{
public:
    CommandParser() = default;
    ~CommandParser() = default;

    void VisitCommand(
        const std::string& command, const CommandInOut& inOut)
    {
        if (command == "cd")
        {
            VisitChangeDirectory(inOut);
        }
        else if (command == "ls")
        {
            VisitList(inOut);
        }
        else
        {
            assert(false);
        }
    }

    void VisitChangeDirectory(const CommandInOut& inOut)
    {
        if (inOut.input == "/")
        {
            if (!m_rootNode)
                m_rootNode = std::make_shared<DirNode>(inOut.input, m_rootNode);

            m_currentDirectory = m_rootNode;
        }
        else if (inOut.input == "..")
        {
            const auto parentNode = m_currentDirectory->GetParent();
            if (parentNode)
                m_currentDirectory = parentNode;
        }
        else
        {
            const auto& contents = m_currentDirectory->GetContents();

            std::string fullyQualifiedName = m_currentDirectory->GetName() + inOut.input + "/";
            const auto itr = contents.find(fullyQualifiedName);
            assert(itr != contents.end());
            m_currentDirectory = std::dynamic_pointer_cast<DirNode>(itr->second);
        }
    }

    void VisitList(const CommandInOut& inOut)
    {
        assert(m_currentDirectory);
        const auto factory = FileSystemNodeFactory();
        for (const auto& listItem : inOut.output)
        {
            const auto node = factory.Create(listItem, m_currentDirectory);
            m_currentDirectory->InsertFileOrSubdirecory(node->GetName(), node);
        }
    }

    std::shared_ptr<const DirNode> GetRoot() const
    {
        return m_rootNode;
    }

private:
    std::shared_ptr<DirNode> m_rootNode;
    std::shared_ptr<DirNode> m_currentDirectory;
};

void AdventOfCodeExercise7()
{
    const auto lines = ReadTextFile("input_exercise_7.txt");
    CommandParser commandParser;
    CommandInOut inOut;
    std::string currentCommandName;
    for (auto i = 0; i < lines.size(); ++i)
    {
        const auto tokens = Split(lines[i], commandDelimiter);
        assert(tokens.size() > 0);

        if (tokens[0] == "$")
        {
            // Processing a command
            currentCommandName = tokens[1];
            if (tokens.size() > 2)
                inOut.input = tokens[2];
        }
        else
        {
            // Processing command output
            inOut.output.push_back(lines[i]);
        }
        
        const auto nextLine = lines[i+1];
        if (nextLine.empty())
        {
            commandParser.VisitCommand(currentCommandName, inOut);
            break;
        }

        const auto nextLineStrings = Split(nextLine, commandDelimiter);
        if (nextLineStrings[0] == "$")
        {
            // We've reached the next command, so we have the input and output
            // for the current command. Visit it!
            commandParser.VisitCommand(currentCommandName, inOut);
            currentCommandName.clear();
            inOut.input.clear();
            inOut.output.clear();
        }
    }

    const auto thresholdPart1 = 100000;
    auto totalPart1 = 0;

    uint32_t totalPart2 = UINT32_MAX;
    const auto totalDiskSpace = 70000000;
    const auto neededDiskSpace = 30000000;
    const auto& directoriesAndSizes = 
            DirectorySizeComputer::ComputeDirectoriesAndSizes(commandParser.GetRoot());

    const auto rootDir = directoriesAndSizes.find("/");
    assert(rootDir != directoriesAndSizes.end());
    const auto unusedSpace = totalDiskSpace - rootDir->second;
    const auto thresholdPart2 = neededDiskSpace - unusedSpace;
    
    for (const auto& directory : directoriesAndSizes)
    {
        if (directory.second <= thresholdPart1)
        {
            totalPart1 += directory.second;
        }

        if (directory.second >= thresholdPart2)
        {
            totalPart2 = std::min(totalPart2, directory.second);
        }

    }

    std::cout << totalPart1 << std::endl;
    std::cout << totalPart2 << std::endl;
}

int main()
{
    AdventOfCodeExercise7();
}