#pragma once

#include "Entry.h"
#include "Global.h"

#include <ll/api/command/CommandHandle.h>
#include <ll/api/command/CommandRegistrar.h>
#include <mc/server/commands/CommandOrigin.h>
#include <mc/server/commands/CommandOutput.h>
#include <mc/world/actor/player/Player.h>
#include <unordered_map>

struct CmdParam {
    CommandSelector<Player> player;
    int                     time = 0;
};

void mutePlayer(CommandOutput& output, CommandSelectorResults<Player> players, int time) {
    if (players.empty()) return output.error("commands.generic.noTargetMatch");
    std::ostringstream oss;
    bool               first     = true;
    auto&              config    = mute::Entry::getInstance().getConfig();
    auto               unbanTime = time < 0 ? -1
                                            : static_cast<long long>(std::chrono::system_clock::to_time_t(
                                    std::chrono::seconds(time) + std::chrono::system_clock::now()
                                ));
    for (auto* player : players) {
        if (!first) oss << ", ";
        first = false;
        config.playerData.erase(player->getUuid());
        if (time != 0) config.playerData.emplace(player->getUuid(), unbanTime);
        oss << player->getRealName();
    }
    mute::Entry::getInstance().saveConfig();
    if (time == 0) output.success(tr("commands.mutePlayer.unban"), {CommandOutputParameter(oss.str())});
    else
        output.success(
            tr("commands.mutePlayer.ban"),
            {CommandOutputParameter(oss.str()), CommandOutputParameter(timestampToString(unbanTime))}
        );
}

void mutePlayer(CommandOutput& output, int time) {
    auto& config    = mute::Entry::getInstance().getConfig();
    auto  unbanTime = time < 0 ? -1
                               : static_cast<long long>(std::chrono::system_clock::to_time_t(
                                    std::chrono::seconds(time) + std::chrono::system_clock::now()
                                ));
    config.all      = unbanTime;
    mute::Entry::getInstance().saveConfig();
    if (time == 0) output.success(tr("commands.muteAll.unban"));
    else output.success(tr("commands.muteAll.ban"), {CommandOutputParameter(timestampToString(unbanTime))});
}

void registerCmd() {
    auto& cmdConfig = mute::Entry::getInstance().getConfig().command;
    auto& cmd       = ll::command::CommandRegistrar::getInstance()
                    .getOrCreateCommand(cmdConfig.command, tr("commands.description"), cmdConfig.permLevel);
    if (!cmdConfig.alias.empty()) cmd.alias(cmdConfig.alias);
    for (auto& item : std::unordered_map<std::string, int>({
             {"s", 1           }, // 秒
             {"m", 60          }, // 分
             {"h", 60 * 60     }, // 时
             {"d", 60 * 60 * 24}, // 天
    })) {
        cmd.overload<CmdParam>()
            .text("all")
            .optional("time")
            .postfix(item.first)
            .execute([item](CommandOrigin const&, CommandOutput& output, CmdParam result) -> void {
                mutePlayer(output, result.time * item.second);
            });
        cmd.overload<CmdParam>()
            .text("player")
            .required("player")
            .optional("time")
            .postfix(item.first)
            .execute([item](CommandOrigin const& origin, CommandOutput& output, CmdParam result) -> void {
                mutePlayer(output, result.player.results(origin), result.time * item.second);
            });
    }
    cmd.overload<CmdParam>().text("all").optional("time").execute(
        [](CommandOrigin const&, CommandOutput& output, CmdParam result) -> void { mutePlayer(output, result.time); }
    );
    cmd.overload<CmdParam>().text("player").required("player").optional("time").execute(
        [](CommandOrigin const& origin, CommandOutput& output, CmdParam result) -> void {
            mutePlayer(output, result.player.results(origin), result.time);
        }
    );
}