import csv


def parseOrderMessage(row):
    row = row.strip('\n')
    items = row.split(",")
    dd = {"MsgType": items[0]}
    fields = dict([tuple(tt.split("=")) for tt in items if "=" in tt])
    dd.update(fields)
    dd['Px'] = float(dd['Px'])
    dd['Side'] = int(dd['Side'])
    dd['Qty'] = int(dd['Qty'])
    dd['Ts'] = int(dd['Ts'])
    dd['StratId'] = int(dd['StratId'])
    dd['MessageId'] = int(dd['MessageId'])
    dd['OrderId'] = int(dd['OrderId'])
    return dd


def parseHeader(row):
    row = row.strip('\n')
    header = row.split(',')
    return header

def parseStrategyStatus(row, header):
    row = row.strip('\n')
    items = row.split(',')
    dd = dict(zip(header, items))
    for key in ['Ts', 'Volume', 'OpenPosition', 'OpenBuyLimitOrder', 'OpenSellLimitOrder', 'FillCount', 'LimitOrderCount', 'MarketOrderCount', 'CancelCount',
                'BidQty', 'AskQty', 'MarketVolume']:
        dd[key] = int(dd[key])
    for key in ['PNL', 'ClosedPNL', 'OpenPNL', 'BidPx', 'AskPx', 'TradePx', 'Signal', 'Volatility']:
        dd[key] = float(dd[key])
    return dd


class Trade:
    def __init__(self, numUpdates, preOpenMarket, openOrder):
        self.numUpdates_ = numUpdates
        self.MarketBeforeOpen_ = preOpenMarket
        self.OpenOrder_ = openOrder
        self.CancelOpenOrder_ = []
        self.MarketAfterOpen_ = []
        self.ExitOrder_ = []
        self.ExitCancel_ = []
        self.Executions_ = []
        self.isInTrade = True
        self.OpenPosition = 0

    def updateWithNewRow(self, dictionary):
        if "Name" in dictionary.keys():
            self.OpenPosition  = dictionary['OpenPosition']
            if len(self.MarketAfterOpen_) <= self.numUpdates_:
                if len(self.MarketAfterOpen_) == 0:
                    self.MarketAfterOpen_.append(dictionary.copy())
                elif dictionary['Ts'] > self.MarketAfterOpen_[-1]['Ts'] + 400000000:
                    self.MarketAfterOpen_.append(dictionary.copy())

        if self.isInTrade:
            if "MsgType" in dictionary.keys():
                if self.OpenPosition == 0:
                    if "CancelLimitOrderMessage" == dictionary['MsgType']:
                        self.CancelOpenOrder_.append(dictionary.copy())
                    elif "CanceledLimitOrderMessage" == dictionary['MsgType']:
                        self.CancelOpenOrder_.append(dictionary.copy())
                        self.isInTrade = False
                    elif "RejectCancelLimitOrderMessage" == dictionary["MsgType"]:
                        self.CancelOpenOrder_.append(dictionary.copy())
                    elif "FilledOrderMessage" == dictionary['MsgType']:
                        self.Executions_.append(dictionary.copy())
                else :
                    if "LimitOrderMessage" == dictionary["MsgType"]:
                        self.ExitOrder_.append(dictionary.copy())
                    elif "CancelLimitOrderMessage" == dictionary["MsgType"]:
                        self.ExitCancel_.append(dictionary.copy())
                    elif "CanceledLimitOrderMessage" == dictionary['MsgType']:
                        self.ExitCancel_.append(dictionary.copy())
                    elif "FilledOrderMessage" == dictionary['MsgType']:
                        self.Executions_.append(dictionary.copy())
                        self.OpenPosition += dictionary['Side'] * dictionary['Qty']
                        if self.OpenPosition == 0:
                            self.isInTrade = False



    def dump(self):
        row = [self.OpenOrder_['Ts'], self.OpenOrder_['Side'], self.OpenOrder_['Qty'],
               self.OpenOrder_['OrderId'], self.OpenOrder_['Px'], 
               self.MarketAfterOpen_[0]['BidQty'], self.MarketAfterOpen_[0]['BidPx'],
               self.MarketAfterOpen_[0]['AskPx'], self.MarketAfterOpen_[0]['AskQty'],
               self.MarketAfterOpen_[0]['TradePx'], self.MarketAfterOpen_[0]['Signal'],
               self.MarketAfterOpen_[0]['Volatility']]
        if len(self.Executions_) > 0:
            row.append(1)
            row.append(self.Executions_[0]['Px'])
            row.append(self.Executions_[1]['Px'])
        else:
            row.append(0)
            row.append(0)
            row.append(0)
        for ii in range(1, self.numUpdates_):
            row.append(( self.MarketAfterOpen_[min(ii, len(self.MarketAfterOpen_)-1)]['BidPx'] + self.MarketAfterOpen_[min(ii, len(self.MarketAfterOpen_)-1)]['AskPx'] ) / 2.0)
               
        return row



class LogProcess:
    def __init__(self, fname, numUpdates):
        self.filename_ = fname
        self.trades_ = []
        self.header_ = None
        self.currentMarket_ = None
        self.currentEvent_ = None
        self.openPosition_ = 0
        self.numUpdates_ = numUpdates

    def run(self):
        count = 0
        ff = open(self.filename_, 'r')
        for row in ff:
            count += 1
            if count == 1:
                self.header_ = parseHeader(row)
            else:
                if row.startswith("StrategyStatus"):
                    self.currentMarket_ = parseStrategyStatus(row, self.header_)
                    self.openPosition_ = self.currentMarket_['OpenPosition']
                    for trade in self.trades_:
                        trade.updateWithNewRow(self.currentMarket_)

                else :
                    if row.startswith("Acknowledged"):
                        continue
                    self.currentEvent_ = parseOrderMessage(row)
                    for trade in self.trades_:
                        trade.updateWithNewRow(self.currentEvent_)

                    if self.openPosition_ == 0 and self.currentEvent_['MsgType'] == "LimitOrderMessage":
                        self.trades_.append(Trade(self.numUpdates_, self.currentMarket_, self.currentEvent_))
        ff.close()



    def dump(self, output):
        ff = open(output, 'w')
        header = ['OrderTs', 'OrderSide', 'OrderQty', 'OrderId', 'OrderPx',
                  'BidQty', 'BidPx', 'AskPx', 'AskQty', 'TradePx', 'Signal', 'Volatility',
                  'isFilled', 'EntryPx', 'ExitPx']
        for ii in range(1, self.numUpdates_ + 1):
            header.append("Mid%d"%ii)
        ff.write(",".join(header)+"\n")
        for trade in self.trades_:
            row  = [str(it) for it in trade.dump()]
            ff.write(','.join(row)+"\n")
        ff.close()


if __name__ == "__main__":
    import sys
    if len(sys.argv) < 3:
        print "usage = processLog.py input_file output_file"
        exit(0)
    
    input_file = sys.argv[1]
    output_file = sys.argv[2]
    numUpdates = 120
    lp = LogProcess(input_file, numUpdates)
    lp.run()
    lp.dump(output_file)


    
        





    
