# Maxeler Bitcoin Miner
Maxeler Dataflow Bitcoin Miner Implementation for study purposes (Master Thesis). <br />
It uses Getwork to fetch work for mining, and APP folder shall be importable into MaxIDE. <br />
It is not meant for serious bitcoin mining though, as ASIC mining rigs took over years ago. <br />

OS Linux CentOS is required, additional libraries have to be installed: <br />
sudo yum install curl -y <br />
sudo yum install curl-devel -y <br />
sudo yum install json-c -y <br />
sudo yum install json-c-devel -y <br />

The prebuilt executable binaries for MAX2B and MAIA boards are included. <br />
The program accepts the following user inputs: <br />
-o is URL address of bitcoin mining pool, <br />
-u is miner's username, <br />
-p is miner's password, <br />
-h is help (optional). <br />

Example (for MAX2B board): <br />
./BitcoinMiner-MAX2B -o http://ip:port, -u username -p password  <br />
Example (for MAIA board): <br />
./BitcoinMiner-MAIA -o http://ip:port, -u username -p password <br />

Because it uses long-deprecated getwork protocol, you can only 
1) connect directly to mining pools still supporting getwork (e.g. P2Pool) or
2) use https://github.com/slush0/stratum-mining-proxy between this getwork miner and stratum mining pool (e.g. SlushPool)

Results available at http://ev.fe.uni-lj.si/5-2017/Meden.pdf
