#include "drhemsworlds.hpp"

std::string decrypt(std::string& encrypted_msg, std::string& key) {
	std::string newKey = extend_key(encrypted_msg, key);
	std::string b64_encoded_str = decrypt_vigenere(encrypted_msg, newKey);
	std::string b64_decode_str = base64_decode(b64_encoded_str);
	return b64_decode_str;
}

drhemsworlds::drhemsworlds(name receiver, name code, datastream<const char *> ds)
  : contract::contract(receiver, code, ds)
{
}

void drhemsworlds::send_lfactor(const name& account, const uint64_t& landId, const uint64_t& avatarId, const std::vector<uint64_t>& tools, const uint64_t& seedId){
	action(
      permission_level{get_self(), "active"_n},
      LANDS_CONTRACT,
      "lfactor"_n,
      std::make_tuple(account, landId, avatarId, tools, seedId))
      .send();
}

void drhemsworlds::set_land_fee(const name& owner, const uint64_t& asset_id, const uint32_t& fee){
	check(has_auth(get_self()) || has_auth(owner), "Unauthorized access");

	check(fee >= 5 && fee <= 100, "Invalid fee");

	assets_t assets_table("atomicassets"_n, owner.value);
	assets_table.get(asset_id, "no nft found");

	lands _lands(get_self(), get_self().value);
  auto it = _lands.find(asset_id);

  if(it == _lands.end())
  {
    _lands.emplace(get_self(), [&](auto &a) {
      a.asset_id = asset_id;
      a.owner = owner;
      a.comission = fee;
      a.usage = 0;
    });
  }
  else
  {
    _lands.modify(it, same_payer, [&](auto &a) {
			a.comission = fee;
      a.owner = owner;
    });
  }
}

void drhemsworlds::set_land_usage(const uint64_t& asset_id, const uint32_t& usage){
	check(has_auth(get_self()), "Unauthorized access");

	lands _lands(get_self(), get_self().value);
  auto it = _lands.find(asset_id);
	check(it != _lands.end(), "No land with asset id");


  _lands.modify(it, same_payer, [&](auto &a) {
		a.usage = usage;
  });

}

void drhemsworlds::receiveLandFactor(const name& account, const asset& factor){
  check(has_auth(get_self()) || has_auth(LANDS_CONTRACT), "Unauthorized access");

	lfactors _factors(get_self(), get_self().value);
  auto it = _factors.find(account.value);

	if(it == _factors.end())
  {
    _factors.emplace(get_self(), [&](auto &a) {
      a.player = account;
      a.factor = factor;
    });
  }
  else
  {
    _factors.modify(it, same_payer, [&](auto &a) {
      a.factor = factor;
    });
  }
}

void drhemsworlds::addrecipe(uint64_t recipe_id, string description, int32_t template_build, int8_t number_assets, int32_t template_craft)
{
  require_auth(get_self());
  recipes_t recipes(get_self(), get_self().value);
  auto itrRecipe = recipes.find(recipe_id);
  check(itrRecipe == recipes.end(), "That recipe already created try other recipe_id");

  recipes.emplace(_self, [&](auto &rec) {
    rec.recipe_id = recipe_id;
    rec.description = description;
    rec.template_build = template_build;
    rec.template_craft = template_craft;
    rec.number_assets = number_assets;
  });
}

void drhemsworlds::updaterecipe(uint64_t recipe_id, string description, int32_t template_build, int8_t number_assets, int32_t template_craft)
{
  require_auth(get_self());
  recipes_t recipes(get_self(), get_self().value);
  auto itrRecipe = recipes.find(recipe_id);
  check(itrRecipe != recipes.end(), "That recipe already created try other recipe_id");

  recipes.modify(itrRecipe, _self, [&](auto &rec) {
    rec.recipe_id = recipe_id;
    rec.description = description;
    rec.template_build = template_build;
    rec.template_craft = template_craft;
    rec.number_assets = number_assets;
  });
}

void drhemsworlds::update_planet(const uint64_t& template_id, const uint32_t& avatar1, const uint32_t& avatar2, const uint32_t& avatar3){
	require_auth(get_self());

  planets _factors(get_self(), get_self().value);
  auto it = _factors.find(template_id);

  if(it == _factors.end())
  {
    _factors.emplace(get_self(), [&](auto &a) {
      a.template_id = template_id;
      a.avatar1 = avatar1;
      a.avatar2 = avatar2;
      a.avatar3 = avatar3;
    });
  }
  else
  {
    _factors.modify(it, same_payer, [&](auto &a) {
			a.avatar1 = avatar1;
      a.avatar2 = avatar2;
      a.avatar3 = avatar3;
    });
  }
}

void drhemsworlds::update_factor(const uint64_t& template_id, const string& data){
  require_auth(get_self());

  factors _factors(get_self(), get_self().value);
  auto it = _factors.find(template_id);

  if(it == _factors.end())
  {
    _factors.emplace(get_self(), [&](auto &a) {
      a.template_id = template_id;
      a.data = data;
    });
  }
  else
  {
    _factors.modify(it, same_payer, [&](auto &a) {
      a.data = data;
    });
  }
}

void drhemsworlds::update_config(const asset& global_point_factor, const asset& global_max_time_factor,
                                 const asset& global_time_factor, const asset& water_price, const name& mode,
                                 const name& current_wheel, const asset& wheel_cost)
{
  require_auth(get_self());

  check(global_point_factor.symbol == POINTS_SYMBOL, "update_config : invalid global point factor symbol or precision");
  check(global_point_factor.amount > 0, "update_config : global point factor amount should be positive");
  check(global_max_time_factor.symbol == MULTI_SYMBOL, "update_config : invalid global max time factor symbol or precision");
  check(global_max_time_factor.amount > 0, "update_config : global max time factor amount should be positive");
  check(global_time_factor.symbol == MULTI_SYMBOL, "update_config : invalid global time factor symbol or precision");
  check(global_time_factor.amount > 0, "update_config : global time factor amount should be positive");
  check(water_price.symbol == HEMS_SYMBOL, "update_config : invalid water symbol or precision");
  check(water_price.amount > 0, "update_config : time factor amount should be positive");
  check(!current_wheel.to_string().empty(), "update_config : invalid current_wheel name");
  check(wheel_cost.symbol == HEMS_SYMBOL, "update_config : invalid wheel cost symbol or precision");
  check(wheel_cost.amount > 0, "update_config : wheel cost amount should be positive");

  config_table _config_table(get_self(), get_self().value);
  auto obj = _config_table.get_or_create(get_self());

  obj.global_point_factor = global_point_factor;
  obj.global_max_time_factor = global_max_time_factor;
  obj.global_time_factor = global_time_factor;
  obj.water_price = water_price;
  obj.mode = mode;
  obj.current_wheel = current_wheel;
  obj.wheel_cost = wheel_cost;

  _config_table.set(obj, get_self());
}

void drhemsworlds::claim(const name& user)
{
  require_auth(user);

  players _players(get_self(), get_self().value);

  auto it = _players.find(user.value);
  check(it != _players.end(), "claim : player is not exist");
  check(it->production_state == PROD, "claim : player stata should be prod");
  check(it->production_end < current_time_point(), "claim : production end time is not expired");

  items _items(get_self(), user.value);
  seeds _seeds(get_self(), user.value);
  // std::vector<uint64_t> asset_ids;

	uint32_t landFee = 5;
	name landOwner;
	bool landListed = false;

  auto items_itr = _items.begin();
  while(items_itr != _items.end())
  {
    if(items_itr->type == name("SEED"))
    {
      auto seed_itr = _seeds.find(items_itr->asset_id);

      if(seed_itr == _seeds.end())
      {
        _seeds.emplace(get_self(), [&](auto &r) {
            r.asset_id = items_itr->asset_id;
            r.used = 1;
        });
      }
      else
      {
        _seeds.modify(seed_itr, get_self(), [&](auto &r) {
            r.used += 1;
        });
      }

      auto factor = get_factor(items_itr->template_id);
      const auto& obj = _seeds.get(items_itr->asset_id, "no seed object found");

      if (obj.used >= factor.max_usage)
      {
        send_burnasset(get_self(), obj.asset_id);
        _seeds.erase(obj);
      }
    }else if(items_itr->type == name("LAND"))
    {

			lands _lands(get_self(), get_self().value);
			auto itLand = _lands.find(items_itr->asset_id);

			if(itLand != _lands.end())
			{
				landFee = itLand->comission;
				landOwner = itLand->owner;
				_lands.modify(itLand, same_payer, [&](auto &a) {
		       a.usage -= 1;
		   	});
				if(landOwner.value != name("").value){
					assets_t assets_table("atomicassets"_n, landOwner.value);
					auto assItr = assets_table.find(items_itr->asset_id);
					if(assItr != assets_table.end()){
						landListed = true;
					}
				}
			}
		}

    items_itr = _items.erase(items_itr);
  }

  auto points_to_add = it->points_to_add;

	lfactors _factors(get_self(), get_self().value);
  auto itLFactor = _factors.find(user.value);

	double lfactor = 1.0;

	if(itLFactor != _factors.end()){
		lfactor = (double) itLFactor->factor.amount / 10000;
	}

	int64_t earnAmount = ceil((double)it->points_to_add.amount*lfactor*100);
	int64_t comissionAmount = ceil((double)earnAmount*((double)landFee/100));
	earnAmount -= comissionAmount;

  _players.modify(it, same_payer, [&](auto &a) {
      a.production_state = LOCKED;
  });

	penalty _spins(get_self(), get_self().value);
  auto index = _spins.get_index<name("byowner2")>();
  auto itNft = index.find(user.value);
	while(itNft != index.end()){
		if(itNft->wheel.value == NFT_CHANCE_WHEEL.value && itNft->penalty_factor > 0){
			auto schema = get_schema_name(COLLECTION_NAME, itNft->penalty_factor);
	    send_mintasset(get_self(), COLLECTION_NAME, schema, itNft->penalty_factor, user, ATTRIBUTE_MAP(), ATTRIBUTE_MAP(), std::vector<asset>());
		}
		if(itNft->wheel.value == NFT_CHANCE_WHEEL.value){
			index.erase(itNft);
			break;
		}

		itNft++;
	}

	action(
      permission_level{get_self(), "active"_n},
      INGAME_CONTRACT,
      "earn"_n,
      std::make_tuple(user, earnAmount))
      .send();

	if(landListed){
		action(
	      permission_level{get_self(), "active"_n},
	      INGAME_CONTRACT,
	      "earn"_n,
	      std::make_tuple(landOwner, comissionAmount))
	      .send();
	}


  // send_transfer(get_self(), user, asset_ids, "");
}

