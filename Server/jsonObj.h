#ifndef JSONOBJ_H
#define JSONOBJ_H

#include "safeMap.h"
#include "json.hpp"

enum { max_length = 1024 };
    
class JsonObj
{
private:
    std::string response;
    std::string request;
    char* m_data;
    SafeMap<std::string,std::string> *m_safeMap;
    nlohmann::json json;
    
    nlohmann::json readData(nlohmann::json &json)
    {
        auto pair = m_safeMap->find(json["key"]);
        json.clear();
                    
        if(!pair.first.empty())
        {    
            json["Status"] = "ok";
            json["key"] = pair.second;
        }
        else
        {
            json["Status"] = "error";
            json["description"] = "Not Found!";
        }
        return json;
    }
    
    nlohmann::json writeData(nlohmann::json &json)
    {
        auto pair = m_safeMap->find(json["key"]);
                
        if(!pair.first.empty())
        {
            json.clear();
            json["Status"] = "error";
            json["description"] = "Key already exists!";
        }
        else
        {
            m_safeMap->insert(json["key"],json["value"]);
            json.clear();
            json["Status"] = "ok";
        }
        return json;
    }
    
    void proccessJsonObj()
    {
        //check json validness
        if(json["Request"] == "read" && !json["key"].empty())
            json = readData(json);
        else if(json["Request"] == "write" && !json["key"].empty() 
            && !json["value"].empty())
            json = writeData(json);
        else
        {
            json.clear();
            json["Status"] = "error";
            json["description"] = "Invalid command!";
        }
    }

public:
    JsonObj(char* data, SafeMap<std::string,std::string> *safeMap) :
        m_data(data), m_safeMap(safeMap)
    {
        std::string in;
        
        for(size_t k = 0; k < max_length; ++k)
        {
            if(m_data[k] != '\n')
                in.push_back(m_data[k]);
            else
                break;
        }
        
        json = nlohmann::json::parse(in);
        request = json.dump();
        
        proccessJsonObj();
    }
    
    std::string getResponse() { return json.dump(); }
    std::string getRequest() { return request; }
};

#endif
