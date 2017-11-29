#include "Locked_Multi.h"

Locked_Multi::Locked_Multi(){}

MIterPair Locked_Multi::GetNS(string key)
{
    MIterPair ns_list;
    mtx.lock();
    ns_list = map.equal_range(key);
    mtx.unlock();

    return ns_list;
}

vector<string> Locked_Multi::GetKeys()
{
    vector<string> all_keys;
    mtx.lock();
    std::multimap<string ,std::pair<string, string>>::iterator it;
    std::multimap<string ,std::pair<string, string>>::iterator end;
    for(it = map.begin(), end = map.end(); it != end; it = map.upper_bound(it->first))
    {
        all_keys.push_back(it->first);
    }
    mtx.unlock();

    return all_keys;
}

int Locked_Multi::Emplace(string key, std::pair<string, string> value)
{
    mtx.lock();
    map.emplace(key, value);
    mtx.unlock();

    return 0;
}

string Locked_Multi::Serialize()
{
    vector<string> all_keys = GetKeys();
    string serialized_string = "";
    for(unsigned int i = 0; i < all_keys.size(); ++i)
    {
        serialized_string += "{";
        serialized_string += all_keys[i];
        serialized_string += ":";
        MIterPair iterator1;
        iterator1 = GetNS(all_keys[i]);
        while(iterator1.first != iterator1.second)
        {

            serialized_string += "{";
            serialized_string += iterator1.first->second.first;
            serialized_string += ":";
            serialized_string += iterator1.first->second.second;
            serialized_string += "}";
            iterator1.first++;
            if(iterator1.first != iterator1.second)
                serialized_string += ",";
        }
        serialized_string += "}";
    }

    return serialized_string;
}

Locked_Multi::~Locked_Multi(){}
