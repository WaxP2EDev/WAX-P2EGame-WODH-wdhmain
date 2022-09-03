#pragma once
#include <eosio/eosio.hpp>

using namespace eosio;

//Scoped by user.value
struct [[eosio::contract("drhemsworlds"), eosio::table]] avatar
{
    uint64_t asset_id;
    time_point_sec last_used_date;

    uint64_t primary_key() const { return asset_id; };
};

using avatars = multi_index<name("avatars"), avatar>;