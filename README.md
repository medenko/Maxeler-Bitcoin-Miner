# Maxeler Bitcoin Miner
Maxeler Dataflow Bitcoin Miner Implementation for study purposes (Master Thesis). <br />
It uses Getwork to fetch work for mining, and APP folder shall be importable into MaxIDE. <br />
It is not meant for serious bitcoin mining though, as ASIC mining rigs took years ago. <br />

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

Measured energy efficiency of stable builds (more is better): <br />
Maxeler MaxCard MAX2B (MAX2336B): 282 Mhash/s at 56 W = 5.0 Mhash/s/W <br />
Maxeler MaxCard MAIA (MAX4848A): 1432 Mhash/s at 56 W = 25.6 Mhash/s/W <br />

Compared to other hardware examples: <br />
GPU AMD Radeon HD 6950: 293 Mhash/s at 200 W = 1.47 Mhash/s/W (less energy-efficient than MaxCards) <br />
ASIC Antminer S7: 4.73 Thash/s at 1293 W = 3.66 Ghash/s/W (far more energy-efficient than MaxCards) <br />

More about it at http://ev.fe.uni-lj.si/5-2017/Meden.pdf
