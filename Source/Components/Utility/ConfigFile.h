/*================================================================
Filename: ConfigFile.h
Date: 2018.1.17
Created by AirGuanZ
================================================================*/
#ifndef VW_CONFIG_FILE_H
#define VW_CONFIG_FILE_H

#include <map>
#include <string>

class ConfigFile
{
public:
    using ConfigSection = std::map<std::string, std::string>;

    ConfigFile(void);
    ConfigFile(const std::wstring &filename);

    bool LoadFromFile(const std::wstring &filename);
    bool IsAvailable(void) const;
    operator bool(void) const;
    void Clear();

    const std::string &operator()(const std::string &section, const std::string &key) const;

private:
    enum State
    {
        Uninitialized,
        Available,
        Error
    };

    State state_;
    std::map<std::string, ConfigSection> map_;
};

#endif //VW_CONFIG_FILE_H
