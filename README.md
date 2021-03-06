# Maxeler Dataflow Bitcoin Miner
Maxeler Dataflow Bitcoin Miner Implementation for study purposes (Master Thesis). <br />
A long deprecated Getwork protocol is used to fetch work for mining, and APP folder should be importable into MaxIDE. <br />
It is not recommended to use this software for serious bitcoin mining, because ASIC mining rigs took over long ago. <br /><br />

## Reads
- http://ev.fe.uni-lj.si/5-2017/Meden.pdf, <br />
- http://ipsitransactions.org/journals/papers/tar/2019jan/p9.pdf
- https://link.springer.com/chapter/10.1007/978-3-030-13803-5_9#citeas

## Dependencies
OS Linux CentOS is required, and additional libraries are needed: <br />
- sudo yum install curl -y <br />
- sudo yum install curl-devel -y <br />
- sudo yum install json-c -y <br />
- sudo yum install json-c-devel -y <br />

## Usage
Prebuilt executable binaries for MAX2B and MAIA boards are included in RELEASE section. <br />
The program accepts the following user inputs: <br />
  - -o is URL address of bitcoin mining pool, <br />
  - -u is miner's username, <br />
  - -p is miner's password, <br />
  - -h is help (optional). <br />

Example (for MAX2B board): ./BitcoinMiner-MAX2B -o http://ip:port, -u username -p password  <br />
Example (for MAIA board): ./BitcoinMiner-MAIA -o http://ip:port, -u username -p password <br />

Since a long deprecated Getwork protocol is used, you can only <br />
- connect directly to mining pools still supporting getwork (e.g. P2Pool) or <br />
- use https://github.com/slush0/stratum-mining-proxy between this Getwork miner and any Stratum mining pool (e.g. SlushPool)

## Licensing
This software is licensed under MIT license. <br />
However, it includes third party open source software components which have its own licenses: 
- for APP/CPUCode/libcurl.a, please see LICENSE/LICENSE-curl (MIT style license).
- for APP/CPUCode/libjsoc-c.a, please see LICENSE/LICENSE-json-c (MIT License).
