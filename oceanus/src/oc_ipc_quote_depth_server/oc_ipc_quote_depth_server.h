#ifndef OC_IPC_QUOTE_DEPTH_SERVER_H
#define OC_IPC_QUOTE_DEPTH_SERVER_H

#include <string.h>
#include <vector>
#include "ti_quote_callback.h"
#include "oc_book_worker.h"
#include "ti_quote_worker_pool.h"
#include "ti_quote_ipc_publisher.h"
#include "ti_quote_worker.h"

class OcIpcQuoteDepthServer 
    : public TiQuoteCallback
{
public:
    OcIpcQuoteDepthServer();
    virtual ~OcIpcQuoteDepthServer(){};
private:
    int32_t m_cout_time;
    std::vector<TiQuoteCallback*>      m_worker_vec;
    TiQuoteWorkerPool*                 m_quote_worker_pool;
    TiQuoteIpcPublisher*               m_quote_ipc_publisher;
public:
    virtual void OnTradingDayRtn(const unsigned int day, const char* exchangeName){};
   
    virtual void OnL2IndexSnapshotRtn(const TiQuoteSnapshotIndexField* pData){};

    virtual void OnL2FutureSnapshotRtn(const TiQuoteSnapshotFutureField* pData);
    virtual void OnL2StockSnapshotRtn(const TiQuoteSnapshotStockField* pData);
    virtual void OnL2StockMatchesRtn(const TiQuoteMatchesField* pData);
    virtual void OnL2StockOrderRtn(const TiQuoteOrderField* pData);

};


#endif // OC_IPC_QUOTE_DEPTH_SERVER_H