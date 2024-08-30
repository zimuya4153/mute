#pragma once
#define _CRT_SECURE_NO_WARNINGS 1

#include "Entry.h"
#include "Global.h"

#include <ll/api/Config.h>
#include <ll/api/i18n/I18n.h>
#include <ll/api/mod/RegisterHelper.h>
#include <mc/certificates/WebToken.h>
#include <mc/network/ConnectionRequest.h>
#include <mc/world/actor/player/Player.h>
#include <memory>

namespace mute {

static std::unique_ptr<Entry> instance;

Entry& Entry::getInstance() { return *instance; }

bool Entry::load() {
    ll::i18n::load(getSelf().getLangDir());
    mConfig.emplace();
    if (!loadConfig()) saveConfig();
    return true;
}

bool Entry::enable() {
    registerCmd();
    return true;
}

bool Entry::disable() { return true; }

bool Entry::unload() { return true; }

Config& Entry::getConfig() { return mConfig.value(); }

bool Entry::loadConfig() {
    try {
        return ll::config::loadConfig(*mConfig, getSelf().getConfigDir() / u8"config.json");
    } catch (...) {
        return false;
    }
}

bool Entry::saveConfig() { return ll::config::saveConfig(*mConfig, getSelf().getConfigDir() / u8"config.json"); }

} // namespace mute

LL_REGISTER_MOD(mute::Entry, mute::instance);

std::string tr(std::string key, std::string language) {
    return std::string(ll::i18n::getInstance()->get(key, language));
}

std::string getLocaleCode(Player& player) {
    if (auto request = player.getConnectionRequest()) {
        return request->mRawToken->mDataInfo["LanguageCode"].asString({});
    }
    return {};
}

std::string timestampToString(long long timestamp, std::string language) {
    if (timestamp < 0) return tr("time.permanent", language);
    time_t     time_t_timestamp = static_cast<time_t>(timestamp);
    struct tm* local_time       = std::localtime(&time_t_timestamp);
    char       time_string[26];
    std::strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S", local_time);
    return std::string(time_string);
}