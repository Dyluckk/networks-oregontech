#pragma once

#include <stdio.h>
#include <string>
#include <thread>
#include <mutex>
#include <map>
#include <utility>
#include <vector>
#include <iterator>

using std::string;
using std::vector;

typedef std::pair<std::multimap<string, std::pair<string, string>>::iterator,
        std::multimap<string, std::pair<string, string>>::iterator> MIterPair;
class Locked_Multi
{
    public:
        Locked_Multi();
        MIterPair GetNS(string find);
        int Emplace(string key, std::pair<string, string> value);
        vector<string> GetKeys();
        string Serialize();
        ~Locked_Multi();

    private:
        std::multimap<string, std::pair<string, string>> map;
        std::mutex mtx;
};
