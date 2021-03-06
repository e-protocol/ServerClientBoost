#ifndef MAPSAFE_H
#define MAPSAFE_H

#include <map>
#include <string>
#include <mutex>

template<typename _Key, typename _Value>

class SafeMap
{
private:
    std::mutex mtx;
    std::map<_Key,_Value> map;

public:
    SafeMap(){};

    void insert(const _Key &key, const _Value &value)
    {
        mtx.lock();
        map.insert(std::pair<_Key,_Value>(key,value));
        mtx.unlock();
    }

    void remove(const _Key &key)
    {
        mtx.lock();
        map.remove(key);
        mtx.unlock();
    }

    void clear()
    {
        mtx.lock();
        map.clear();
        mtx.unlock();
    }

    typename std::pair<_Key,_Value> find(const _Key &key)
    {
        mtx.lock();
        auto it = map.find(key);
        mtx.unlock();
        if(it!=map.end())
            return std::pair<_Key,_Value>(it->first,it->second);
        return std::pair<_Key,_Value>();
    }
};

#endif
