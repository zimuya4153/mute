#pragma once

#include <mc/deps/core/mce/UUID.h>
#include <mc/server/commands/CommandPermissionLevel.h>
#include <unordered_map>
#include <vector>

namespace mute {
struct Config {
    int                                      version     = 1;
    long long                                all         = 0;
    std::unordered_map<mce::UUID, long long> playerData  = {};
    std::vector<std::string>                 disabledCmd = {"me", "msg", "tell", "w"};
    struct {
        bool book = true;
        bool sign = true;
    } disabled;
    struct {
        std::string            command   = "mute";
        std::string            alias     = "";
        CommandPermissionLevel permLevel = CommandPermissionLevel::GameDirectors;
    } command;
};
} // namespace mute