tar cvzf zz_trader.tar.gz src/
tar cvzf config.tar.gz config/

scp -P 48484 zz_trader.tar.gz config.tar.gz install.sh kill.sh tune_tcp_latency.sh mingyuan@180.169.101.108:/home/mingyuan/packages/

rm zz_trader.tar.gz config.tar.gz