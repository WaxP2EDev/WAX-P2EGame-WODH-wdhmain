#pragma once
#include <eosio/eosio.hpp>

using namespace eosio;

//Scoped by user.value
struct [[eosio::contract("drhemsworlds"), eosio::table]] land
{
    uint64_t asset_id;
    name owner;
    uint32_t comission;
    uint32_t usage;
    // uint32_t template_id;

    uint64_t primary_key() const { return asset_id; };
    // uint64_t owner_key3() const { return  owner.value; };
};

// using by_owner_key3 = indexed_by<name("byowner3"), const_mem_fun<land, uint64_t, &land::owner_key3>>;
using lands = multi_index<name("lands"), land>;
