// Copyright (C) 2021 Michal Drozd
// All Rights Reserved


#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include <string>

using namespace eosio;

class game : public contract {
 public:
	using contract::contract;

	// action to create a new game
	[[eosio::action]]
	void newgame(name creator, std::string game_name) {
		// require the creator to have authorization to create a new game
		require_auth(creator);
		// create a new row in the game table and set the values of the fields
		game_index games(_self, _self.value);
		games.emplace(creator, [&](auto& g) {
			g.game_id = games.available_primary_key();
			g.creator = creator;
			g.game_name = game_name;
			g.status = "in progress";
		});
	}

	// action to join an existing game
	[[eosio::action]]
	void joingame(name player, uint64_t game_id) {
		// get the game being joined
		game_index games(_self, _self.value);
		auto game = games.get(game_id, "Game not found");
		// check if the game is still in progress
		eosio_assert(game.status == "in progress", "This game has already ended");
		// update the game with the new player
		games.modify(game, player, [&](auto& g) {
			g.players.push_back(player);
		});
	}

	// action to end a game
	[[eosio::action]]
	void endgame(uint64_t game_id, std::string winner) {
		// get the game being ended
		game_index games(_self, _self.value);
		auto game = games.get(game_id, "Game not found");
		// check if the game is still in progress
		eosio_assert(game.status == "in progress", "This game has already ended");
		// update the game's status and winner
		games.modify(game, same_payer, [&](auto& g) {
			g.status = "ended";
			g.winner = winner;
		});
	}

 private:
	// struct to represent a row in the game table
	struct [[eosio::table]] game_info {
		uint64_t game_id; // primary key
		name creator;
		std::string game_name;
		std::vector<name> players;
		std::string status;
		std::string winner;

		uint64_t primary_key() const { return game_id; }
	};

	// create a multi-index table to store the games
	typedef eosio::multi_index<"games"_n, game_info> game_index;
};

EOSIO_DISPATCH(game, (newgame)(joingame)(endgame))