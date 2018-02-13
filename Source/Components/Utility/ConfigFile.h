/*================================================================
Filename: ConfigFile.h
Date: 2018.1.17
Created by AirGuanZ
================================================================*/
#ifndef VW_CONFIG_FILE_H
#define VW_CONFIG_FILE_H

#include <map>
#include <string>

class ConfigFileSection
{
public:
    const std::string &operator[](const std::string &key) const
    {
        static const std::string EMPTY;
        auto it = data_.find(key);
        return it != data_.end() ? it->second : EMPTY;
    }

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

    const std::string &operator()(const std::string &section, const std::string &key) const;

    ConfigFileSection GetSection(const std::string &section) const
    {
        static const std::map<std::string, std::string> EMPTY;
        decltype(map_)::const_iterator it = map_.find(section);
        return (it != map_.end()) ? ConfigFileSection(it->second) : ConfigFileSection(EMPTY);
    }

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
