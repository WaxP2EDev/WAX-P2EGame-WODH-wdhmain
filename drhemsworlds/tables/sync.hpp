#pragma once
#include <eosio/eosio.hpp>

using namespace eosio;

//Scoped by user.value
struct [[eosio::contract("drhemsworlds"), eosio::table]] syn
{
    name key;
    uint32_t ratio;

    uint64_t primary_key() const { return  key.value; };
};

using sync = multi_index<name("sync"), syn>;