// void drhemsworlds::set_game_points(const name& user, const asset& quantity)
// {
//   require_auth(get_self());
//   check(quantity.symbol == POINTS_SYMBOL, "set_game_points : invalid quantity symbol or precision");
//   check(quantity.amount >= 0, "set_game_points : quantity amount should be greater or equal to 0");
//
//   players _players(get_self(), get_self().value);
//   auto it = _players.find(user.value);
//   check(it != _players.end(), "set_game_points : no player found");
//
//   _players.modify(it, same_payer, [&](auto &a) {
//       a.game_balance = quantity;
//   });
// }

void drhemsworlds::set_prize_won(const name& user, const uint32_t& prize)
{
  require_auth(get_self());

  players _players(get_self(), get_self().value);
  auto it = _players.find(user.value);
  check(it != _players.end(), "set_game_points : no player found");

  _players.modify(it, same_payer, [&](auto &a) {
      a.prize_won = prize;
  });
}

void drhemsworlds::set_water_level(const name& user, const asset& quantity)
{
  require_auth(get_self());

  check(quantity.symbol == WATER_SYMBOL, "set_water_level : invalid quantity symbol or precision");
  check(quantity.amount >= 0, "set_water_level : quantity amount should be greater or equal to 0");
  check(quantity <= MAX_WATER_LEVEL, "set_water_level : quantity should be less or equal 100 WATER");

  players _players(get_self(), get_self().value);
  auto it = _players.find(user.value);
  check(it != _players.end(), "set_water_level : no player found");

  _players.modify(it, same_payer, [&](auto &a) {
    a.water_level = quantity;
  });
}

void drhemsworlds::unlock(const name& user, const name& state)
{
  require_auth(get_self());
  check(is_valid_state(state), "unlock : invalid state");

  players _players(get_self(), get_self().value);
  auto it = _players.find(user.value);
  check(it != _players.end(), "unlock : no player exist");

  _players.modify(it, same_payer, [&](auto &a) {
      a.production_state = state;
  });
}

void drhemsworlds::buy_water(const name& owner, const asset& quantity)
{
  require_auth(owner);
  check(quantity.amount > 0, "buy_water : quantity should be positive");
  check(quantity.amount <= 100, "buy_water : quantity amount should be less or equal to 100");
  auto cfg = get_config();

  sub_game_balance(owner, calculate_water_price(owner, quantity.amount)); // DIKKAT
  add_water_balance(owner, quantity);

  water_table _water_table(get_self(), get_self().value);
  auto it = _water_table.find(owner.value);

  if(it == _water_table.end())
  {
    _water_table.emplace(get_self(), [&](auto &r) {
      r.user = owner;
      r.last_update = current_time_point();
    });
  }
  else
  {
    auto exp_date = it->last_update + days(1);
    check(exp_date < current_time_point() , "buy_water : period is not expired");

    _water_table.modify(it, same_payer, [&](auto &r) {
      r.last_update = current_time_point();
    });
  }
}

void drhemsworlds::get_water(const name& owner)
{
  require_auth(owner);

  freewater_table _freewater_table(get_self(), get_self().value);
  auto it = _freewater_table.find(owner.value);

  if(it == _freewater_table.end())
  {
    _freewater_table.emplace(get_self(), [&](auto &r) {
      r.user = owner;
      r.last_update = current_time_point();
    });
  }
  else
  {
    auto edge_date = to_current_day(it->last_update) + days(1);
    check(it->last_update <= edge_date && edge_date <= current_time_point() , "get_water : period is not expired");

    _freewater_table.modify(it, same_payer, [&](auto &r) {
      r.last_update = current_time_point();
    });
  }

  players _players(get_self(), get_self().value);
  auto itr = _players.find(owner.value);
  check(itr != _players.end(), "get_water : player is not exist");

  _players.modify(itr, same_payer, [&](auto &a) {
      a.water_level = MAX_WATER_LEVEL;
  });
}

void drhemsworlds::cancel_production(const name& user)
{
  require_auth(user);

  players _players(get_self(), get_self().value);
  auto it = _players.find(user.value);
  check(it != _players.end(), "cancel_production : player is not exist");
  check(it->production_state == PROD, "cancel_production : player stata should be prod");

  items _items(get_self(), user.value);
  seeds _seeds(get_self(), user.value);
  // std::vector<uint64_t> asset_ids;

  auto items_itr = _items.begin();
  while(items_itr != _items.end())
  {
    if(items_itr->type == SEED)
    {
      auto seed_itr = _seeds.find(items_itr->asset_id);

      if(seed_itr == _seeds.end())
      {
        _seeds.emplace(get_self(), [&](auto &r) {
            r.asset_id = items_itr->asset_id;
            r.used = 1;
        });
      }
      else
      {
        _seeds.modify(seed_itr, get_self(), [&](auto &r) {
            r.used += 1;
        });
      }

      auto factor = get_factor(items_itr->template_id);
      const auto& obj = _seeds.get(items_itr->asset_id, "no seed object found");

      if (obj.used >= factor.max_usage)
      {
        send_burnasset(get_self(), obj.asset_id);
        _seeds.erase(obj);
      }
    }else if(items_itr->type == LAND)
    {

			lands _lands(get_self(), get_self().value);
			auto itLand = _lands.find(items_itr->asset_id);

			if(itLand != _lands.end())
			{
				_lands.modify(itLand, same_payer, [&](auto &a) {
		       a.usage -= 1;
		   	});
			}
		}

    items_itr = _items.erase(items_itr);
  }

  _players.modify(it, same_payer, [&](auto &a) {
      a.production_state = LOCKED;
      a.production_end = time_point_sec();
  });

	penalty _spins(get_self(), get_self().value);
  auto index = _spins.get_index<name("byowner2")>();
  auto itNft = index.find(user.value);
	while(itNft != index.end()){
		if(itNft->wheel.value == NFT_CHANCE_WHEEL.value){
			index.erase(itNft);
			break;
		}
		itNft++;
	}

  // send_transfer(get_self(), user, asset_ids, "");
}

void drhemsworlds::retake_avatars(const name& user, const std::vector<uint64_t>& asset_ids)
{
  require_auth(user);
  avatars _avatars(get_self(), user.value);

  for(auto id : asset_ids)
  {
    auto it = _avatars.find(id);
    check(it != _avatars.end(), "retake_avatars : avatar is not exist");
    check(it->last_used_date + hours(24).to_seconds() < current_time_point(), "retake_avatars : production date is not expired");
    _avatars.erase(it);
  }

  send_transfer(get_self(), user, asset_ids, "");
}

void drhemsworlds::retake_tools(const name& user, const std::vector<uint64_t>& asset_ids)
{
  require_auth(user);
  tools _tools(get_self(), user.value);

  for(auto id : asset_ids)
  {
    auto it = _tools.find(id);
    check(it != _tools.end(), "retake_tools : tool is not exist");
    check(it->last_used_date + hours(24).to_seconds() < current_time_point(), "retake_tools : production date is not expired");
    _tools.erase(it);
  }

  send_transfer(get_self(), user, asset_ids, "");
}

void drhemsworlds::remove_items(const name& user)
{
  require_auth(get_self());

  items _items(get_self(), user.value);
  check(_items.begin() != _items.end(), "remove_items : no items found");
  auto it = _items.begin();

  while(it != _items.end())
  {
    it = _items.erase(it);
  }
}

