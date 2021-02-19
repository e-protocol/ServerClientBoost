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

    typename std::map<_Key,_Value>::iterator find(const _Key &key)
    {
        mtx.lock();
        auto it = map.find(key);
        mtx.unlock();
        return it;
    }

    _Value getValue(const _Key &key)
    {
        mtx.lock();
        auto it = map.find(key);
        _Value value;
        
        if(it != map.end())
            value = it->second;
        
        mtx.unlock();
        return value;
    }

    typename std::map<_Key,_Value>::iterator begin()
    {
        mtx.lock();
        typename std::map<_Key,_Value>::iterator it;
        it = map.begin();
        mtx.unlock();
        return it;
    }

    typename std::map<_Key,_Value>::iterator end()
    {
        mtx.lock();
        typename std::map<_Key,_Value>::iterator it;
        it = map.end();
        mtx.unlock();
        return it;
    }
};

#endif
