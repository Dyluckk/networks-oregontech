#include "Locked_Map.h"

Locked_Map::Locked_Map()
{
}

string Locked_Map::Find(string key)
{
    string value = "";
    mtx.lock();
    if(map.find(key) != map.end())
        value = map.find(key)->second;
    mtx.unlock();

    return value;
}

int Locked_Map::Add(string key, string value)
{
    mtx.lock();
    map[key] = value;
    mtx.unlock();

    return 0;
}

vector<string> Locked_Map::GetKeys()
{
    vector<string> keys;
    for(std::unordered_map<string, string>::iterator iter = map.begin();
            iter != map.end(); ++iter)
    {
        keys.push_back(iter->first);
    }
    return keys;
}

Locked_Map::~Locked_Map(){}