void drhemsworlds::misc()
{
  require_auth(get_self());

  lands _items(get_self(), get_self().value);
  auto it = _items.begin();

	int delCnt = 0;

  while(it != _items.end() && delCnt < 50)
  {
		delCnt++;
    it = _items.erase(it);
		if(it != _items.end()) it++;
	}
}

void drhemsworlds::remove_spin(const uint64_t& id)
{
  require_auth(get_self());

  penalty _spins(get_self(), get_self().value);
  auto it = _spins.find(id);
  check(it != _spins.end(), "remove_spin : spin is not exist");
  _spins.erase(it);
}

// void drhemsworlds::remove_prizes(const name& wheel)
// {
//   require_auth(get_self());
//
//   prizes _prizes(get_self(), wheel.value);
//   auto it = _prizes.begin();
//   while(it != _prizes.end())
//   {
//     it = _prizes.erase(it);
//   }
// }

void drhemsworlds::remove_factors()
{
  require_auth(get_self());

  factors _factors(get_self(), get_self().value);
  auto it = _factors.begin();
  while(it != _factors.end())
  {
    it = _factors.erase(it);
  }
}

void drhemsworlds::remove_config()
{
  require_auth(get_self());

  config_table _config_table(get_self(), get_self().value);
  _config_table.remove();
}

void drhemsworlds::run_wheel(const name& owner)
{
  require_auth(owner);
  check(!is_prize_won(owner), "run_wheel : claim prize before run wheel");
  check(!is_spin_exist(owner), "run_wheel : active spin exist");

  auto config = get_config();
  sub_game_balance(owner, config.wheel_cost);

  penalty _spins(get_self(), get_self().value);
  auto id = _spins.available_primary_key();

  _spins.emplace(_self, [&](auto &r) {
    r.id = id;
    r.wheel = config.current_wheel;
    r.owner = owner;
  });

  auto sig_value = get_signing_value();
  send_requestrand(id, sig_value, get_self());
}

void drhemsworlds::claim_prize(const name& owner)
{
  require_auth(owner);

  players _players(get_self(), get_self().value);
  auto it = _players.find(owner.value);
  check(it != _players.end(), "claim_prize : no user found");
  check(it->prize_won != 0, "claim_prize : no prize won");
  auto template_id = it->prize_won;

  _players.modify(it, same_payer, [&](auto &r) {
      r.prize_won = 0;
  });

  if(template_id != EXCEPTION_PRIZE_ID)
  {
    auto schema = get_schema_name(COLLECTION_NAME, template_id);
    send_mintasset(get_self(), COLLECTION_NAME, schema, template_id, owner, ATTRIBUTE_MAP(), ATTRIBUTE_MAP(), std::vector<asset>());
  }
}

void drhemsworlds::receiverand(const uint64_t& assoc_id, const checksum256& random_value)
{
  require_auth(ORNG_CONTRACT);

  penalty _spins(get_self(), get_self().value);
  auto it = _spins.find(assoc_id);
  check(it != _spins.end(), "receiverand : no assoc_id found");

  players _players(get_self(), get_self().value);
  const auto& obj = _players.get(it->owner.value, "no player object found");

	/*prodplugin1 _plugin(get_self(), get_self().value);
	auto to = _plugin.find(it->owner.value);
	check(to != _plugin.end(),"no plugin object found");*/



  auto r_value = to_random_value(random_value);
	if(it->wheel.value == TOOL1_PENALTY_WHEEL.value || it->wheel.value == TOOL2_PENALTY_WHEEL.value){
		double tool1Factor = 1.0;
		auto tool1 = get_factor(it->pentemp);
		asset tool1GameFactor = tool1.game_factor;
		tool1Factor = (double)r_value / 10000 * ((double) -tool1.penalty / 100) + 0.1;
		auto cfg = get_config();
		auto points_to_add = obj.points_to_add;
		points_to_add = asset(points_to_add.amount + ceil(cfg.global_point_factor.amount * ((double)tool1GameFactor.amount / 100 * tool1Factor)),POINTS_SYMBOL);
		_players.modify(obj, same_payer, [&](auto &a) {
       a.points_to_add = points_to_add;
   	});
		_spins.erase(it);
	}else if(it->wheel.value == NFT_CHANCE_WHEEL.value){
		auto nftTool = get_factor(it->pentemp);
		if(nftTool.pass1 > 0 && r_value <= nftTool.pass1){
			_spins.modify(it, same_payer, [&](auto &a) {
	       a.penalty_factor = PASS1_TEMPLATE_ID;
	   	});
		}else if(nftTool.pass2 > 0 && r_value <= nftTool.pass2){
			_spins.modify(it, same_payer, [&](auto &a) {
	       a.penalty_factor = PASS2_TEMPLATE_ID;
	   	});
		}else if(nftTool.pass3 > 0 && r_value <= nftTool.pass3){
			_spins.modify(it, same_payer, [&](auto &a) {
	       a.penalty_factor = PASS3_TEMPLATE_ID;
	   	});
		}
	}else{
	  auto template_id = get_random_prize(it->wheel, r_value);

	  _players.modify(obj, same_payer, [&](auto &r) {
	      r.prize_won = template_id;
	  });
		_spins.erase(it);
	}
}

void drhemsworlds::on_transfer(const name& from, const name& to, const std::vector<uint64_t>& asset_ids, const std::string& memo)
{
  if (to != get_self())
    return;
  if (memo.find("craft") != string::npos)
  {
    craft(from, to, asset_ids, memo);
  }
  else if (is_new_prod_memo(memo))
  {
    farming(from, to, asset_ids, memo);
  }
  else if(is_part_exst_prod_memo(memo))
  {
    farming_with_part_existing_items(from, to, asset_ids, memo);
  }
  else if(is_transfer_memo(memo))
  {
  }
  else
  {
     return abort();
  }
}

void drhemsworlds::add_water_balance(const name &owner, const asset &value)
{
  players _players(get_self(), get_self().value);
  auto to = _players.find(owner.value);
  if (to == _players.end())
  {
    check(value <= MAX_WATER_LEVEL, "overflow water level");

    _players.emplace(get_self(), [&](auto &a) {
        a.user = owner;
        a.game_balance = asset(0, POINTS_SYMBOL);
        a.water_level = value;
        a.production_info = string();
        a.production_state = READY;
        a.production_end = time_point_sec();
        a.points_to_add = asset(0, POINTS_SYMBOL);
        a.prize_won = 0;
    });
  }
  else
  {
    check(to->water_level + value <= MAX_WATER_LEVEL, "overflow water level");
    _players.modify(to, same_payer, [&](auto &a) {
        a.water_level += value;
    });
  }
}

void drhemsworlds::sub_water_balance(const name &owner, const asset &value)
{
  players _players(get_self(), get_self().value);
  const auto &from = _players.get(owner.value, "no player object found");
  check(from.water_level.amount >= value.amount, "overdrawn water level");

  _players.modify(from, same_payer, [&](auto &a) {
      a.water_level -= value;
  });
}

// void drhemsworlds::add_game_balance(const name &owner, const asset &value)
// {
//   players _players(get_self(), get_self().value);
//   auto to = _players.find(owner.value);
//   if (to == _players.end())
//   {
//     _players.emplace(get_self(), [&](auto &a) {
//         a.user = owner;
//         a.game_balance = value;
//         a.water_level = MAX_WATER_LEVEL;
//         a.production_info = string();
//         a.production_state = READY;
//         a.production_end = time_point_sec();
//         a.points_to_add = asset(0, POINTS_SYMBOL);
//         a.prize_won = 0;
//     });
//   }
//   else
//   {
//     _players.modify(to, same_payer, [&](auto &a) {
//         a.game_balance += value;
//     });
//   }
// }

void drhemsworlds::sub_game_balance(const name &owner, const asset &value)
{
  players _players(get_self(), get_self().value);
  const auto &from = _players.get(owner.value, "no player object found");

	action(
      permission_level{get_self(), "active"_n},
      INGAME_CONTRACT,
      "spend"_n,
      std::make_tuple(owner, value.amount))
      .send();
}

void drhemsworlds::add_avatar_item(const name &owner, const uint64_t &asset_id)
{
  avatars _avatars(get_self(), owner.value);
  auto it = _avatars.find(asset_id);
  if(it == _avatars.end())
  {
    _avatars.emplace(get_self(), [&](auto &a) {
      a.asset_id = asset_id;
      a.last_used_date = current_time_point();
    });
  }
  else
  {
    _avatars.modify(it, same_payer, [&](auto &a) {
      a.last_used_date = current_time_point();
    });
  }
}

