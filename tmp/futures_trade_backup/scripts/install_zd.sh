rm -rf zz_trader_zd

tar xvzf zz_trader_zd.tar.gz
mv src zz_trader_zd

make -C zz_trader_zd clean
make -C zz_trader_zd MdCollector SPFLAG=PRODUCTION DFLAG=DEBUG BFLAG=BROKER_ZD THFLAG=TICKERHEAD_IF XFLAG=EXCHANGE_CFFEX

cd zz_trader_zd
rm *.hpp *.cpp *.h *.o IF.test.csv 20*.IF* .gold*.csv makefile test.config.specs test.contract.specs TODO README
rm -rf unit.test.docs
rm ../zz_trader_zd.tar.gz
