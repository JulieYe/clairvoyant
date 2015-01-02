//
// File:        $ ThostStructToString.cpp $
//
// Version:     $ Revision: 1.0 $
//
// Modified:    $ Date: 2013/02/12 16:36:00 $
//
// ID:          $ mzhang $
//
// Copyright (c) 2012-2012 by Zhengzhang Futures Co. Ltd.

#include "ThostStructToString.hpp"

namespace simulator {
    
    string convertStructToString(CThostFtdcDepthMarketDataField *pData, string delimiter)
    {
	stringstream ss;
	ss << "TradingDay=" << pData->TradingDay << delimiter;
	ss << "InstrumentID=" << pData->InstrumentID << delimiter;
	ss << "ExchangeID=" << pData->ExchangeID << delimiter;
	ss << "ExchangeInstID=" << pData->ExchangeInstID << delimiter;
	ss << "LastPrice=" << pData->LastPrice << delimiter;
	ss << "PreSettlementPrice=" << pData->PreSettlementPrice << delimiter;
	ss << "PreClosePrice=" << pData->PreClosePrice << delimiter;
	ss << "PreOpenInterest=" << pData->PreOpenInterest << delimiter;
	ss << "OpenPrice=" << pData->OpenPrice << delimiter;
	ss << "HighestPrice=" << pData->HighestPrice << delimiter;
	ss << "LowestPrice=" << pData->LowestPrice << delimiter;
	ss << "Volume=" << pData->Volume << delimiter;
	ss << "Turnover=" << pData->Turnover << delimiter;
	ss << "OpenInterest=" << pData->OpenInterest << delimiter;
	ss << "ClosePrice=" << pData->ClosePrice << delimiter;
	ss << "SettlementPrice=" << pData->SettlementPrice <<delimiter;
	ss << "UpperLimitPrice=" << pData->UpperLimitPrice <<delimiter;
	ss << "LowerLimitPrice=" << pData->LowerLimitPrice <<delimiter;
	ss << "PreDelta=" << pData->PreDelta <<delimiter;
	ss << "CurrDelta=" << pData->CurrDelta <<delimiter;
	ss << "UpdateTime=" << pData->UpdateTime <<delimiter;
	ss << "UpdateMillisec=" << pData->UpdateMillisec <<delimiter;
	ss << "BidPrice1=" << pData->BidPrice1 <<delimiter;
	ss << "BidVolume1=" << pData->BidVolume1 <<delimiter;
	ss << "AskPrice1=" << pData->AskPrice1 <<delimiter;
	ss << "AskVolume1=" << pData->AskVolume1 <<delimiter;
	ss << "BidPrice2=" << pData->BidPrice2 <<delimiter;
	ss << "BidVolume2=" << pData->BidVolume2 <<delimiter;
	ss << "AskPrice2=" << pData->AskPrice2 <<delimiter;
	ss << "AskVolume2=" << pData->AskVolume2 <<delimiter;
	ss << "BidPrice3=" << pData->BidPrice3 <<delimiter;
	ss << "BidVolume3=" << pData->BidVolume3 <<delimiter;
	ss << "AskPrice3=" << pData->AskPrice3 <<delimiter;
	ss << "AskVolume3=" << pData->AskVolume3 <<delimiter;
	ss << "BidPrice4=" << pData->BidPrice4 <<delimiter;
	ss << "BidVolume4=" << pData->BidVolume4 <<delimiter;
	ss << "AskPrice4=" << pData->AskPrice4 <<delimiter;
	ss << "AskVolume4=" << pData->AskVolume4 <<delimiter;
	ss << "BidPrice5=" << pData->BidPrice5 <<delimiter;
	ss << "BidVolume5=" << pData->BidVolume5 <<delimiter;
	ss << "AskPrice5=" << pData->AskPrice5 <<delimiter;
	ss << "AskVolume5=" << pData->AskVolume5 <<delimiter;
	ss << "AveragePrice=" << pData->AveragePrice;
	return ss.str();
    }