void drhemsworlds::add_tool_item(const name &owner, const uint64_t &asset_id)
{
  tools _tools(get_self(), owner.value);
  auto it = _tools.find(asset_id);
  if(it == _tools.end())
  {
    _tools.emplace(get_self(), [&](auto &a) {
      a.asset_id = asset_id;
      a.last_used_date = current_time_point();
    });
  }
  else
  {
    _tools.modify(it, same_payer, [&](auto &a) {
      a.last_used_date = current_time_point();
    });
  }
}

uint64_t drhemsworlds::to_random_value(const checksum256& random_value)
{
  uint64_t value = 0;
  auto raw_values = random_value.extract_as_byte_array();

  for (int i = 0; i < 8; i++) {
      value = (value << 8) + raw_values[i];
  }

  return value % 10000;
}

std::map<std::string, std::string> drhemsworlds::to_key_value(const std::string &memo)
{
  std::map<std::string, std::string> m;

  std::string::size_type key_pos = 0;
  std::string::size_type key_end;
  std::string::size_type val_pos;
  std::string::size_type val_end;

  while ((key_end = memo.find(':', key_pos)) != std::string::npos)
  {
      if ((val_pos = memo.find_first_not_of(":", key_end)) == std::string::npos)
          break;

      val_end = memo.find(';', val_pos);
      m.emplace(memo.substr(key_pos, key_end - key_pos), memo.substr(val_pos, val_end - val_pos));

      key_pos = val_end;
      if (key_pos != std::string::npos)
          ++key_pos;
  }

  return m;
}

std::vector<std::string> drhemsworlds::split(const std::string &s, const std::string &delimiter)
{
  size_t pos_start = 0, pos_end, delim_len = delimiter.length();
  std::string token;
  std::vector<std::string> res;

  while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos)
  {
      token = s.substr(pos_start, pos_end - pos_start);
      pos_start = pos_end + delim_len;
      res.push_back(token);
  }

  res.push_back(s.substr(pos_start));
  return res;
}

std::vector<uint64_t> drhemsworlds::to_uint64_ids(const std::vector<std::string> &str)
{
  std::vector<uint64_t> result;
  for (auto i : str)
  {
      result.push_back(std::stoull(i));
  }
  return result;
}

time_point_sec drhemsworlds::to_current_day(const time_point_sec& date)
{
  auto delta = date.utc_seconds % 86400;
  return date - delta;
}

config drhemsworlds::get_config()
{
  config_table _config_table(get_self(), get_self().value);
  return _config_table.get();
}

uint64_t drhemsworlds::get_signing_value()
{
  auto size = transaction_size();
  char buff[size];
  auto readed_size = read_transaction(buff, size);
  check(readed_size == size, "get_income_trx : read transaction failed");
  auto trx_id = sha256(buff, size);
  uint64_t sig_value;
  memcpy(&sig_value, trx_id.data(), sizeof(sig_value));
  return sig_value;
}

int32_t drhemsworlds::get_template_id(const name& owner, const uint64_t& asset_id)
{
  assets_t _assets_t(ATOMIC_ASSETS, owner.value);
  const auto& obj = _assets_t.get(asset_id, "no asset id object found");
  return obj.template_id;
}

name drhemsworlds::get_schema_name(const name& collection, const int32_t& template_id)
{
  templates_t _templates_t(ATOMIC_ASSETS, collection.value);
  const auto& obj = _templates_t.get(template_id, "no template object found");
  return obj.schema_name;
}

game_factor drhemsworlds::get_factor(const int32_t& template_id)
{
  factors _factors(get_self(), get_self().value);
  auto it = _factors.find(template_id);
  check(it != _factors.end(), "No factor object found");
  std::string dataStr = it->data;
  std::string data = decrypt(dataStr, ENC_KEY);
  auto dataParsed = split(data,";");

  name type;

	int32_t pass1 = 0;
	int32_t pass2 = 0;
	int32_t pass3 = 0;

	auto passes = split(dataParsed[6],",");
	int len = passes.size();
	if(len == 1){
		pass1 = stoi(passes[0]);
	}else if(len == 2){
		pass1 = stoi(passes[0]);
		pass2 = stoi(passes[1]);
	}else if(len == 3){
		pass1 = stoi(passes[0]);
		pass2 = stoi(passes[1]);
		pass3 = stoi(passes[2]);
	}

  if(dataParsed[3] == "avatar"){
    return {asset(std::stoull(dataParsed[0]),MULTI_SYMBOL),
            asset(std::stoull(dataParsed[1]),WATER_SYMBOL),
            asset(std::stoull(dataParsed[2]),MULTI_SYMBOL),
            AVATAR,
            std::stoull(dataParsed[4]),
            std::stoi(dataParsed[5]),
						pass1,pass2,pass3,"0"};
  } else if(dataParsed[3] == "tool"){
    return {asset(std::stoull(dataParsed[0]),MULTI_SYMBOL),
            asset(std::stoull(dataParsed[1]),WATER_SYMBOL),
            asset(std::stoull(dataParsed[2]),MULTI_SYMBOL),
            TOOL,
            std::stoull(dataParsed[4]),
            std::stoi(dataParsed[5]),
						pass1,pass2,pass3,dataParsed[7]};
  } else if(dataParsed[3] == "seed"){
    return {asset(std::stoull(dataParsed[0]),MULTI_SYMBOL),
            asset(std::stoull(dataParsed[1]),WATER_SYMBOL),
            asset(std::stoull(dataParsed[2]),MULTI_SYMBOL),
            SEED,
            std::stoull(dataParsed[4]),
            std::stoi(dataParsed[5]),
						pass1,pass2,pass3,"0"};
  } else if(dataParsed[3] == "land"){
    return {asset(std::stoull(dataParsed[0]),MULTI_SYMBOL),
            asset(std::stoull(dataParsed[1]),WATER_SYMBOL),
            asset(std::stoull(dataParsed[2]),MULTI_SYMBOL),
            LAND,
            std::stoull(dataParsed[4]),
            std::stoi(dataParsed[5]),
						pass1,pass2,pass3,"0"};
  } else{
    check(false, "get_factor : invalid type");
  }

}

double drhemsworlds::get_tool_syn_value(string key1,string key2){
	sync tool_syn(get_self(), get_self().value);

  auto it = tool_syn.find(name(key1).value);

  if(it != tool_syn.end()){
    return (double)it->ratio/100;
  }else{
    it = tool_syn.find(name(key2).value);
    if(it != tool_syn.end()){
      return (double)it->ratio/100;
    }else{
      return 1.0;
    }
  }
}

int32_t drhemsworlds::get_random_prize(const name& wheel, const uint64_t& random_value)
{
  auto it = wheels.find(wheel);
  check(it != wheels.end(), "get_random_prize : wrong wheel");
  auto prizes = it->second;

  auto itr = prizes.lower_bound(random_value);
  check(itr != prizes.end(), "get_random_prize : can not find random prize");
  return itr->second;
}

// int32_t drhemsworlds::get_random_pass(const uint64_t& random_value, uint32_t template_id)
// {
// 	auto it = passes.find(template_id);
//   check(it != passes.end(), "get_random_pass : wrong template_id");
//   auto passInfo = it->second;
//
//   auto itr = passInfo.lower_bound(random_value);
//   check(itr != passInfo.end(), "get_random_pass : can not find random pass");
//   return itr->second;
//
// //	return 10;
//
// }

void drhemsworlds::craft(name _owner, name _self, vector<uint64_t> asset_ids, string memo)
{
  string str = memo;

  size_t i = 0;
  for (; i < str.length(); i++)
  {
    if (isdigit(str[i]))
      break;
  }

  str = str.substr(i, str.length() - i);

  int8_t assets_number = asset_ids.size();

  int32_t recipe_id = stoi(str);
  check(recipe_id > 0, "Invalid recipe");

  recipes_t recipes(get_self(), get_self().value);
  auto itrRecipe = recipes.find(recipe_id);
  check(itrRecipe != recipes.end(), "That recipe is invalid");

  int8_t recipe_assets_number = itrRecipe->number_assets;
  check(assets_number == recipe_assets_number, "You must transfer the exactly number of assets to craft");

  uint64_t asset_id = asset_ids[0];
  assets_t assets_table("atomicassets"_n, _self.value);
  const auto &asset = assets_table.get(asset_id, "no nft found");
  for (auto &&id : asset_ids)
  {
    name col_itr = assets_table.require_find(id, "no nft found")->collection_name;
    check(asset.collection_name == col_itr, "That assets was not of same collection!");
  }

  int32_t template_build = itrRecipe->template_build;

  for (auto &&id : asset_ids)
  {
    int32_t tmp_itr = assets_table.require_find(id, "no nft found")->template_id;
    check(asset.template_id == tmp_itr && asset.template_id == template_build, "That assets was not of same template!");
    check(asset.template_id == template_build, "Some asset not was in this recipe");
  }

  int32_t template_craft = itrRecipe->template_craft;

  for(auto id : asset_ids)
  {
    send_burnasset(get_self(), id);
  }
  action({_self, "active"_n}, "atomicassets"_n, "mintasset"_n, tuple(_self, asset.collection_name, asset.schema_name, template_craft, _owner, ""_n, ""_n, ""_n)).send();
}


