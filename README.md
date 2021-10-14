# EOS Smart Contracts in C++
Welcome to this repository of sample EOS smart contracts in C++. Here you will find several practical examples of smart contracts that can be deployed on the EOS blockchain.

## Getting started
Before you can deploy any of the smart contracts in this repository, you will need to have the EOSIO software and the eosio.cdt library installed on your machine. You can find instructions for installing these dependencies on the EOSIO website.

## Building and deploying
Each smart contract in this repository comes with its own CMakeLists.txt file, which is used to build and deploy the contract. To build and deploy a contract, follow these steps:

Navigate to the directory of the contract you want to build and deploy.
* Run the command `cmake .` to generate the build files.
* Run the command `make` to build the contract.
* Run the command `cleos set contract [contract_name] [path_to_contract_folder]` to deploy the contract to the EOS blockchain.

## Contributions
We welcome contributions to this repository. If you have a new smart contract or sample that you would like to add, please submit a pull request.

## License
This repository is licensed under the Mozilla Public License Version 2.0. See the LICENSE file for more details.