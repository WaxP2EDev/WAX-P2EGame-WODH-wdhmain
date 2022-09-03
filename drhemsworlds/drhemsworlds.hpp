#include <math.h>
/*#include <string>*/
#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/crypto.hpp>
#include <eosio/transaction.hpp>
#include "resources.hpp"
#include "const_factors.hpp"
// #include "const_tool_syn.hpp"

#include "const_prizes.hpp"
// #include "const_tool_groups.hpp"
// #include "const_prizes_test.hpp"
// #include "const_tool_groups_test.hpp"

#include "../atomicassets/atomicdata.hpp"
#include "lands.hpp"
#include "avatars.hpp"
#include "assets.hpp"
#include "sync.hpp"
#include "lfactors.hpp"
// #include "collections.hpp"
#include "config.hpp"
#include "factors.hpp"
#include "items.hpp"
#include "players.hpp"
#include "penalty.hpp"
#include "planets.hpp"
// #include "prizes.hpp"
#include "ltools.hpp"
// #include "prodplugin1.hpp"
#include "schemas.hpp"
#include "seeds.hpp"
// #include "spins.hpp"
#include "templates.hpp"
#include "recipes.hpp"
#include "tools.hpp"
#include "water.hpp"
#include "b64.h"
#include "vigenere.h"

using namespace eosio;
using namespace std;
using namespace atomicdata;

const std::vector<FORMAT> toolFormat = {
  {"name","string"},
  {"img","image"},
  {"rarity","string"},
  {"type","string"},
  {"cardid","uint64"},
  {"description","string"}
};

class [[eosio::contract("drhemsworlds")]] drhemsworlds : public contract
{
public:
   drhemsworlds(name receiver, name code, datastream<const char *> ds);

   //For managing recipes
   [[eosio::action("addrecipe")]] void addrecipe(uint64_t recipe_id, string description, int32_t template_build, int8_t number_assets, int32_t template_craft);

   [[eosio::action("updaterecipe")]] void updaterecipe(uint64_t recipe_id, string description, int32_t template_build, int8_t number_assets, int32_t template_craft);

   //For managing config
   [[eosio::action("tutupdcfg")]] void update_config(const asset& global_point_factor, const asset& global_max_time_factor,
                                                     const asset& global_time_factor, const asset& water_price, const name& mode,
                                                     const name& current_wheel, const asset& wheel_cost);

   [[eosio::action("tutupdfactor")]] void update_factor(const uint64_t& template_id, const string& data);

   // [[eosio::action("updateland")]] void update_land(const uint64_t& asset_id, const name& owner, const uint32_t& comission);
   // [[eosio::action("updatelastt")]] void update_last_tool(const name& user, uint64_t& tool1, uint64_t& tool2);
   [[eosio::action("updatesync")]] void update_sync(const name& key, uint32_t& ratio);
   [[eosio::action("updateplnt")]] void update_planet(const uint64_t& template_id, const uint32_t& avatar1, const uint32_t& avatar2, const uint32_t& avatar3);

   //For production
   [[eosio::action("tutclaim")]] void claim(const name& user);

   // [[eosio::action("tutsetpoints")]] void set_game_points(const name& user, const asset& quantity);

   [[eosio::action("tutsetprizew")]] void set_prize_won(const name& user, const uint32_t& prize);

   [[eosio::action("tutsetwtrlvl")]] void set_water_level(const name& user, const asset& quantity);

   [[eosio::action("tutunlock")]] void unlock(const name& user, const name& state);

   [[eosio::action("tutbuywater")]] void buy_water(const name& owner, const asset& quantity);

   [[eosio::action("tutgetwater")]] void get_water(const name& owner);

   [[eosio::action("tutcnlprod")]] void cancel_production(const name& user);

   [[eosio::action("tutrtkavtrs")]] void retake_avatars(const name& user, const std::vector<uint64_t>& asset_ids);

   [[eosio::action("tutrtktools")]] void retake_tools(const name& user, const std::vector<uint64_t>& asset_ids);

   [[eosio::action("tutrmvitems")]] void remove_items(const name& user);

   [[eosio::action("tutrmvspin")]] void remove_spin(const uint64_t& id);

   // [[eosio::action("tutrmvprizes")]] void remove_prizes(const name& wheel);

   [[eosio::action("tutrmvfctrs")]] void remove_factors();

   [[eosio::action("tutrmvcfg")]] void remove_config();

   //For managing lucky wheel
   [[eosio::action("tutrunwheel")]] void run_wheel(const name& owner);

   [[eosio::action("tutclmprize")]] void claim_prize(const name& owner);