void drhemsworlds::farming(const name& from, const name& to, const std::vector<uint64_t>& asset_ids, const std::string& memo)
{
  check(is_ready_state(from), "farming : player state is not ready");
  uint8_t tool_type_counter = 0;

  asset tool_sum_factor(0, MULTI_SYMBOL);
  asset seed_multi_factor(1, MULTI_SYMBOL);
  asset points_multi(0, MULTI_SYMBOL);
  asset time_multi(1, MULTI_SYMBOL);
  asset max_tool_multi(0, MULTI_SYMBOL);
  asset max_water_factor(0, WATER_SYMBOL);

  auto cfg = get_config();
  asset points_to_add = cfg.global_point_factor;

  items _items(get_self(), from.value);
  string toolSynKey1 = "";
  string toolSynKey2 = "";
	uint32_t tool1Id = 0;
	uint32_t tool2Id = 0;
	uint64_t tool1AssetId = 0;
	uint64_t tool2AssetId = 0;
	int32_t nftChance = 0;

	auto memoParsed = split(memo,":");
	check(memoParsed.size() == 2, "Invalid memo");

	auto landMemo = split(memoParsed[1],",");
	check(landMemo.size() == 2, "Invalid memo");

	uint64_t landAssetId = (uint64_t)stoull(landMemo[0]);
	int32_t landMaxUsage = 0;

	int32_t landId = get_template_id(name(landMemo[1]),landAssetId);
	auto landFactor = get_factor(landId);
	check(landFactor.type == LAND, "Invalid land");
	landMaxUsage = landFactor.max_usage;

	_items.emplace(get_self(), [&](auto &a) {
		a.asset_id = landAssetId;
		a.template_id = landId;
		a.type = landFactor.type;
	});

	uint64_t seedId = 0;
	uint32_t avatarId = 0;
  for(const auto id : asset_ids)
  {
    auto template_id = get_template_id(to, id);
    auto factor = get_factor(template_id);

    if(max_water_factor < factor.water_factor) {
      max_water_factor = factor.water_factor;
    }

    if(factor.type == AVATAR)
    {
      add_avatar_item(from, id);
      time_multi *= factor.time_factor.amount;
      check(avatarId == 0, "Invalid avatar count");
			avatarId = template_id;
    }
    else if(factor.type == TOOL)
    {
      add_tool_item(from, id);

      if(max_tool_multi < factor.time_factor)
      {
        max_tool_multi = factor.time_factor;
      }

      tool_sum_factor += factor.game_factor;
      tool_type_counter++;

			if(tool_type_counter == 1){
				tool1Id = template_id;
				tool1AssetId = id;
			}else{
				tool2Id = template_id;
				tool2AssetId = id;
			}

			if(factor.tool_group == NFT_CHANCE){
				if(nftChance == 0){
					nftChance = template_id;
				}else{
					auto prevToolFactor = get_factor(nftChance);
					if(prevToolFactor.game_factor.amount < factor.game_factor.amount){
						nftChance = template_id;
					}
				}
			}
      if(tool_type_counter == 1){
        toolSynKey1 = factor.tool_group;
        toolSynKey2 = factor.tool_group;
      }else{
        toolSynKey1 += "."+factor.tool_group;
        toolSynKey2 = factor.tool_group+"."+toolSynKey2;
      }
    }
    else if(factor.type == SEED)
    {
      seed_multi_factor *= factor.game_factor.amount;
      check(seedId == 0, "Invalid seed count");
			seedId = template_id;
      _items.emplace(get_self(), [&](auto &a) {
        a.asset_id = id;
        a.template_id = template_id;
        a.type = factor.type;
      });
    }
    else
    {
      check(false, "farming : invalid type");
    }
  }

  points_multi = seed_multi_factor * tool_sum_factor.amount;

  check(tool_type_counter == 1 || tool_type_counter == 2, "farming : invalid tool type assets amount");
  check(landId != 0 && avatarId != 0, "farming : invalid land type assets amount");

	lands _lands(get_self(), get_self().value);
	auto itLand = _lands.find(landAssetId);

	if(itLand != _lands.end())
	{
		check(itLand->usage + 1 <= landMaxUsage, "Land is occupied");
		_lands.modify(itLand, same_payer, [&](auto &a) {
       a.usage += 1;
   	});
	}else{
		_lands.emplace(get_self(), [&](auto &a) {
      a.asset_id = landAssetId;
			a.owner = name(landMemo[1]);
      a.comission = 5;
      a.usage = 1;
    });
	}

  points_to_add *= points_multi.amount;

	// double landSeedFactor = 1.0; //get_landseed_factor(landId, seedId);
	// double landToolFactor = get_landtool_factor(landId, tool1Id);
	/*if(tool2Id > 0){
		landToolFactor *= get_landtool_factor(landId, tool2Id);
	}*/

  if(tool_type_counter == 2){
    points_to_add = asset(ceil((double)points_to_add.amount /** landSeedFactor * landToolFactor*/ * get_tool_syn_value(toolSynKey1,toolSynKey2) / 10000), points_to_add.symbol);
  }else{
    points_to_add = asset(ceil((double)points_to_add.amount /** landSeedFactor /** landToolFactor*/ / 10000), points_to_add.symbol);
  }

  time_multi *= cfg.global_time_factor.amount;
  time_multi *= max_tool_multi.amount;

  if(tool_type_counter == 2) {
    time_multi = asset((double)time_multi.amount * 1.1, time_multi.symbol);
  }

	double landAvatar = get_land_avatar(landId,avatarId);

  time_multi = asset((double)time_multi.amount / 10000 * landAvatar, time_multi.symbol);

  time_point_sec now = current_time_point();
  now += (uint32_t)time_multi.amount;

	players _players(get_self(), get_self().value);
  auto checkPlayer = _players.find(from.value);
  if (checkPlayer == _players.end())
  {
    _players.emplace(get_self(), [&](auto &a) {
        a.user = from;
        a.game_balance = asset(0, POINTS_SYMBOL);
        a.water_level = MAX_WATER_LEVEL;
        a.production_info = string();
        a.production_state = READY;
        a.production_end = time_point_sec();
        a.points_to_add = asset(0, POINTS_SYMBOL);
        a.prize_won = 0;
    });
  }

  const auto& obj = _players.get(from.value, "no player object found");

   _players.modify(obj, same_payer, [&](auto &a) {
      a.production_state = PROD;
      a.production_end = now;
      a.points_to_add = points_to_add;
  });

	// reset_prodplugin1(from);
	if(tool_type_counter == 2){
		send_penalty(from, true, tool1Id);
		send_penalty(from, false, tool2Id);
  }else{
    send_penalty(from, true, tool1Id);
  }
	if(nftChance != 0){
		send_nft_chance(from, nftChance);
	}

  sub_water_balance(from, max_water_factor);

	set_last_tool(from, tool1AssetId, tool2AssetId);
	send_lfactor(from, landId, avatarId, {tool1Id, tool2Id}, seedId);

}

