#pragma once
#include <eosio/eosio.hpp>

using namespace eosio;

//Scoped by user.value
struct [[eosio::contract("drhemsworlds"), eosio::table]] planet
{
    uint64_t template_id;
    uint32_t avatar1;
    uint32_t avatar2;
    uint32_t avatar3;

    uint64_t primary_key() const { return template_id; };
};

using planets = multi_index<name("planets"), planet>;
