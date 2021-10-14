// Copyright (C) 2021 Michal Drozd
// All Rights Reserved

#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>

using namespace eosio;

class textstorage : public contract {
 public:
	using contract::contract;

	// savetext action allows a user to save a text string to the contract's storage
	[[eosio::action]]
	void savetext(name user, std::string text) {
		// require the user to have authorization to save the text
		require_auth(user);
		text_index texts(_self, _self.value);
		// emplace a new row in the table and set the values of the fields
		texts.emplace(user, [&](auto& p) {
			p.key = texts.available_primary_key();
			p.user = user;
			p.text = text;
		});
	}

	// gettext action allows a user to retrieve the text string saved under a specific key
	[[eosio::action]]
	void gettext(uint64_t key) {
		text_index texts(_self, _self.value);
		// get the text stored under a specific key
		auto text = texts.get(key, "Text not found");
		// print the text
		print(text.text);
	}

 private:
	// stored_text struct represents a row in the table
	struct [[eosio::table]] stored_text {
		uint64_t key; // primary key
		name user;
		std::string text;

		uint64_t primary_key() const { return key; }
	};

	// create a multi-index table to store the texts
	typedef eosio::multi_index<"texts"_n, stored_text> text_index;
};

EOSIO_DISPATCH(textstorage, (savetext)(gettext))