// Copyright (C) 2021 Michal Drozd
// All Rights Reserved

#include <eosio/eosio.hpp>

using namespace eosio;

CONTRACT token: public contract {
public:
using contract::contract;

ACTION create(name issuer, asset maximum_supply);

ACTION issue(name to, asset quantity, string memo);

ACTION transfer(name from, name to, asset quantity, string memo);

private:
TABLE account{
	asset    balance;
	uint64_t primary_key()const { return balance.symbol.code().raw(); }
};

TABLE currency_stats{
	asset          supply;
	asset          max_supply;
	name           issuer;
	uint64_t primary_key()const { return supply.symbol.code().raw(); }
};

typedef eosio::multi_index<"accounts"_n, account> accounts;
typedef eosio::multi_index<"stat"_n, currency_stats> stats;

void sub_balance(name owner, asset value);

void add_balance(name owner, asset value, name ram_payer);

};

ACTION token::create(name issuer, asset maximum_supply)
{
	require_auth(issuer);
	auto sym = maximum_supply.symbol;
	eosio::check(sym.is_valid(), "invalid symbol name");
	eosio::check(maximum_supply.is_valid(), "invalid supply");
	eosio::check(maximum_supply.amount > 0, "max-supply must be positive");

	stats statstable(get_self(), sym.code().raw());
	auto existing = statstable.find(sym.code().raw());
	eosio::check(existing == statstable.end(), "token with symbol already exists");

	statstable.emplace(issuer, [&](auto& s)
	{
		s.supply = asset{ 0, sym };
		s.max_supply = maximum_supply;
		s.issuer = issuer;
	});
}

ACTION token::issue(name to, asset quantity, string memo)
{
	auto sym = quantity.symbol;
	eosio::check(sym.is_valid(), "invalid symbol name");
	eosio::check(memo.size() <= 256, "memo has more than 256 bytes");

	stats statstable(get_self(), sym.code().raw());
	auto existing = statstable.find(sym.code().raw());
	eosio::check(existing != statstable.end(), "token with symbol does not exist, create token before issue");
	const auto& st = *existing;

	require_auth(st.issuer);
	eosio::check(quantity.is_valid(), "invalid quantity");
	eosio::check(quantity.amount > 0, "must issue positive quantity");

	eosio::check(quantity.symbol == st.supply.symbol, "symbol precision mismatch");
	eosio::check(quantity.amount <= st.max_supply.amount - st.supply.amount, "quantity exceeds available supply");
	statstable.modify(st, same_payer, [&](auto& s)
	{
		s.supply += quantity;
	});

	add_balance(to, quantity, st.issuer);
}

ACTION token::transfer(name from, name to, asset quantity, string memo)
{
	eosio::check(from != to, "cannot transfer to self");
	require_auth(from);
	eosio::check(is_account(to), "to account does not exist");
	auto sym = quantity.symbol.code();
	stats statstable(get_self(), sym.raw());
	const auto& st = statstable.get(sym.raw());
	require_recipient(from);
	require_recipient(to);

	eosio::check(quantity.is_valid(), "invalid quantity");
	eosio::check(quantity.amount > 0, "must transfer positive quantity");
	eosio::check(quantity.symbol == st.supply.symbol, "symbol precision mismatch");
	eosio::check(memo.size() <= 256, "memo has more than 256 bytes");

	sub_balance(from, quantity);
	add_balance(to, quantity, from);
}

void token::sub_balance(name owner, asset value)
{
	accounts from_acnts(get_self(), owner.value);

	const auto& from = from_acnts.get(value.symbol.code().raw(), "no balance object found");
	eosio::check(from.balance.amount >= value.amount, "overdrawn balance");

	from_acnts.modify(from, owner, [&](auto& a)
	{
		a.balance -= value;
	});
}

void token::add_balance(name owner, asset value, name ram_payer)
{
	accounts to_acnts(get_self(), owner.value);
	auto to = to_acnts.find(value.symbol.code().raw());
	if (to == to_acnts.end())
	{
		to_acnts.emplace(ram_payer, [&](auto& a)
		{
			a.balance = value;
		});
	}
	else
	{
		to_acnts.modify(to, same_payer, [&](auto& a)
		{
			a.balance += value;
		});
	}
}