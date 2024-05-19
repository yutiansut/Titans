#ifndef __IA_ETF_WORKER_BUY_ETF_H__
#define __IA_ETF_WORKER_BUY_ETF_H__

#include "ia_etf_trading_worker.h"

#include <set>
#include <unordered_map>
#include <memory.h>

class IaETFWorkerBuyEtf :
    public IaETFTradingWorker
{
public:
    struct Status
    {
        std::string symbol;
        std::string exchange;
        double expect_cost;
        int32_t volume;
        double real_cost;
        int32_t finish_volume;
    }; 
public:
    virtual void OnCommonJsonRespones(const json* rspData, int req_id, bool isLast, int err, const char* err_str){};     //非交易逻辑的统一实现接口
    
    virtual void OnRspAccountInfo(const TiRspAccountInfo* pData){};

    virtual void OnRspOrderDelete(const TiRspOrderDelete* pData);
    
    virtual void OnRspQryOrder(const TiRspQryOrder* pData, bool isLast){};
    virtual void OnRspQryMatch(const TiRspQryMatch* pData, bool isLast){};
    virtual void OnRspQryPosition(const TiRspQryPosition* pData, bool isLast){};

    virtual void OnRtnOrderStatusEvent(const TiRtnOrderStatus* pData);
    virtual void OnRtnOrderMatchEvent(const TiRtnOrderMatch* pData){};

public:
    virtual void OnTimer();
private:
    Status m_status;

    TiQuoteSnapshotStockField m_open_snap;
    std::set<int64_t> m_req_id_set;                                 // order_req_id
    std::unordered_map<std::string, TiRtnOrderStatus> m_order_map;  // szOrderStreamId -> order_status
private:
    void updateExpectCost(TiQuoteSnapshotStockField* pData);
    void updateStatus();
    bool hasQueueOrder();
    double getOrderPrice(TiQuoteSnapshotStockField* pData);
public:
    int64_t open();
    bool isOver();
public:
    IaETFWorkerBuyEtf(TiTraderClient* client, IaEtfQuoteDataCache* m_quote_cache, IaEtfSignalFactorPtr factor, std::string account);
    virtual ~IaETFWorkerBuyEtf(){};
};

typedef std::shared_ptr<IaETFWorkerBuyEtf> IaETFWorkerBuyEtfPtr;

#endif