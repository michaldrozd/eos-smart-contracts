// Copyright (C) 2021 Michal Drozd
// All Rights Reserved

// rock-paper-scissors game in EOS

#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>

using namespace eosio;

class rps : public contract {
 public:
	using contract::contract;

	[[eosio::action]]
	void play(name player1, name player2, std::string choice1, std::string choice2) {
		require_auth(player1);
		require_auth(player2);

		eosio_assert(choice1 == "rock" || choice1 == "paper" || choice1 == "scissors", "Invalid choice1");
		eosio_assert(choice2 == "rock" || choice2 == "paper" || choice2 == "scissors", "Invalid choice2");

		game_index games(_self, _self.value);
		games.emplace(player1, [&](auto& g) {
			g.key = games.available_primary_key();
			g.player1 = player1;
			g.player2 = player2;
			g.choice1 = choice1;
			g.choice2 = choice2;
		});
	}

	[[eosio::action]]
	void reveal(uint64_t key) {
		game_index games(_self, _self.value);
		auto itr = games.find(key);
		eosio_assert(itr != games.end(), "Game not found");

		name p1 = itr->player1;
		name p2 = itr->player2;
		require_auth(p1);
		require_auth(p2);

		endgame(itr);
	}

 private:
	struct [[eosio::table]] game {
		uint64_t key;
		name player1;
		name player2;
		std::string choice1;
		std::string choice2;

		uint64_t primary_key() const { return key; }
	};

	typedef eosio::multi_index<"games"_n, game> game_index;

	void endgame(game_index::const_iterator itr) {
		name winner;
		std::string choice1 = itr->choice1;
		std::string choice2 = itr->choice2;
		if (choice1 == choice2) {
			winner = "tie"_n;
		} else if (choice1 == "rock" && choice2 == "scissors" || choice1 == "paper" && choice2 == "rock" || choice1 == "scissors" && choice2 == "paper") {
			winner = itr->player1;
		} else {
			winner = itr->player2;
		}

		print("Winner: ", winner);
	}
};

EOSIO_DISPATCH(rps, (play)(reveal))