   [[eosio::action("tutmisc")]] void misc();

   //For WAX RNG
   [[eosio::action("receivelf")]] void receiveLandFactor(const name& account, const asset& factor);
   [[eosio::action("receiverand")]] void receiverand(const uint64_t& assoc_id, const checksum256& random_value);
   [[eosio::action("setlandfee")]] void set_land_fee(const name& owner, const uint64_t& asset_id, const uint32_t& fee);
   [[eosio::action("setlandu")]] void set_land_usage(const uint64_t& asset_id, const uint32_t& usage);

   [[eosio::action("farm")]] void farming_with_existing_items(const name& from, const std::vector<uint64_t>& items_ids, const uint64_t& land_id, const name& land_owner);

   [[eosio::on_notify("atomicassets::transfer")]] void on_transfer(const name& from, const name& to, const std::vector<uint64_t>& asset_ids, const std::string& memo);

   // using addrecipe_action = action_wrapper<name("addrecipe"), &drhemsworlds::addrecipe>;
   // using updaterecipe_action = action_wrapper<name("updaterecipe"), &drhemsworlds::updaterecipe>;

private:
   void add_water_balance(const name &owner, const asset &value);
   void sub_water_balance(const name &owner, const asset &value);

   // void add_game_balance(const name &owner, const asset &value);
   void sub_game_balance(const name &owner, const asset &value);

   void add_avatar_item(const name &owner, const uint64_t &asset_id);
   void add_tool_item(const name &owner, const uint64_t &asset_id);

   uint64_t to_random_value(const checksum256& random_value);
   std::map<std::string, std::string> to_key_value(const std::string &memo);
   std::vector<std::string> split(const std::string &s, const std::string &delimiter);
   std::vector<uint64_t> to_uint64_ids(const std::vector<std::string> &str);
   time_point_sec to_current_day(const time_point_sec& date);

   config get_config();
   uint64_t get_signing_value();
   int32_t get_template_id(const name& owner, const uint64_t& asset_id);
   name get_schema_name(const name& collection, const int32_t& template_id);
   game_factor get_factor(const int32_t& template_id);
   double get_tool_syn_value(string key1,string key2);
   int32_t get_random_prize(const name& wheel, const uint64_t& random_value);

   void craft(name _owner, name _self, vector<uint64_t> asset_ids, string memo);
   void farming(const name& from, const name& to, const std::vector<uint64_t>& asset_ids, const std::string& memo);
   void farming_with_part_existing_items(const name& from, const name& to, const std::vector<uint64_t>& asset_ids, const std::string& memo);

   bool is_prize_won(const name& owner);
   bool is_spin_exist(const name& owner);
   bool is_valid_state(const name& state);
   bool is_new_prod_memo(const std::string& memo);
   bool is_part_exst_prod_memo(const std::string& memo);
   bool is_transfer_memo(const std::string& memo);
   bool is_digit(const std::string &str);
   bool is_digit(const std::vector<std::string> &str);

   bool is_ready_state(const name& user);
   bool is_seed_exist(const name& owner, const uint64_t& asset_id);
   bool is_valid_factor_type(const name& type);

   std::tuple<bool, std::vector<uint64_t>, std::vector<string>>
  is_valid_part_existing_items_farm_memo(const std::string &memo);

   void send_requestrand(const uint64_t& assoc_id, const uint64_t& signing_value, const name& caller);
   void send_mintasset(const name& authorized_minter, const name& collection_name, const name& schema_name,
                       const int32_t& template_id, const name& new_asset_owner, const ATTRIBUTE_MAP& immutable_data,
                       const ATTRIBUTE_MAP& mutable_data, const std::vector<asset>& tokens_to_back);

   void send_burnasset(const name& asset_owner, const uint64_t& asset_id);
   void send_transfer(const name& from, const name& to, const std::vector<uint64_t>& asset_ids, const std::string& memo);

// /*   void reset_prodplugin1(const name &owner);
   void send_penalty(const name &owner, const bool& tool1, const int32_t& template_id);
   void send_nft_chance(const name &owner, const int32_t& template_id);
   double get_land_avatar(const uint64_t& land_id, const uint32_t& avatar_id);
   asset calculate_water_price(const name& user, const int32_t& amount);
   void set_last_tool(const name& user, const uint64_t& tool1, const uint64_t& tool2);
   void send_lfactor(const name& account, const uint64_t& landId, const uint64_t& avatarId, const std::vector<uint64_t>& tools, const uint64_t& seedId);
   // int32_t get_random_pass(const uint64_t& random_value, uint32_t template_id);*/
};
