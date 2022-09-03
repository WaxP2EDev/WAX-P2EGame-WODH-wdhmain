#pragma once
#include <eosio/eosio.hpp>

using namespace eosio;

struct [[eosio::contract("drhemsworlds"), eosio::table]] ltool
{
    name user;
    uint64_t tool1;
    uint64_t tool2;

    uint64_t primary_key() const { return user.value; };
};

using ltools = multi_index<name("ltools"), ltool>;
