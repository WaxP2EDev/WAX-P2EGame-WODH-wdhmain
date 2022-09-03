#pragma once
#include <eosio/eosio.hpp>

using namespace eosio;

//Scoped by user.value
struct [[eosio::contract("drhemsworlds"), eosio::table]] seed
{
    uint64_t asset_id;
    uint64_t used;

    uint64_t primary_key() const { return asset_id; };
};

using seeds = multi_index<name("seeds"), seed>;
