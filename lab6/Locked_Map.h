#pragma once

#include <unordered_map>
#include <stdio.h>
#include <string>
#include <thread>
#include <mutex>
#include <vector>
#include <iterator>

using std::string;
using std::vector;

class Locked_Map
{
    public:
        Locked_Map();
        string Find(string find);
        int Add(string key, string value);
        vector<string> GetKeys();
        string Serialize();
        ~Locked_Map();

    private:
        std::unordered_map<string, string> map;
        std::mutex mtx;
};