    string convertStructToString(CThostFtdcTradeField * pTrade, string delimiter)
    {
	stringstream ss;
	ss << "BrokerID=" << pTrade->BrokerID <<delimiter;
	ss << "InvestorID=" << pTrade->InvestorID <<delimiter;
	ss << "InstrumentID=" << pTrade->InstrumentID <<delimiter;
	ss << "OrderRef=" << pTrade->OrderRef <<delimiter;
	ss << "UserID=" << pTrade->UserID <<delimiter;
	ss << "ExchangeID=" << pTrade->ExchangeID <<delimiter;
	ss << "TradeID=" << pTrade->TradeID <<delimiter;
	ss << "Direction=" << pTrade->Direction <<delimiter;
	ss << "OrderSysID=" << pTrade->OrderSysID <<delimiter;
	ss << "ParticipantID=" << pTrade->ParticipantID <<delimiter;
	ss << "ClientID=" << pTrade->ClientID <<delimiter;
	ss << "TradingRole=" << pTrade->TradingRole <<delimiter;
	ss << "ExchangeInstID=" << pTrade->ExchangeInstID <<delimiter;
	ss << "OffsetFlag=" << pTrade->OffsetFlag <<delimiter;
	ss << "HedgeFlag=" << pTrade->HedgeFlag <<delimiter;
	ss << "Price=" << pTrade->Price <<delimiter;
	ss << "Volume=" << pTrade->Volume <<delimiter;
	ss << "TradeDate=" << pTrade->TradeDate <<delimiter;
	ss << "TradeTime=" << pTrade->TradeTime <<delimiter;
	ss << "TradeType=" << pTrade->TradeType <<delimiter;
	ss << "PriceSource=" << pTrade->PriceSource <<delimiter;
	ss << "TraderID=" << pTrade->TraderID <<delimiter;
	ss << "OrderLocalID=" << pTrade->OrderLocalID <<delimiter;
	ss << "ClearingPartID=" << pTrade->ClearingPartID <<delimiter;
	ss << "BusinessUnit=" << pTrade->BusinessUnit <<delimiter;
	ss << "SequenceNo=" << pTrade->SequenceNo <<delimiter;
	ss << "TradingDay=" << pTrade->TradingDay <<delimiter;
	ss << "SettlementID=" << pTrade->SettlementID <<delimiter;
	ss << "BrokerOrderSeq=" << pTrade->BrokerOrderSeq <<delimiter;
	ss << "TradeSource=" << pTrade->TradeSource;

	return ss.str();


    }

