# Maxeler-Bitcoin-Miner
Maxeler Dataflow Implementation of Bitcoin Miner for study purposes (Master Thesis). <br />
It uses Getwork to fetch work for mining, and APP folder shall be importable into MaxIDE. <br />
It is not meant for serious bitcoin mining though, as ASIC mining rigs took over long time ago. <br />

Additional libraries have to be installed: <br />
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
./BitcoinMiner_MAX2B -o http://ip:port, -u username -p password  <br />
Example (for MAIA board): <br />
./BitcoinMiner_MAIA -o http://ip:port, -u username -p password <br />

Measured energy efficiency (more is better): <br />
Maxeler MaxCard MAX2B (MAX2336B): 342 Mhash/s at 56 W = 6.1 Mhash/s/W <br />
Maxeler MaxCard MAIA (MAX4848A): 1432 Mhash/s at 56 W = 25.6 Mhash/s/W <br />

Compared to other hardware examples: <br />
GPU AMD Radeon HD 6950: 280 Mhash/s at 200 W = 1.4 Mhash/s/W (less efficient than MaxCards) <br />
ASIC Antminer S7: 4.37 Thash/s at 1350 W = 3.2 Ghash/s/W (more efficient than MaxCards) <br />
