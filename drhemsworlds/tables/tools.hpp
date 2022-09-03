#pragma once
#include <eosio/eosio.hpp>

using namespace eosio;

//Scoped by user.value
struct [[eosio::contract("drhemsworlds"), eosio::table]] tool
{
    uint64_t asset_id;
    time_point_sec last_used_date;

    uint64_t primary_key() const { return asset_id; };
};

using tools = multi_index<name("tools"), tool>;