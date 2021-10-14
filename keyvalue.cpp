#include <eosio/eosio.hpp>

using namespace eosio;

CONTRACT storage : public contract {
public:
using contract::contract;

//set action for storing key value pair
ACTION set(name user, std::string key, std::string value) {
	//require user to be the same as the one provided in action
	require_auth(user);

	//create table of type data_table
	data_table data(get_self(), get_self().value);
	//look for the user in the table
	auto itr = data.find(user.value);
	if (itr == data.end()) {
		//user not found, insert new user with key and value
		data.emplace(user, [&](auto& row) {
			row.user = user;
			row.key = key;
			row.value = value;
		});
	} else {
		//user found, modify the key and value
		data.modify(itr, user, [&](auto& row) {
			row.key = key;
			row.value = value;
		});
	}
}

//get action for retrieving key value pair
ACTION get(name user, std::string key) {
	//create table of type data_table
	data_table data(get_self(), get_self().value);
	//look for the user in the table
	auto itr = data.find(user.value);
	//check if user exists
	eosio::check(itr != data.end(), "User data not found");
	//check if key exists
	eosio::check(itr->key == key, "Key not found for user");
	//print the value
	print(itr->value);
}

private:
//structure of the table
TABLE data_struct {
	name user;
	std::string key;
	std::string value;

	//primary key
	uint64_t primary_key() const { return user.value; }
};
//typedef of data_table
typedef eosio::multi_index<"data"_n, data_struct> data_table;
};