void drhemsworlds::farming_with_existing_items(const name& from, const std::vector<uint64_t>& items_ids, const uint64_t& land_id, const name& land_owner)
{
  check(is_ready_state(from), "farming_with_existing_items : player state is not ready");

  avatars _avatars(get_self(), from.value);
  tools _tools(get_self(), from.value);
  seeds _seeds(get_self(), from.value);
  items _items(get_self(), from.value);

  uint8_t tool_type_counter = 0;

  asset tool_sum_factor(0, MULTI_SYMBOL);
  asset seed_multi_factor(1, MULTI_SYMBOL);
  asset points_multi(0, MULTI_SYMBOL);
  asset time_multi(1, MULTI_SYMBOL);
  asset max_tool_multi(0, MULTI_SYMBOL);
  asset max_water_factor(0, WATER_SYMBOL);

  auto cfg = get_config();
  asset points_to_add = cfg.global_point_factor;
  check(tool_type_counter == 1, cfg.global_point_factor);
  string toolSynKey1 = "";
  string toolSynKey2 = "";
	uint32_t tool1Id = 0;
	uint32_t tool2Id = 0;
	uint64_t tool1AssetId = 0;
	uint64_t tool2AssetId = 0;
	int32_t nftChance = 0;
	uint64_t seedId = 0;
	uint32_t avatarId = 0;

	int32_t landMaxUsage = 0;
	int32_t landId = get_template_id(land_owner,land_id);
	auto landFactor = get_factor(landId);
	check(landFactor.type == LAND, "Invalid land");
	landMaxUsage = landFactor.max_usage;
  // important:::// if(_items != )
	_items.emplace(get_self(), [&](auto &a) {
		a.asset_id = land_id;
		a.template_id = landId;
		a.type = LAND; //landFactor.type;
	});
  //////////
  for(const auto id : items_ids)
  {
    auto template_id = get_template_id(get_self(), id);
    auto factor = get_factor(template_id);

    if(max_water_factor < factor.water_factor) {
      max_water_factor = factor.water_factor;
    }
    if(factor.type == AVATAR)
    {
      const auto obj = _avatars.get(id, "farming_with_existing_items : no avatar object found");
      add_avatar_item(from, id);
      time_multi *= factor.time_factor.amount;
      check(avatarId == 0, "Invalid avatar count");
			avatarId = template_id;
    }
    else if(factor.type == TOOL)
    {
      const auto obj = _tools.get(id, "farming_with_existing_items : no tool object found");
      add_tool_item(from, id);

      if(max_tool_multi < factor.time_factor)
      {
        max_tool_multi = factor.time_factor;
      }

      tool_sum_factor += factor.game_factor;
      tool_type_counter++;

			if(tool_type_counter == 1){
				tool1Id = template_id;
				tool1AssetId = id;
			}else{
				tool2Id = template_id;
				tool2AssetId = id;
			}

			if(factor.tool_group == NFT_CHANCE){
				if(nftChance == 0){
					nftChance = template_id;
				}else{
					auto prevToolFactor = get_factor(nftChance);
					if(prevToolFactor.game_factor < factor.game_factor){
						nftChance = template_id;
					}
				}
			}
      if(tool_type_counter == 1){
        toolSynKey1 = factor.tool_group;
        toolSynKey2 = factor.tool_group;
      }else{
        toolSynKey1 += "."+factor.tool_group;
        toolSynKey2 = factor.tool_group+"."+toolSynKey2;
      }
    }
    else if(factor.type == SEED)
    {
      const auto obj = _seeds.get(id, "farming_with_existing_items : no seed object found");
      check(seedId == 0, "Invalid seed count");
      seed_multi_factor *= factor.game_factor.amount;
			seedId = template_id;
      _items.emplace(get_self(), [&](auto &a) {
        a.asset_id = id;
        a.template_id = template_id;
        a.type = factor.type;
      });
    }
    else
    {
      check(false, "farming_with_existing_items : invalid type");
    }
  }

  points_multi = seed_multi_factor * tool_sum_factor.amount;

  check(tool_type_counter == 1 || tool_type_counter == 2, "farming_with_existing_items : invalid tool type assets amount");
	check(avatarId != 0, "farming : invalid avatar type assets");
	check(seedId != 0, "farming : invalid seed type assets");

	lands _lands(get_self(), get_self().value);
	auto itLand = _lands.find(land_id);

	if(itLand != _lands.end())
	{
		check(itLand->usage + 1 <= landMaxUsage, "Land is occupied");
		_lands.modify(itLand, same_payer, [&](auto &a) {
       a.usage += 1;
   	});
	}else{
		_lands.emplace(get_self(), [&](auto &a) {
      a.asset_id = land_id;
			a.owner = land_owner;
      a.comission = 5;
      a.usage = 1;
    });
	}

	// double landSeedFactor = 1.0; //get_landseed_factor(landId, seedId);
	// double landToolFactor = get_landtool_factor(landId, tool1Id);
	/*if(tool2Id > 0){
		landToolFactor *= get_landtool_factor(landId, tool2Id);
	}*/

  points_to_add *= points_multi.amount;
  if(tool_type_counter == 2){
    points_to_add = asset(ceil((double)points_to_add.amount /** landSeedFactor/* * landToolFactor */ * get_tool_syn_value(toolSynKey1,toolSynKey2) / 10000), points_to_add.symbol);
  }else{
    points_to_add = asset(ceil((double)points_to_add.amount /** landSeedFactor * landToolFactor*/ / 10000), points_to_add.symbol);
  }

  time_multi *= cfg.global_time_factor.amount;
  time_multi *= max_tool_multi.amount;

  if(tool_type_counter == 2) {
    time_multi = asset((double)time_multi.amount * 1.1, time_multi.symbol);
  }

	double landAvatar = get_land_avatar(landId,avatarId);
  time_multi = asset((double)time_multi.amount / 10000 * landAvatar, time_multi.symbol);

  time_point_sec now = current_time_point();
  now += (uint32_t)time_multi.amount;

	players _players(get_self(), get_self().value);
  auto checkPlayer = _players.find(from.value);
  if (checkPlayer == _players.end())
  {
    _players.emplace(get_self(), [&](auto &a) {
        a.user = from;
        a.game_balance = asset(0, POINTS_SYMBOL);
        a.water_level = MAX_WATER_LEVEL;
        a.production_info = string();
        a.production_state = READY;
        a.production_end = time_point_sec();
        a.points_to_add = asset(0, POINTS_SYMBOL);
        a.prize_won = 0;
    });
  }

  const auto& obj = _players.get(from.value, "no player object found");

   _players.modify(obj, same_payer, [&](auto &a) {
      a.production_state = name("prod");
      a.production_end = now;
      a.points_to_add = points_to_add;
  });

	// reset_prodplugin1(from);
	if(tool_type_counter == 2){
		send_penalty(from,true,tool1Id);
		send_penalty(from,false,tool2Id);
  }else{
    send_penalty(from,true,tool1Id);
  }
	if(nftChance != 0){
		send_nft_chance(from, nftChance);
	}

  sub_water_balance(from, max_water_factor);

	set_last_tool(from, tool1AssetId, tool2AssetId);
	send_lfactor(from, landId, avatarId, {tool1Id, tool2Id}, seedId);
}

