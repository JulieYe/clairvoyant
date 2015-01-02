rm -rf zz_trader
rm -rf config

tar xvzf zz_trader.tar.gz
mv src zz_trader

tar xvzf config.tar.gz

make -C zz_trader clean
make -C zz_trader Trader SPFLAG=PRODUCTION DFLAG=DEBUG BFLAG=BROKER_GJ THFLAG=TICKERHEAD_IF XFLAG=EXCHANGE_CFFEX
mv zz_trader/Trader zz_trader/Trader.GJ.IF

make -C zz_trader clean
make -C zz_trader Trader SPFLAG=PRODUCTION DFLAG=DEBUG BFLAG=BROKER_GJ THFLAG=TICKERHEAD_RU XFLAG=EXCHANGE_SHFE
mv zz_trader/Trader zz_trader/Trader.GJ.RU

make -C zz_trader clean
make -C zz_trader Trader SPFLAG=PRODUCTION DFLAG=DEBUG BFLAG=BROKER_ZD THFLAG=TICKERHEAD_IF XFLAG=EXCHANGE_CFFEX
mv zz_trader/Trader zz_trader/Trader.ZD.IF

make -C zz_trader clean
make -C zz_trader Trader SPFLAG=PRODUCTION DFLAG=DEBUG BFLAG=BROKER_ZD THFLAG=TICKERHEAD_RU XFLAG=EXCHANGE_SHFE
mv zz_trader/Trader zz_trader/Trader.ZD.RU

make -C zz_trader clean
make -C zz_trader TraderDummy SPFLAG=PRODUCTION DFLAG=DEBUG BFLAG=BROKER_ZD THFLAG=TICKERHEAD_IF XFLAG=EXCHANGE_CFFEX
mv zz_trader/TraderDummy zz_trader/TraderDummy.ZD.IF

make -C zz_trader clean
make -C zz_trader TraderDummy SPFLAG=PRODUCTION DFLAG=DEBUG BFLAG=BROKER_ZD THFLAG=TICKERHEAD_RU XFLAG=EXCHANGE_SHFE
mv zz_trader/TraderDummy zz_trader/TraderDummy.ZD.RU

#make -C zz_trader MdCollector SPFLAG=PRODUCTION DFLAG=DEBUG BFLAG=BROKER_GJ THFLAG=TICKERHEAD_IF XFLAG=EXCHANGE_CFFEX

cd zz_trader
rm *.hpp *.cpp *.h *.o IF.test.csv 20*.IF* .gold*.csv makefile test.config.specs test.contract.specs TODO README
rm -rf unit.test.docs
rm ../zz_trader.tar.gz
rm ../config.tar.gz