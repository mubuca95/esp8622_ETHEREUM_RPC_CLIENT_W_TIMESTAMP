# esp8622_ethereum_rpc

Example Code for accessing a private Ethereum Node via RPC in a local network. "eth.sendTransaction" and "eth.blockNumber" functions are available.
RPC can be activated through following commands in Geth-Console
--rpc --rpcport 8545 --rpccorsdomain <ip's of devices allowed to use rpc> --rpcaddr   --rpcapi "db,eth,net,web3,miner,personal"