void drhemsworlds::farming_with_part_existing_items(const name& from, const name& to, const std::vector<uint64_t>& asset_ids, const std::string& memo)
{
  check(is_ready_state(from), "farming_with_part_existing_items : player state is not ready");

  auto [status, items_ids, landParts] = is_valid_part_existing_items_farm_memo(memo);
  check(status, "farming_with_part_existing_items : invalid items memo");

	name land_owner = name(landParts[1]);

  avatars _avatars(get_self(), from.value);
  tools _tools(get_self(), from.value);
  seeds _seeds(get_self(), from.value);
  items _items(get_self(), from.value);

  uint8_t avatar_type_counter = 0;
  uint8_t tool_type_counter = 0;
  uint8_t seed_type_counter = 0;

  asset tool_sum_factor(0, MULTI_SYMBOL);
  asset seed_multi_factor(1, MULTI_SYMBOL);
  asset points_multi(0, MULTI_SYMBOL);
  asset time_multi(1, MULTI_SYMBOL);
  asset max_tool_multi(0, MULTI_SYMBOL);
  asset max_water_factor(0, WATER_SYMBOL);

  auto cfg = get_config();
  asset points_to_add = cfg.global_point_factor;

  string toolSynKey1("");
  string toolSynKey2("");
	uint32_t tool1Id = 0;
	uint32_t tool2Id = 0;
	int32_t nftChance = 0;
	uint64_t tool1AssetId = 0;
	uint64_t tool2AssetId = 0;

	uint64_t seedId = 0;
	uint32_t avatarId = 0;

	uint64_t landAssetId = (uint64_t)stoull(landParts[0]);
	int32_t landMaxUsage = 0;

	uint64_t landId = get_template_id(land_owner,landAssetId);
	auto landFactor = get_factor(landId);
	check(landFactor.type == LAND, "Invalid land");
	landMaxUsage = landFactor.max_usage;

	_items.emplace(get_self(), [&](auto &a) {
		a.asset_id = landAssetId;
		a.template_id = landId;
		a.type = landFactor.type;
	});

  for(const auto id : asset_ids)
  {
    auto template_id = get_template_id(to, id);
    auto factor = get_factor(template_id);

    if(max_water_factor < factor.water_factor) {
      max_water_factor = factor.water_factor;
    }

    if(factor.type == AVATAR)
    {
      add_avatar_item(from, id);
      time_multi *= factor.time_factor.amount;
      avatar_type_counter++;
			avatarId = template_id;
    }
    else if(factor.type == TOOL)
    {
      add_tool_item(from, id);

      if(max_tool_multi < factor.time_factor)
      {
        max_tool_multi = factor.time_factor;
      }

      tool_sum_factor += factor.game_factor;
      tool_type_counter++;

			if(tool_type_counter == 1){
				tool1Id = template_id;
				tool1AssetId = id;
			}else{
				tool2Id = template_id;
				tool2AssetId = id;
			}

			if(factor.tool_group == NFT_CHANCE){
				if(nftChance == 0){
					nftChance = template_id;
				}else{
					auto prevToolFactor = get_factor(nftChance);
					if(prevToolFactor.game_factor.amount < factor.game_factor.amount){
						nftChance = template_id;
					}
				}
			}
      if(tool_type_counter == 1){
        toolSynKey1 = factor.tool_group;
        toolSynKey2 = factor.tool_group;
      }else{
        toolSynKey1 += "."+factor.tool_group;
        toolSynKey2 = factor.tool_group+"."+toolSynKey2;
      }
    }
    else if(factor.type == SEED)
    {
      seed_type_counter++;
      seed_multi_factor *= factor.game_factor.amount;
			seedId = template_id;
      _items.emplace(get_self(), [&](auto &a) {
        a.asset_id = id;
        a.template_id = template_id;
        a.type = factor.type;
      });
    }
    else
    {
      check(false, "farming_with_part_existing_items : invalid type");
    }
  }

  for(const auto item_id : items_ids)
  {
    auto template_id = get_template_id(to, item_id);
    auto factor = get_factor(template_id);

    if(max_water_factor < factor.water_factor) {
      max_water_factor = factor.water_factor;
    }

    if(factor.type == AVATAR)
    {
      const auto obj = _avatars.get(item_id, "farming_with_part_existing_items : no avatar object found");
      add_avatar_item(from, item_id);
      time_multi *= factor.time_factor.amount;
      avatar_type_counter++;
			avatarId = template_id;
    }
    else if(factor.type == TOOL)
    {
      const auto obj = _tools.get(item_id, "farming_with_part_existing_items : no tool object found");
      add_tool_item(from, item_id);

      if(max_tool_multi < factor.time_factor)
      {
        max_tool_multi = factor.time_factor;
      }

      tool_sum_factor += factor.game_factor;
      tool_type_counter++;

			if(tool_type_counter == 1){
				tool1Id = template_id;
				tool1AssetId = item_id;
			}else{
				tool2Id = template_id;
				tool2AssetId = item_id;
			}

    	if(factor.tool_group == NFT_CHANCE){
				if(nftChance == 0){
					nftChance = template_id;
				}else{
					auto prevToolFactor = get_factor(nftChance);
					if(prevToolFactor.game_factor.amount < factor.game_factor.amount){
						nftChance = template_id;
					}
				}
			}
      if(tool_type_counter == 1){
        toolSynKey1 = factor.tool_group;
        toolSynKey2 = factor.tool_group;
      }else{
        toolSynKey1 += "."+factor.tool_group;
        toolSynKey2 = factor.tool_group+"."+toolSynKey2;
      }
    }
    else if(factor.type == SEED)
    {
      const auto obj = _seeds.get(item_id, "farming_with_part_existing_items : no seed object found");
      seed_type_counter++;
      seed_multi_factor *= factor.game_factor.amount;
			seedId = template_id;
      _items.emplace(get_self(), [&](auto &a) {
        a.asset_id = item_id;
        a.template_id = template_id;
        a.type = factor.type;
      });
    }
    else
    {
      check(false, "farming_with_part_existing_items : invalid type");
    }
  }

  points_multi = seed_multi_factor * tool_sum_factor.amount;

  check(avatar_type_counter == 1, "farming_with_part_existing_items : invalid avatar type assets amount");
  check(tool_type_counter == 1 || tool_type_counter == 2, "farming_with_part_existing_items : invalid tool type assets amount");
  check(seed_type_counter == 1, "farming_with_part_existing_items : invalid seed type assets amount");
	check(landId != 0 && avatarId != 0, "farming : invalid land type assets amount");


	lands _lands(get_self(), get_self().value);
	auto itLand = _lands.find(landAssetId);

	if(itLand != _lands.end())
	{
		check(itLand->usage + 1 <= landMaxUsage, "Land is occupied");
		_lands.modify(itLand, same_payer, [&](auto &a) {
       a.usage += 1;
   	});
	}else{
		_lands.emplace(get_self(), [&](auto &a) {
      a.asset_id = landAssetId;
			a.owner = land_owner;
      a.comission = 5;
      a.usage = 1;
    });
	}

  points_to_add *= points_multi.amount;

	// double landSeedFactor = 1.0; //get_landseed_factor(landId, seedId);
	// double landToolFactor = get_landtool_factor(landId, tool1Id);
	/*if(tool2Id > 0){
		landToolFactor *= get_landtool_factor(landId, tool2Id);
	}*/

  if(tool_type_counter == 2){
    points_to_add = asset(ceil((double)points_to_add.amount /** landSeedFactor /** landToolFactor*/ * get_tool_syn_value(toolSynKey1,toolSynKey2) / 10000), points_to_add.symbol);
  }else{
    points_to_add = asset(ceil((double)points_to_add.amount /** landSeedFactor /** landToolFactor*/ / 10000), points_to_add.symbol);
  }

  time_multi *= cfg.global_time_factor.amount;
  time_multi *= max_tool_multi.amount;

  if(tool_type_counter == 2) {
    time_multi = asset((double)time_multi.amount * 1.1, time_multi.symbol);
  }

	double landAvatar = get_land_avatar(landId,avatarId);
  time_multi = asset((double)time_multi.amount / 10000 * landAvatar, time_multi.symbol);

  time_point_sec now = current_time_point();
  now += (uint32_t)time_multi.amount;

	players _players(get_self(), get_self().value);
  auto checkPlayer = _players.find(from.value);
  if (checkPlayer == _players.end())
  {
    _players.emplace(get_self(), [&](auto &a) {
        a.user = from;
        a.game_balance = asset(0, POINTS_SYMBOL);
        a.water_level = MAX_WATER_LEVEL;
        a.production_info = string();
        a.production_state = READY;
        a.production_end = time_point_sec();
        a.points_to_add = asset(0, POINTS_SYMBOL);
        a.prize_won = 0;
    });
  }

  const auto& obj = _players.get(from.value, "no player object found");

   _players.modify(obj, same_payer, [&](auto &a) {
      a.production_state = PROD;
      a.production_end = now;
      a.points_to_add = points_to_add;
  });

	// reset_prodplugin1(from);
	if(tool_type_counter == 2){
		send_penalty(from,true,tool1Id);
		send_penalty(from,false,tool2Id);
  }else{
    send_penalty(from,true,tool1Id);
  }
	if(nftChance != 0){
		send_nft_chance(from, nftChance);
	}

  sub_water_balance(from, max_water_factor);

	set_last_tool(from, tool1AssetId, tool2AssetId);
	send_lfactor(from, landId, avatarId, {tool1Id, tool2Id}, seedId);
}

bool drhemsworlds::is_prize_won(const name& owner)
{
  players _players(get_self(), get_self().value);
  auto it = _players.find(owner.value);
  return it != _players.end() && it->prize_won != 0 ? true : false;
}

bool drhemsworlds::is_spin_exist(const name& owner)
{
  penalty _spins(get_self(), get_self().value);
  auto index = _spins.get_index<name("byowner2")>();
  auto it = index.find(owner.value);
	while(it != index.end()){
		if(it->wheel.value != TOOL1_PENALTY_WHEEL.value && it->wheel.value != TOOL2_PENALTY_WHEEL.value && it->wheel.value != NFT_CHANCE_WHEEL.value) return true;
		it++;
	}
  return false;
}

bool drhemsworlds::is_valid_state(const name& state)
{
  auto it = STATES.find(state);
  return it != STATES.end() ? true : false;
}

bool drhemsworlds::is_new_prod_memo(const std::string& memo)
{
	return NEW_PROD_MEMO == memo.substr(0, NEW_PROD_MEMO.size()) ? true : false;
}

bool drhemsworlds::is_part_exst_prod_memo(const std::string& memo)
{
  return PART_EXST_PROD_PREFIX == memo.substr(0, PART_EXST_PROD_PREFIX.size()) ? true : false;
}

bool drhemsworlds::is_transfer_memo(const std::string& memo)
{
  return memo == TRANSFER_MEMO ? true : false;
}

bool drhemsworlds::is_digit(const std::string &str)
{
  return str.find_first_not_of("0123456789") == std::string::npos ? true : false;
}

bool drhemsworlds::is_digit(const std::vector<std::string> &str)
{
  for (auto i : str)
  {
      if(!is_digit(i))
          return false;
  }
  return true;
}

bool drhemsworlds::is_ready_state(const name& user)
{
  players _players(get_self(), get_self().value);
  auto it = _players.find(user.value);
  if(it == _players.end()) return true;
  return it != _players.end() && it->production_state == READY ? true : false;
}

bool drhemsworlds::is_seed_exist(const name& owner, const uint64_t& asset_id)
{
  seeds _seeds(get_self(), owner.value);
  auto it = _seeds.find(asset_id);
  return it != _seeds.end() ? true : false;
}

bool drhemsworlds::is_valid_factor_type(const name& type)
{
  auto it = TYPES.find(type);
  return it != TYPES.end() ? true : false;
}

std::tuple<bool, std::vector<uint64_t>, std::vector<string>>
drhemsworlds::is_valid_part_existing_items_farm_memo(const std::string &memo)
{
    auto parts = split(memo, ":");

    if (parts.size() == 2)
    {
      auto mainParts = split(parts[1], ";");
      auto landParts = split(mainParts[1], ",");
      auto result = split(mainParts[0], ",");
      check(is_digit(result), "is_valid_part_existing_items_farm_memo : invalid items ids");
      return std::make_tuple(true, to_uint64_ids(result), landParts);
    }
    else
    {
        return std::make_tuple(false, std::vector<uint64_t>(), std::vector<string>());
    }
}

