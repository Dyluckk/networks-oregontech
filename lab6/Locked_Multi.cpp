#include "Locked_Multi.h"

Locked_Multi::Locked_Multi(){}
Locked_Multi::~Locked_Multi(){}

MIterPair Locked_Multi::GetNS(string key)
{
    MIterPair ns_list;
    mtx.lock();
    ns_list = map.equal_range(key);
    mtx.unlock();

    return ns_list;
}

int Locked_Multi::Emplace(string key, std::pair<string, string> value)
{
    mtx.lock();
    map.emplace(key, value);
    mtx.unlock();

    return 0;
}
