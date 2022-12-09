#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

namespace AdventOfCodeUtils
{
    vector<string> ReadTextFile(string input_filename)
    {
        ifstream myfile;
        string line;

        vector<string> output;
        myfile.open(input_filename);
        if (myfile.is_open())
        {
            while (getline(myfile,line))
            {
                output.push_back(string(line));
            }

            myfile.close();
        }
        return output;
    }
}