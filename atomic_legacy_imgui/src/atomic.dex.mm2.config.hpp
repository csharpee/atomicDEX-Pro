/******************************************************************************
 * Copyright © 2013-2019 The Komodo Platform Developers.                      *
 *                                                                            *
 * See the AUTHORS, DEVELOPER-AGREEMENT and LICENSE files at                  *
 * the top-level directory of this distribution for the individual copyright  *
 * holder information and the developer policies on copyright and licensing.  *
 *                                                                            *
 * Unless otherwise agreed in a custom licensing agreement, no part of the    *
 * Komodo Platform software, including this file may be copied, modified,     *
 * propagated or distributed except according to the terms contained in the   *
 * LICENSE file                                                               *
 *                                                                            *
 * Removal or modification of this copyright notice is prohibited.            *
 *                                                                            *
 ******************************************************************************/

#pragma once

//! PCH Headers
#include "atomic.dex.pch.hpp"

namespace atomic_dex
{
    using nlohmann::json;

    struct mm2_config
    {
        std::string gui{"MM2GUI"};
        int64_t     netid{9999};
#ifdef _WIN32
        std::string userhome{std::getenv("HOMEPATH")};
#else
        std::string userhome{std::getenv("HOME")};
#endif
        // TODO: Handle passphrase
        std::string passphrase{"thisIsTheNewProjectSeed2019##"};
        std::string rpc_password{"atomic_dex_mm2_passphrase"};
    };

    void from_json(const json& j, mm2_config& cfg);

    void to_json(json& j, const mm2_config& cfg);

    inline void
    from_json(const json& j, mm2_config& cfg)
    {
        cfg.gui          = j.at("gui").get<std::string>();
        cfg.netid        = j.at("netid").get<int64_t>();
        cfg.userhome     = j.at("userhome").get<std::string>();
        cfg.passphrase   = j.at("passphrase").get<std::string>();
        cfg.rpc_password = j.at("rpc_password").get<std::string>();
    }

    inline void
    to_json(json& j, const mm2_config& cfg)
    {
        j                 = json::object();
        j["gui"]          = cfg.gui;
        j["netid"]        = cfg.netid;
        j["userhome"]     = cfg.userhome;
        j["passphrase"]   = cfg.passphrase;
        j["rpc_password"] = cfg.rpc_password;
    }
} // namespace atomic_dex
