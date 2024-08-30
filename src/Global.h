#pragma once

#include <string>

extern void        registerCmd();
extern std::string tr(std::string key, std::string language = {});
extern std::string getLocaleCode(class Player& player);
extern std::string timestampToString(long long timestamp, std::string language = {});