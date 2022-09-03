#pragma once
#include <eosio/eosio.hpp>

using namespace eosio;

//Scope: collection_name
struct [[eosio::table]] schemas_s
{
    name schema_name;
    vector<atomicdata::FORMAT> format;

    uint64_t primary_key() const { return schema_name.value; }
};

typedef multi_index<name("schemas"), schemas_s> schemas_t;