    string convertStructToString(CThostFtdcOrderField * pOrder, string delimiter)
    {
	stringstream ss;
	ss << "BrokerID=" << pOrder->BrokerID << delimiter;
	ss << "InvestorID=" << pOrder->InvestorID << delimiter;
	ss << "InstrumentID=" << pOrder->InstrumentID << delimiter;
	ss << "OrderRef=" << pOrder->OrderRef << delimiter;
	ss << "UserID=" << pOrder->UserID << delimiter;
	ss << "OrderPriceType=" << pOrder->OrderPriceType << delimiter;
	ss << "Direction=" << pOrder->Direction << delimiter;
	ss << "CombOffsetFlag=" << pOrder->CombOffsetFlag << delimiter;
	ss << "CombHedgeFlag=" << pOrder->CombHedgeFlag << delimiter;
	ss << "LimitPrice=" << pOrder->LimitPrice << delimiter;
	ss << "VolumeTotalOriginal=" << pOrder->VolumeTotalOriginal << delimiter;
	ss << "TimeCondition=" << pOrder->TimeCondition << delimiter;
	ss << "GTDDate=" << pOrder->GTDDate << delimiter;
	ss << "VolumeCondition=" << pOrder->VolumeCondition << delimiter;
	ss << "MinVolume=" << pOrder->MinVolume << delimiter;
	ss << "ContingentCondition=" << pOrder->ContingentCondition << delimiter;
	ss << "StopPrice=" << pOrder->StopPrice << delimiter;
	ss << "ForceCloseReason=" << pOrder->ForceCloseReason << delimiter;
	ss << "IsAutoSuspend=" << pOrder->IsAutoSuspend << delimiter;
	ss << "BusinessUnit=" << pOrder->BusinessUnit << delimiter;
	ss << "RequestID=" << pOrder->RequestID << delimiter;
	ss << "OrderLocalID=" << pOrder->OrderLocalID << delimiter;
	ss << "ExchangeID=" << pOrder->ExchangeID << delimiter;
	ss << "ParticipantID=" << pOrder->ParticipantID << delimiter;
	ss << "ClientID=" << pOrder->ClientID << delimiter;
	ss << "ExchangeInstID=" << pOrder->ExchangeInstID << delimiter;
	ss << "TraderID=" << pOrder->TraderID << delimiter;
	ss << "InstallID=" << pOrder->InstallID << delimiter;
	ss << "OrderSubmitStatus=" << pOrder->OrderSubmitStatus << delimiter;
	ss << "NotifySequence=" << pOrder->NotifySequence << delimiter;
	ss << "TradingDay=" << pOrder->TradingDay << delimiter;
	ss << "SettlementID=" << pOrder->SettlementID << delimiter;
	ss << "OrderSysID=" << pOrder->OrderSysID << delimiter;
	ss << "OrderSource=" << pOrder->OrderSource << delimiter;
	ss << "OrderStatus=" << pOrder->OrderStatus << delimiter;
	ss << "OrderType=" << pOrder->OrderType << delimiter;
	ss << "VolumeTraded=" << pOrder->VolumeTraded << delimiter;
	ss << "VolumeTotal=" << pOrder->VolumeTotal << delimiter;
	ss << "InsertDate=" << pOrder->InsertDate << delimiter;
	ss << "InsertTime=" << pOrder->InsertTime << delimiter;
	ss << "ActiveTime=" << pOrder->ActiveTime << delimiter;
	ss << "SuspendTime=" << pOrder->SuspendTime << delimiter;
	ss << "UpdateTime=" << pOrder->UpdateTime << delimiter;
	ss << "CancelTime=" << pOrder->CancelTime << delimiter;
	ss << "ActiveTraderID=" << pOrder->ActiveTraderID << delimiter;
	ss << "ClearingPartID=" << pOrder->ClearingPartID << delimiter;
	ss << "SequenceNo=" << pOrder->SequenceNo << delimiter;
	ss << "FrontID=" << pOrder->FrontID << delimiter;
	ss << "SessionID=" << pOrder->SessionID << delimiter;
	ss << "UserProductInfo=" << pOrder->UserProductInfo << delimiter;
	ss << "StatusMsg=" << pOrder->StatusMsg << delimiter;
	ss << "UserForceClose=" << pOrder->UserForceClose << delimiter;
	ss << "ActiveUserID=" << pOrder->ActiveUserID << delimiter;
	ss << "BrokerOrderSeq=" << pOrder->BrokerOrderSeq << delimiter;
	ss << "RelativeOrderSysID=" << pOrder->RelativeOrderSysID << delimiter;
	ss << "ZCETotalTradedVolume=" << pOrder->ZCETotalTradedVolume << delimiter;
	ss << "IsSwapOrder=" << pOrder->IsSwapOrder;
	return ss.str();


    }

    string convertStructToString(CThostFtdcInputOrderField * pInput, string delimiter)
    {
	stringstream ss;
	ss << "BrokerID=" << pInput->BrokerID<< delimiter;
	ss << "InvestorID=" << pInput->InvestorID<< delimiter;
	ss << "InstrumentID=" << pInput->InstrumentID<< delimiter;
	ss << "OrderRef=" << pInput->OrderRef<< delimiter;
	ss << "UserID=" << pInput->UserID<< delimiter;
	ss << "OrderPriceType=" << pInput->OrderPriceType<< delimiter;
	ss << "Direction=" << pInput->Direction<< delimiter;
	ss << "CombOffsetFlag=" << pInput->CombOffsetFlag<< delimiter;
	ss << "CombHedgeFlag=" << pInput->CombHedgeFlag<< delimiter;
	ss << "LimitPrice=" << pInput->LimitPrice<< delimiter;
	ss << "VolumeTotalOriginal=" << pInput->VolumeTotalOriginal<< delimiter;
	ss << "TimeCondition=" << pInput->TimeCondition<< delimiter;
	ss << "GTDDate=" << pInput->GTDDate<< delimiter;
	ss << "VolumeCondition=" << pInput->VolumeCondition<< delimiter;
	ss << "MinVolume=" << pInput->MinVolume<< delimiter;
	ss << "ContingentCondition=" << pInput->ContingentCondition<< delimiter;
	ss << "StopPrice=" << pInput->StopPrice<< delimiter;
	ss << "ForceCloseReason=" << pInput->ForceCloseReason<< delimiter;
	ss << "IsAutoSuspend=" << pInput->IsAutoSuspend<< delimiter;
	ss << "BusinessUnit=" << pInput->BusinessUnit<< delimiter;
	ss << "RequestID=" << pInput->RequestID<< delimiter;
	ss << "UserForceClose=" << pInput->UserForceClose<< delimiter;
	ss << "IsSwapOrder=" << pInput->IsSwapOrder;
	

	return ss.str();

    }

