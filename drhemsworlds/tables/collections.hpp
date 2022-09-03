#pragma once
#include <eosio/eosio.hpp>

using namespace eosio;

struct [[eosio::table]] collections_s
{
    name collection_name;
    name author;
    bool allow_notify;
    vector<name> authorized_accounts;
    vector<name> notify_accounts;
    double market_fee;
    vector<uint8_t> serialized_data;

    uint64_t primary_key() const { return collection_name.value; };
};

typedef multi_index<name("collections"), collections_s> collections_t;