# Maxeler-Bitcoin-Miner
Maxeler Dataflow Implementation of Bitcoin Miner for study purposes (Master Thesis).
It uses Getwork to fetch work, and it shall be importable into MaxIDE.
It is not meant for serious bitcoin mining, as ASIC miners took over long time ago.

Additional libraries have to be installed:
sudo yum install curl -y
sudo yum install curl-devel -y
sudo yum install json-c -y
sudo yum install json-c-devel -y

Measured energy efficiency (more is better):
Maxeler MaxCard MAX2B (MAX2336B): 360 Mhash/s at 56 W = 6.4 Mhash/s/W
Maxeler MaxCard MAIA (MAX4848A): 1430 Mhash/s at 56 W = 25.5 Mhash/s/W

Compared to other hardware examples:
GPU AMD Radeon HD 6950: 280 Mhash/s at 200 W = 1.4 Mhash/s/W (less efficient than MaxCards)
ASIC Antminer S7: 4.37 Thash/s at 1350 W = 3.2 Ghash/s/W (more efficient than MaxCards) 