    string convertStructToString(CThostFtdcOrderActionField *pAction, string delimiter)
    {
	stringstream ss;
	ss << "BrokerID=" << pAction->BrokerID << delimiter;
	ss << "InvestorID=" << pAction->InvestorID << delimiter;
	ss << "OrderActionRef=" << pAction->OrderActionRef << delimiter;
	ss << "OrderRef=" << pAction->OrderRef << delimiter;
	ss << "RequestID=" << pAction->RequestID << delimiter;
	ss << "FrontID=" << pAction->FrontID << delimiter;
	ss << "SessionID=" << pAction->SessionID << delimiter;
	ss << "ExchangeID=" << pAction->ExchangeID << delimiter;
	ss << "OrderSysID=" << pAction->OrderSysID << delimiter;
	ss << "ActionFlag=" << pAction->ActionFlag << delimiter;
	ss << "LimitPrice=" << pAction->LimitPrice << delimiter;
	ss << "VolumeChange=" << pAction->VolumeChange << delimiter;
	ss << "ActionDate=" << pAction->ActionDate << delimiter;
	ss << "ActionTime=" << pAction->ActionTime << delimiter;
	ss << "TraderID=" << pAction->TraderID << delimiter;
	ss << "InstallID=" << pAction->InstallID << delimiter;
	ss << "OrderLocalID=" << pAction->OrderLocalID << delimiter;
	ss << "ActionLocalID=" << pAction->ActionLocalID << delimiter;
	ss << "ParticipantID=" << pAction->ParticipantID << delimiter;
	ss << "ClientID=" << pAction->ClientID << delimiter;
	ss << "BusinessUnit=" << pAction->BusinessUnit << delimiter;
	ss << "OrderActionStatus=" << pAction->OrderActionStatus << delimiter;
	ss << "UserID=" << pAction->UserID << delimiter;
	ss << "StatusMsg=" << pAction->StatusMsg << delimiter;
	ss << "InstrumentID=" << pAction->InstrumentID;
	return ss.str();



    }

    string convertStructToString(CThostFtdcInputOrderActionField * pAction, string delimiter)
    {
	stringstream ss;
	ss << "BrokerID=" << pAction->BrokerID<< delimiter;
	ss << "InvestorID=" << pAction->InvestorID<< delimiter;
	ss << "OrderActionRef=" << pAction->OrderActionRef<< delimiter;
	ss << "OrderRef=" << pAction->OrderRef<< delimiter;
	ss << "RequestID=" << pAction->RequestID<< delimiter;
	ss << "FrontID=" << pAction->FrontID<< delimiter;
	ss << "SessionID=" << pAction->SessionID<< delimiter;
	ss << "ExchangeID=" << pAction->ExchangeID<< delimiter;
	ss << "OrderSysID=" << pAction->OrderSysID<< delimiter;
	ss << "ActionFlag=" << pAction->ActionFlag<< delimiter;
	ss << "LimitPrice=" << pAction->LimitPrice<< delimiter;
	ss << "VolumeChange=" << pAction->VolumeChange<< delimiter;
	ss << "UserID=" << pAction->UserID<< delimiter;
	ss << "InstrumentID=" << pAction->InstrumentID;
	
	return ss.str();
    }
    
    string convertStructToString(CThostFtdcSettlementInfoField * pSettlementInfo, string delimiter)
    {
	stringstream ss;
	ss << "TradingDay=" << pSettlementInfo->TradingDay << delimiter;
	ss << "SettlementID=" << pSettlementInfo->SettlementID << delimiter;
	ss << "BrokderID=" << pSettlementInfo->BrokerID << delimiter;
	ss << "InvestorID=" << pSettlementInfo->InvestorID << delimiter;
	ss << "SequenceNo=" << pSettlementInfo->SequenceNo << delimiter;
	ss << "Content=" << pSettlementInfo->Content;
	return ss.str();

    }


}

