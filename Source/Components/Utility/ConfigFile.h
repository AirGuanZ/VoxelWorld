/*================================================================
Filename: ConfigFile.h
Date: 2018.1.17
Created by AirGuanZ
================================================================*/
#pragma once

#include <map>
#include <string>

class ConfigFileSection
{
public:
    const std::string &operator[](const std::string &key) const;

    ConfigFileSection(const ConfigFileSection&) = default;

private:
    friend class ConfigFile;

    ConfigFileSection(const std::map<std::string, std::string> &data)
        : data_(data)
    {

    }

    std::map<std::string, std::string> data_;
};

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

    bool FindSection(const std::string &section);

    //Ä¬ÈÏsectionÃûÎª"Global"
    const std::string &operator()(const std::string &section, const std::string &key) const;

    ConfigFileSection GetSection(const std::string &section) const;

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
