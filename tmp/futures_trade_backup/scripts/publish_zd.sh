tar cvzf zz_trader_zd.tar.gz src/
#cp -rf config config_zd
#tar cvzf config_zd.tar.gz config_zd/

#scp -P 48484 zz_trader_zd.tar.gz config_zd.tar.gz install_zd.sh kill.sh tune_tcp_latency.sh mingyuan@180.169.101.108:/home/mingyuan/packages/

scp -P 48484 zz_trader_zd.tar.gz install_zd.sh mingyuan@180.169.101.108:/home/mingyuan/packages/

rm zz_trader_zd.tar.gz 
