#pragma once

#include "Config.h"

#include <ll/api/mod/NativeMod.h>

namespace mute {

class Entry {

public:
    static Entry& getInstance();

    Entry(ll::mod::NativeMod& self) : mSelf(self) {}

    [[nodiscard]] ll::mod::NativeMod& getSelf() const { return mSelf; }

    bool load();

    bool enable();

    bool disable();

    bool unload();

    Config& getConfig();

    bool loadConfig();

    bool saveConfig();

private:
    ll::mod::NativeMod&   mSelf;
    std::optional<Config> mConfig;
};

} // namespace mute