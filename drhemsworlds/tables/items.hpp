#pragma once
#include <eosio/eosio.hpp>

using namespace eosio;

//Scoped by user.value
struct [[eosio::contract("drhemsworlds"), eosio::table]] item
{
    uint64_t asset_id;
    uint32_t template_id;
    name type;

    uint64_t primary_key() const { return asset_id; };
};

using items = multi_index<name("items"), item>;