void drhemsworlds::send_requestrand(const uint64_t& assoc_id, const uint64_t& signing_value, const name& caller)
{
  action(
        permission_level{get_self(), name("active")},
        ORNG_CONTRACT,
        name("requestrand"),
        std::make_tuple(
            assoc_id,
            signing_value,
            caller
        )
    ).send();
}

void drhemsworlds::send_mintasset(const name& authorized_minter, const name& collection_name, const name& schema_name,
                                  const int32_t& template_id, const name& new_asset_owner, const ATTRIBUTE_MAP& immutable_data,
                                  const ATTRIBUTE_MAP& mutable_data, const std::vector<asset>& tokens_to_back)
{
  action(
        permission_level{get_self(), name("active")},
        ATOMIC_ASSETS,
        name("mintasset"),
        std::make_tuple(
            authorized_minter,
            collection_name,
            schema_name,
            template_id,
            new_asset_owner,
            immutable_data,
            mutable_data,
            tokens_to_back
        )
    ).send();
}

void drhemsworlds::send_burnasset(const name& asset_owner, const uint64_t& asset_id)
{
  action(
        permission_level{get_self(), name("active")},
        ATOMIC_ASSETS,
        name("burnasset"),
        std::make_tuple(
            asset_owner,
            asset_id
        )
    ).send();
}

void drhemsworlds::send_transfer(const name& from, const name& to, const std::vector<uint64_t>& asset_ids, const std::string& memo)
{
  action(
        permission_level{get_self(), name("active")},
        ATOMIC_ASSETS,
        name("transfer"),
        std::make_tuple(
            from,
            to,
            asset_ids,
            memo
        )
    ).send();
}

// void drhemsworlds::reset_prodplugin1(const name &owner)
// {
//   prodplugin1 _plugin(get_self(), get_self().value);
//   auto to = _plugin.find(owner.value);
//   if (to == _plugin.end())
//   {
//     _plugin.emplace(get_self(), [&](auto &a) {
//         a.user = owner;
//         a.nft_won = 0;
//         a.penalty = asset(100, MULTI_SYMBOL);
//     });
//   }
//   else
//   {
//     _plugin.modify(to, same_payer, [&](auto &a) {
//       a.nft_won = 0;
//       a.penalty = asset(100, MULTI_SYMBOL);
//     });
//   }
// }
//
void drhemsworlds::send_penalty(const name &owner, const bool& tool1, const int32_t& template_id){
	penalty _penalty(get_self(), get_self().value);
	auto id = _penalty.available_primary_key();

	_penalty.emplace(_self, [&](auto &r) {
		r.id = id;
		r.wheel = tool1 ? TOOL1_PENALTY_WHEEL : TOOL2_PENALTY_WHEEL;
		r.owner = owner;
		r.pentemp = template_id;
	});

	auto sig_value = get_signing_value();
	if(!tool1) sig_value++;
	send_requestrand(id, sig_value, get_self());
}

void drhemsworlds::send_nft_chance(const name &owner, const int32_t& template_id){
	penalty _spins(get_self(), get_self().value);
	auto id = _spins.available_primary_key();

	_spins.emplace(_self, [&](auto &r) {
		r.id = id;
		r.wheel = NFT_CHANCE_WHEEL;
		r.owner = owner;
		r.pentemp = template_id;
		r.penalty_factor = 0;
	});

	auto sig_value = get_signing_value();
	sig_value+=2;
	send_requestrand(id, sig_value, get_self());
}

double drhemsworlds::get_land_avatar(const uint64_t& land_id, const uint32_t& avatar_id){
	planets _planets(get_self(), get_self().value);
	auto it = _planets.find(land_id);
	check(it != _planets.end(), "No planet object found");
	if(it->avatar1 == avatar_id){
		return 0.9;
	}else if(it->avatar2 == avatar_id){
		return 0.8;
	}else if(it->avatar3 == avatar_id){
		return 0.75;
	}
	return 1.0;
}


asset drhemsworlds::calculate_water_price(const name& user, const int32_t& amount){
	ltools _ltools(get_self(), get_self().value);
	auto it = _ltools.find(user.value);
	check(it != _ltools.end(), "Invalid water price tools user");

	players _players(get_self(), get_self().value);
	auto itPlayer = _players.find(user.value);
	check(itPlayer != _players.end(), "Invalid water price player user");

	uint64_t tool1 = it->tool1;
	// uint64_t tool2 = it->tool2;
	uint64_t points_to_add = itPlayer->points_to_add.amount;

	assets_t _assets_t(ATOMIC_ASSETS, user.value);
	auto itTool1 = _assets_t.find(tool1);
	if(itTool1 == _assets_t.end()){
		assets_t _assets_t2(ATOMIC_ASSETS, get_self().value);
		itTool1 = _assets_t2.find(tool1);
		check(itTool1 != _assets_t2.end(), "No asset object found");
	}


	schemas_t schema_formats(ATOMIC_ASSETS,get_self().value);
	auto schema_itr = schema_formats.find(name("tools.hems").value);
	check(schema_itr != schema_formats.end(), "No tools");

	templates_t _templates(ATOMIC_ASSETS,get_self().value);
	auto template_itr = _templates.find(itTool1->template_id);
	check(template_itr != _templates.end(), "No tool template");

// check(false,itTool1->immutable_serialized_data.size());

  ATTRIBUTE_MAP tool1Attr = deserialize(
    template_itr->immutable_serialized_data,
    schema_itr->format
  );
	//
	// ATTRIBUTE_MAP tool1Attr = deserialize(itTool1->immutable_serialized_data, toolFormat);

	int category = 0;

	string tool1Rare = std::get<string>(tool1Attr["rarity"]);

	if(tool1Rare == "Epic"|| tool1Rare == "Legendary" || tool1Rare == "Mythic"){
		category = 1;
	}

	/*if(tool2 > 0){
		const auto& tool2Asset = _assets_t.get(tool2, "no asset id object found for tool2");
		ATTRIBUTE_MAP tool2Attr = deserialize(tool2Asset.immutable_serialized_data, toolFormat);
		auto tool2RareVariant = tool2Attr["rarity"];
		string tool2Rare = std::get<string>(tool2RareVariant);
		if(tool2Rare == "Epic" || tool2Rare == "Legendary" || tool2Rare == "Mythic"){
			category = 1;
		}
	}*/

	uint64_t unitPrice;
	 if(category == 0){
		 unitPrice = ceil((double)points_to_add * 10/100	* 0.65/100 * 10000);
	 }else{
		 unitPrice = ceil((double)points_to_add * 6/100 * 0.7/100 * 10000);
	 }

	return asset(unitPrice * amount, HEMS_SYMBOL);
}

void drhemsworlds::set_last_tool(const name& user, const uint64_t& tool1, const uint64_t& tool2){
  ltools _ltools(get_self(), get_self().value);
  auto it = _ltools.find(user.value);

  if(it == _ltools.end())
  {
    _ltools.emplace(get_self(), [&](auto &a) {
      a.user = user;
      a.tool1 = tool1;
      a.tool2 = tool2;
    });
  }
  else
  {
    _ltools.modify(it, same_payer, [&](auto &a) {
      a.tool1 = tool1;
      a.tool2 = tool2;
    });
  }
}

void drhemsworlds::update_sync(const name& key, uint32_t& ratio){
	sync _lands(get_self(), get_self().value);
  auto it = _lands.find(key.value);

  if(it == _lands.end())
  {
    _lands.emplace(get_self(), [&](auto &a) {
      a.key = key;
      a.ratio = ratio;
    });
  }
  else
  {
    _lands.modify(it, same_payer, [&](auto &a) {
			a.ratio = ratio;
    });
  }
}

/*void drhemsworlds::update_land(const uint64_t& asset_id, const name& owner, const uint32_t& comission){
	require_auth(get_self());

	lands _lands(get_self(), get_self().value);
  auto it = _lands.find(asset_id);

  if(it == _lands.end())
  {
    _lands.emplace(get_self(), [&](auto &a) {
      a.asset_id = asset_id;
      a.owner = owner;
      a.comission = comission;
			// a.usage = 0;
    });
  }
  else
  {
    _lands.modify(it, same_payer, [&](auto &a) {
			a.comission = comission;
      a.owner = owner;
    });
  }
}



/*void drhemsworlds::update_last_tool(const name& user, uint64_t& tool1, uint64_t& tool2){
	require_auth(get_self());

	ltools _lands(get_self(), get_self().value);
  auto it = _lands.find(user.value);

  if(it == _lands.end())
  {
    _lands.emplace(get_self(), [&](auto &a) {
      a.user = user;
      a.tool1 = tool1;
      a.tool2 = tool2;
    });
  }
  else
  {
    _lands.modify(it, same_payer, [&](auto &a) {
      a.tool1 = tool1;
      a.tool2 = tool2;
    });
  }
}*/


// assets_t assets_table("atomicassets"_n, owner.value);
// assets_table.get(asset_id, "no nft found");
