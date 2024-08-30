#include "Entry.h"
#include "Global.h"

#include <ll/api/memory/Hook.h>
#include <mc/network/ServerNetworkHandler.h>
#include <mc/network/packet/BookEditPacket.h>
#include <mc/network/packet/ToastRequestPacket.h>
#include <mc/network/packet/TextPacket.h>
#include <mc/server/commands/MinecraftCommands.h>
#include <mc/world/actor/player/Player.h>

template <typename... Args>
inline std::string format(const std::string& fmt, Args... args) {
    try {
        return fmt::vformat(fmt, fmt::make_format_args(args...));
    } catch (...) {
        return fmt;
    }
}

bool checkMute(Player& player) {
    if (player.isOperator()) return false;
    auto& config = mute::Entry::getInstance().getConfig();
    auto  time   = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    if (config.all != 0) {
        if (config.all == -1 || config.all > time) {
            ToastRequestPacket(
                tr("mute.tip.title", getLocaleCode(player)),
                format(tr("mute.tip.all", getLocaleCode(player)), timestampToString(config.all, getLocaleCode(player)))
            )
                .sendTo(player);
            return true;
        }
        config.all = 0;
    }
    if (config.playerData.contains(player.getUuid())) {
        if (config.playerData.at(player.getUuid()) == -1 || config.playerData.at(player.getUuid()) > time) {
            ToastRequestPacket(
                tr("mute.tip.title", getLocaleCode(player)),
                format(
                    tr("mute.tip.player", getLocaleCode(player)),
                    timestampToString(config.playerData.at(player.getUuid()), getLocaleCode(player))
                )
            )
                .sendTo(player);
            return true;
        }
        config.playerData.erase(player.getUuid());
    }
    mute::Entry::getInstance().saveConfig();
    return false;
}

bool checkCmdDisabled(std::string& str) {
    auto& prefixes = mute::Entry::getInstance().getConfig().disabledCmd;
    if (str.starts_with("/")) str.erase(0, 1);
    for (auto prefix : prefixes)
        if (str.starts_with(prefix)) return true;
    return false;
}

LL_AUTO_TYPE_INSTANCE_HOOK(
    PlayerSendMessageHook,
    HookPriority::High,
    ServerNetworkHandler,
    &ServerNetworkHandler::handle,
    void,
    NetworkIdentifier const& source,
    TextPacket const&        packet
) {
    if (auto player = getServerPlayer(source, packet.mClientSubId); player.has_value() && checkMute(player)) return;
    origin(source, packet);
}

LL_AUTO_TYPE_INSTANCE_HOOK(
    PlayerRunCmdHook,
    HookPriority::High,
    MinecraftCommands,
    &MinecraftCommands::executeCommand,
    MCRESULT,
    CommandContext& context,
    bool            suppressOutput
) {
    if (context.mOrigin->getOriginType() == CommandOriginType::Player) {
        auto player = static_cast<Player*>(context.mOrigin->getEntity());
        if (checkCmdDisabled(context.mCommand) && checkMute(*player)) return MCRESULT_CommandsDisabled;
    }
    return origin(context, suppressOutput);
}

LL_AUTO_TYPE_INSTANCE_HOOK(
    PlayerEditBookHook,
    HookPriority::Normal,
    ServerNetworkHandler,
    &ServerNetworkHandler::handle,
    void,
    NetworkIdentifier const& source,
    BookEditPacket const&    packet
) {
    if (mute::Entry::getInstance().getConfig().disabled.book)
        if (auto player = getServerPlayer(source, packet.mClientSubId); player.has_value() && checkMute(player)) return;
    origin(source, packet);
}

LL_AUTO_TYPE_INSTANCE_HOOK(
    PlayeOpenSignHook,
    HookPriority::Normal,
    Player,
    "?openSign@ServerPlayer@@UEAAXAEBVBlockPos@@_N@Z",
    void,
    const BlockPos& pos,
    bool            isFront
) {
    if (mute::Entry::getInstance().getConfig().disabled.book && checkMute(*this)) return;
    origin(pos, isFront);
}