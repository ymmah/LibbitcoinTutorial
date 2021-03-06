#include <bitcoin/bitcoin.hpp>
#include "HD_walletTESTNET.cpp"
#include <string.h>

using namespace bc;
using namespace bc::wallet;
using namespace bc::machine;
using namespace bc::chain;



int main()
{
	std::string Mnemonic1 = "";
	std::string Mnemonic2 = "";
	std::string Mnemonic3 = "";

	HD_Wallet wallet1(split(Mnemonic1));
	HD_Wallet wallet2(split(Mnemonic2));
	HD_Wallet wallet3(split(Mnemonic3));

	data_chunk pubkey1 = to_chunk(wallet1.childPublicKey(1).point());
	data_chunk pubkey2 = to_chunk(wallet2.childPublicKey(1).point());
	data_chunk pubkey3 = to_chunk(wallet3.childPublicKey(1).point());
	wallet1.displayAddress(1);



	operation::list opList {operation(opcode(82)), operation(opcode(33)), operation(pubkey1), operation(opcode(33)), operation(pubkey2), operation(opcode(33)), operation(pubkey3), operation(opcode(83)), operation(opcode(175))};
	script multisigScript(opList);

	if(multisigScript.is_valid())
	{
		std::cout << "Script is Valid!\n" << std::endl;
	}else{
		std::cout << "Script Invalid!  \n" << std::endl;
	}

	std::cout << "Redeeem Script: \n" << std::endl;
	std::cout << multisigScript.to_string(0) << "\n" << std::endl;
	std::cout << encode_base16(multisigScript.to_data(0)) << "\n" <<std::endl;

	short_hash scriptHash = bitcoin_short_hash(multisigScript.to_data(0));

	script pay2ScriptHash = script(multisigScript.to_pay_script_hash_pattern(scriptHash));



	std::cout << "Locking Script: " << std::endl;
	std::cout << pay2ScriptHash.to_string(0xffffffff) << "\n" << std::endl;

	payment_address multsigAddress(multisigScript, 0xc4);
	std::cout << "Payment Address: " << std::endl;

	std::cout << multsigAddress.encoded() << "\n" << std::endl;
	std::cout << payment_address(scriptHash, 0xc4).encoded() << "\n" << std::endl;


	//transaction old(txHash);
	transaction tx = transaction();
	script unlocking = script();
	unlocking = unlocking.to_pay_key_hash_pattern(bitcoin_short_hash(pubkey1));
	std::cout << unlocking.to_string(0xffffffff) << std:: endl; 
	hash_digest prevOutHash = hash_literal("3b2c02693ef047ad8b8898fbc09fc17391e3d101913b7ea1161034928e1c48ca");
	output_point prevOut(prevOutHash, 0u);
	input input1 = input();
	//input1.set_script(unlocking);
	input1.set_previous_output(prevOut);
	input1.set_sequence(0xffffffff);
	if(input1.is_valid())
	{
		std::cout<< "Valid! " << std::endl;
	} else {
		std::cout << "Invalid! " << std::endl;
	}
	output output1(819940000, pay2ScriptHash);
	tx.inputs().push_back(input1);
	tx.outputs().push_back(output1);

	endorsement endorsed;
	if(unlocking.create_endorsement(endorsed, wallet1.childPrivateKey(1).secret(), unlocking, tx, 0u, all))
	{
		std::cout << encode_base16(endorsed) << std::endl;
	}
	operation::list sigScript;
	sigScript.push_back(operation(endorsed));
	sigScript.push_back(operation(pubkey1));
	unlocking = script(sigScript);
	//input1.set_script(unlocking);
	//tx.inputs().push_back(input1);
	tx.inputs()[0].set_script(unlocking);
	std::cout << unlocking.to_string(0xffffffff) << "\n" << std::endl;
	if(unlocking.is_sign_key_hash_pattern(unlocking.operations()))
	{
		std::cout << "Sign Key Pattern" << std::endl;
	}else{
		std::cout << "Nope" << std::endl;
	}
	if(tx.is_valid())
	{
		std::cout << "TX: Valid! " << std::endl;
	} else {
		std::cout << "TX: Invalid! " << std::endl;
	}
	std::cout << tx.total_input_value() << std::endl;
	std::cout << tx.total_output_value() << std::endl;
	std::cout << tx.is_overspent() << std::endl;
	std::cout << encode_base16(tx.to_data()) << std::endl;

}