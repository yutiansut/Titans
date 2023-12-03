#include "oc_book_worker.h"
#include <string.h>

OcBookWorker::OcBookWorker(int32_t id, TiQuoteCallback* callback, TiQuoteDepthCallback* depth_callback)
{
    m_book_engine = new TiBookEngine(this, 0, 10);
    memset(&m_book_snap_cash, 0, sizeof(TiQuoteSnapshotStockField));
    m_cout_time = 0;
    m_id = id;
    m_callback = callback;
    m_depth_callback = depth_callback;
}

OcBookWorker::~OcBookWorker()
{
    delete m_book_engine;
}


void OcBookWorker::OnL2FutureSnapshotRtn(const TiQuoteSnapshotFutureField* pData)
{
    if (!m_id)
    {
        if ((pData->time - m_cout_time) > 1000)
        {
            printf("[OcBookWorker::OnL2FutureSnapshotRtn] %s, %s, %d, %d, %s, %f, %ld, %ld\n", 
                pData->symbol, pData->exchange, pData->time, m_cout_time, pData->time_str, pData->last, pData->acc_volume, pData->acc_turnover);
            m_cout_time = pData->time;
        }
    }
    m_callback->OnL2FutureSnapshotRtn(pData);
};

void OcBookWorker::OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData)
{    
    OnDepthSnapshotRtn(pData);
    if(m_book_engine){
        m_book_engine->RecvStockSnap(pData);
    }
};
void OcBookWorker::OnL2StockMatchesRtn(const TiQuoteMatchesField* pData){
    m_book_engine->RecvMatch(pData);
};
void OcBookWorker::OnL2StockOrderRtn(const TiQuoteOrderField* pData){
    m_book_engine->RecvOrder(pData);
};

void OcBookWorker::OnDepthSnapshotRtn(const TiQuoteDepthSnapshotBaseField* pBase, 
    const std::vector<TiQuoteDepthLevel*> &asks, 
    const std::vector<TiQuoteDepthLevel*> &bids)
{
    if (asks.size() < 10 || bids.size() < 10)
    {
        return;
    }
    strcpy(m_book_snap_cash.exchange, pBase->exchange);
    strcpy(m_book_snap_cash.symbol, pBase->symbol);
    m_book_snap_cash.date = pBase->date;
    m_book_snap_cash.time = pBase->time;
    m_book_snap_cash.timestamp = pBase->timestamp;
    m_book_snap_cash.last = pBase->last;
    m_book_snap_cash.pre_close = pBase->pre_close;
    m_book_snap_cash.open = pBase->open;
    m_book_snap_cash.high = pBase->high;
    m_book_snap_cash.low = pBase->low;
    m_book_snap_cash.high_limit = pBase->high_limit;
    m_book_snap_cash.low_limit = pBase->low_limit;
    m_book_snap_cash.acc_volume = pBase->acc_volume;
    m_book_snap_cash.acc_turnover = pBase->acc_turnover;
    m_book_snap_cash.match_items = pBase->match_items;

    
    for (size_t i = 0; i < 10; i++)
    {
        m_book_snap_cash.ask_price[i] = asks[i]->price;
        m_book_snap_cash.ask_volume[i] = asks[i]->volume;
        m_book_snap_cash.ask_order_num[i] = asks[i]->order_count;
        m_book_snap_cash.bid_price[i] = bids[i]->price;
        m_book_snap_cash.bid_volume[i] = bids[i]->volume;
        m_book_snap_cash.bid_order_num[i] = bids[i]->order_count;
    }

    OnDepthSnapshotRtn(&m_book_snap_cash);
};

void OcBookWorker::OnDepthOrderBookLevelRtn(const TiQuoteOrderBookField* pData)
{
    m_depth_callback->OnDepthOrderBookLevelRtn(pData);
};

void OcBookWorker::OnDepthSnapshotRtn(const TiQuoteSnapshotStockField* pData)
{
    if (!m_id)
    {
        if ((pData->time - m_cout_time) > 1000)
        {
            printf("[OcBookWorker::OnL2StockSnapshotRtn] %s, %s, %d, %d, %s, %f, %ld, %ld\n", 
                pData->symbol, pData->exchange, pData->time, m_cout_time, pData->time_str, pData->last, pData->acc_volume, pData->acc_turnover);
            m_cout_time = pData->time;
        }
    }
    m_callback->OnL2StockSnapshotRtn(pData);
};