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

//! Project Headers
#include "atomic.dex.security.hpp"
#include "atomic.dex.mm2.error.code.hpp"


namespace atomic_dex
{
    t_password_key
    derive_password(const std::string& password, std::error_code& ec)
    {
        LOG_SCOPE_FUNCTION(INFO);
        std::array<unsigned char, g_salt_len> salt{};
        std::array<unsigned char, g_key_len>  generated_crypto_key{};

        sodium_memzero(salt.data(), salt.size());
        // randombytes_buf(salt.data(), salt.size());

        if (crypto_pwhash(
                generated_crypto_key.data(), generated_crypto_key.size(), password.c_str(), password.size(), salt.data(), crypto_pwhash_OPSLIMIT_INTERACTIVE,
                crypto_pwhash_MEMLIMIT_INTERACTIVE, crypto_pwhash_ALG_DEFAULT) != 0)
        {
            ec = dextop_error::derive_password_failed;
            return generated_crypto_key;
        }
        LOG_F(INFO, "Key generated successfully");

        return generated_crypto_key;
    }

    void
    encrypt(const std::filesystem::path& target_path, const char* mnemonic, const unsigned char* key)
    {
        LOG_SCOPE_FUNCTION(INFO);

        std::array<unsigned char, g_chunk_size>     buf_in{};
        std::array<unsigned char, g_buff_len>       buf_out{};
        std::array<unsigned char, g_header_size>    header{};
        crypto_secretstream_xchacha20poly1305_state st;
        std::ofstream                               fp_t(target_path, std::ios::binary);
        unsigned long long                          out_len;
        unsigned char                               tag;
        std::stringstream                           mnemonic_ss;

        mnemonic_ss << mnemonic;
        crypto_secretstream_xchacha20poly1305_init_push(&st, header.data(), key);
        fp_t.write(reinterpret_cast<const char*>(header.data()), header.size());
        do
        {
            mnemonic_ss.read(reinterpret_cast<char*>(buf_in.data()), buf_in.size());
            tag = mnemonic_ss.eof() ? crypto_secretstream_xchacha20poly1305_TAG_FINAL : 0;
            crypto_secretstream_xchacha20poly1305_push(&st, buf_out.data(), &out_len, buf_in.data(), mnemonic_ss.gcount(), nullptr, 0, tag);
            fp_t.write(reinterpret_cast<const char*>(buf_out.data()), (size_t)out_len);
        } while (not mnemonic_ss.eof());
    }

    std::string
    decrypt(const std::filesystem::path& encrypted_file_path, const unsigned char* key, std::error_code& ec)
    {
        LOG_SCOPE_FUNCTION(INFO);

        std::array<unsigned char, g_buff_len>       buf_in{};
        std::array<unsigned char, g_chunk_size>     buf_out{};
        std::array<unsigned char, g_header_size>    header{};
        std::stringstream                           out;
        crypto_secretstream_xchacha20poly1305_state st;
        std::ifstream                               fp_s(encrypted_file_path, std::ios::binary);
        unsigned long long                          out_len;
        unsigned char                               tag;

        fp_s.read(reinterpret_cast<char*>(header.data()), header.size());
        if (crypto_secretstream_xchacha20poly1305_init_pull(&st, header.data(), key) != 0)
        {
            ec = dextop_error::wrong_password;
            return "";
        }
        do
        {
            fp_s.read(reinterpret_cast<char*>(buf_in.data()), buf_in.size());
            if (crypto_secretstream_xchacha20poly1305_pull(&st, buf_out.data(), &out_len, &tag, buf_in.data(), fp_s.gcount(), nullptr, 0) != 0)
            {
                ec = dextop_error::corrupted_file_or_wrong_password;
                return "";
            }
            if (tag == crypto_secretstream_xchacha20poly1305_TAG_FINAL && not fp_s.eof())
            {
                ec = dextop_error::corrupted_file_or_wrong_password;
                return "";
            }
            out.write(reinterpret_cast<const char*>(buf_out.data()), out_len);
        } while (not fp_s.eof());

        return out.str();
    }
} // namespace atomic_dex
