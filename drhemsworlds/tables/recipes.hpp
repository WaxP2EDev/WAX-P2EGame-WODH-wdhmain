#pragma once
#include <eosio/eosio.hpp>

using namespace eosio;

struct [[eosio::contract("drhemsworlds"), eosio::table]] recipe
{
    uint64_t recipe_id;
    string description;
    int32_t template_build;
    int32_t template_craft;
    int8_t number_assets;

    uint64_t primary_key() const { return recipe_id; };
};

typedef multi_index<name("recipes"), recipe> recipes_t;