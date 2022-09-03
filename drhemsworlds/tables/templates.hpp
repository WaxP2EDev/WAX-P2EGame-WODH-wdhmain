#pragma once
#include <eosio/eosio.hpp>

using namespace eosio;

//Scope: collection_name
struct [[eosio::table]] templates_s
{
    int32_t template_id;
    name schema_name;
    bool transferable;
    bool burnable;
    uint32_t max_supply;
    uint32_t issued_supply;
    vector<uint8_t> immutable_serialized_data;

    uint64_t primary_key() const { return (uint64_t)template_id; }
};

typedef multi_index<name("templates"), templates_s> templates_t;