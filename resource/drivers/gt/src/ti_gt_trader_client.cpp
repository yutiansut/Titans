#include "ti_gt_trader_client.h"
#include <glog/logging.h>
#include "iniFile.h"
#include "datetime.h"
#include "ti_encoding_tool.h"

#include <atomic>

TiGtTraderClient::TiGtTraderClient(std::string configPath, TiTraderCallback* userCb)
{
    m_config = NULL;
    m_trading_day = 0;
    m_client = NULL;

    nSessionId = 0;
    nOrderId = 1680000;

    m_cb = userCb;

    loadConfig(configPath);
    nReqId = 100;   //跳过xtp client设置成交模式的区段
};


TiGtTraderClient::~TiGtTraderClient()
{
    if(m_config){
        delete m_config;
        m_config = NULL;
    }
    m_cb = NULL;
    nReqId = 0;
};
////////////////////////////////////////////////////////////////////////
// 回调方法
////////////////////////////////////////////////////////////////////////

void TiGtTraderClient::work_cb(uv_work_t* req)
{
    std::cout << "TiGtTraderClient::work_cb" << std::endl;    
    TiGtTraderClient* client = (TiGtTraderClient*)req->data;
    client->m_client->join();
};

void TiGtTraderClient::after_work_cb(uv_work_t* req, int status)
{
    
};

////////////////////////////////////////////////////////////////////////
// 回调方法
////////////////////////////////////////////////////////////////////////

void TiGtTraderClient::onConnected(bool success, const char* errorMsg) 
{
    cout << "[onConnected] server connect " << (success ? string("success") : string("failure, err: ") + errorMsg) << endl;
    if (!success)
    {
        return;
    }
    std::cout << m_config->szUser.c_str() << " " << m_config->szPass.c_str() << std::endl;
    m_client->userLogin(m_config->szUser.c_str(), m_config->szPass.c_str(), nReqId++);
}

void TiGtTraderClient::onUserLogin(const char* userName, const char* password, int nRequestId, const XtError& error)
{
    cout << "[onUserLogin] login " << (error.isSuccess() ? "success" : string("failure, err: ") + error.errorMsg()) << endl;
}

void TiGtTraderClient::onRtnLoginStatus(const char* accountId, EBrokerLoginStatus status, int brokerType, const char* errorMsg)
{
    cout << "[onRtnLoginStatus] account id: " << accountId << ", type: " << brokerType << ", status: " << status << endl;

    switch (brokerType)
    {
    case AT_STOCK:
        m_config->m_strStockAccount = accountId;
        break;
    case AT_FUTURE:
        m_config->m_strFutureAccount = accountId;
        break;
    case AT_STOCK_OPTION:
        m_config->m_strOptionAccount = accountId;
        break;
    case AT_CREDIT:
        m_config->m_strCreditAccount = accountId;
        break;
    default:
        break;
    }

    m_client->reqAccountDetail(accountId, ++nReqId);
}

 // 资金账号信息
void TiGtTraderClient::onReqAccountDetail(const char* accountId, int nRequestId, const CAccountDetail* data, bool isLast, const XtError& error)
{
    cout << "[onReqAccountDetail]  资金账号 :" << data->m_strAccountID
        << "\n    账号状态:" << data->m_strStatus
        << "\n    交易日:" << data->m_strTradingDate
        << "\n    可用资金:" << data->m_dAvailable
        << "\n    净资产:" << data->m_dAssureAsset
        << "\n    总负债:" << data->m_dTotalDebit
        << "\n    期初权益:" << data->m_dPreBalance
        << endl;
}

void TiGtTraderClient::onOrder(int nRequestId, int orderId, const char* strRemark, const XtError& error)
{
    cout << "[onOrder] isSuccess: " << (error.isSuccess()?"true":"false")
        << "\n    orderId:  " << orderId
        << "\n    RequestId: " << nRequestId  
        << "\n    errorMsg: " << error.errorMsg()
        << endl;
}

void TiGtTraderClient::onRtnOrder(const COrderInfo* data)
{
    string orderStatus = "";
    switch(data->m_eStatus)
    {
    case OCS_CHECKING:   orderStatus = "风控检查中";  break;
    case OCS_APPROVING:  orderStatus = "审批中";  break;
    case OCS_REJECTED:   orderStatus = "已驳回";  break;
    case OCS_RUNNING:    orderStatus = "运行中";  break;
    case OCS_CANCELING:  orderStatus = "撤销中";  break;
    case OCS_FINISHED:   orderStatus = "已完成";  break;
    case OCS_STOPPED:    orderStatus = "已撤销";  break;
    }

    cout << "[onRtnOrder]"
        << "\n    下单ID: " << data->m_nOrderID
        << "\n    m_startTime：" << data->m_startTime
        << "\n    m_endTime: " << data->m_endTime
        << "\n    指令状态：" << orderStatus
        << "\n    成交量：" << data->m_dTradedVolume
        << "\n    撤销者：" << data->m_canceler
        << "\n    指令执行信息：" << data->m_strMsg
        << endl;
}

void TiGtTraderClient::onRtnOrderDetail(const COrderDetail* data)
{
    string entrust_status;
    switch(data->m_eOrderStatus)
    {
    case ENTRUST_STATUS_UNREPORTED:  entrust_status = "未报";  break;
    case ENTRUST_STATUS_WAIT_REPORTING:  entrust_status = "待报"; break;
    case ENTRUST_STATUS_REPORTED:        entrust_status = "已报"; break;
    case ENTRUST_STATUS_REPORTED_CANCEL: entrust_status = "已报待撤";  break;
    case ENTRUST_STATUS_PARTSUCC_CANCEL: entrust_status = "部成待撤";  break;
    case ENTRUST_STATUS_PART_CANCEL:     entrust_status = "部撤";  break;
    case ENTRUST_STATUS_CANCELED:        entrust_status = "已撤";  break;
    case ENTRUST_STATUS_PART_SUCC:       entrust_status = "部成";  break;
    case ENTRUST_STATUS_SUCCEEDED:       entrust_status = "已成";  break;
    case ENTRUST_STATUS_JUNK:            entrust_status = "废单";  break;
    }
    if (data == NULL)
    {
        return;
    }
    cout << "[onRtnOrderDetail]"
        << "\n    委托号：" << data->m_strOrderSysID
        << "\n    委托状态：" << entrust_status
        << "\n    已成交量：" << data->m_nTradedVolume 
        << "\n    成交均价：" << data->m_dAveragePrice 
        << "\n    成交额: " << data->m_dTradeAmount 
        << "\n    市场ID：" << data->m_strExchangeID
        << "\n    产品ID：" << data->m_strProductID
        << "\n    股票/期货代码：" << data->m_strInstrumentID
        << "\n    冻结保证金：" << data->m_dFrozenMargin
        << "\n    冻结手续费：" << data->m_dFrozenCommission
        << "\n    ErrorID：" << data->m_nErrorID
        << "\n    ErrorMsg: " << data->m_strErrorMsg
        << endl;

}

void TiGtTraderClient::onRtnDealDetail(const CDealDetail* data)
{
    if (data == NULL)
    {
        return;
    }
    cout << "[onRtnDealDetail]"
        << "\n    orderId: " << data->m_nOrderID 
        << "\n    成交量： " << data->m_nVolume
        << "\n    成交额: " << data->m_dAmount
        << "\n    成交均价： " << data->m_dAveragePrice
        << endl;
}

void TiGtTraderClient::onRtnOrderError(const COrderError* data)
{
    if (data == NULL)
    {
        return;
    }
    cout << "[onRtnOrderError] orderId: " << data->m_nOrderID 
        << "\n    error id: " << data->m_nErrorID
        << "\n    errormsg: " << data->m_strErrorMsg
        << "\n    m_nRequestID: " << data->m_nRequestID
        << "\n    m_nOrderID: " << data->m_nOrderID
        << endl;
}
    
////////////////////////////////////////////////////////////////////////
// 私有工具方法
////////////////////////////////////////////////////////////////////////

int TiGtTraderClient::loadConfig(std::string iniFileName){
    if(m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }

	IniFile _iniFile;
    _iniFile.load(iniFileName);

    m_config = new ConfigInfo();
    m_config->szLocations           = string(_iniFile["ti_gt_trader_client"]["locations"]);
    
    m_config->szUser                = string(_iniFile["ti_gt_trader_client"]["user"]);
    m_config->szPass                = string(_iniFile["ti_gt_trader_client"]["pass"]);
    
    if( m_config->szLocations.empty() |
        m_config->szUser.empty() |
        m_config->szPass.empty())
    {
        LOG(INFO) << "[loadConfig] Not enough parameters in inifile";
        delete m_config;
        m_config = NULL;
        return -1;
    }
    return 0;
};


int TiGtTraderClient::orderInsertStock(TiReqOrderInsert* req){
    req->nReqId = ++nReqId;

    return nReqId;
};
int TiGtTraderClient::orderInsertEtf(TiReqOrderInsert* req){
    req->nReqId = ++nReqId;


    return nReqId;
};

void TiGtTraderClient::connect(){
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return;
    }
    if (m_client)   //如果已经实例了就退出
    {
        return;
    }

    m_client = XtTraderApi::createXtTraderApi(m_config->szLocations.c_str());
    if (NULL == m_client)
    {
        cout << "[init] create api failure" << endl;
        return;
    }

    m_client->setCallback(this);
    m_client->init("../config");

    //m_client->join_async();

    memset(&m_work_req, 0, sizeof(uv_work_t));
    m_work_req.data = this;

    uv_queue_work(uv_default_loop(), &m_work_req, TiGtTraderClient::work_cb, TiGtTraderClient::after_work_cb);
};


int TiGtTraderClient::orderInsert(TiReqOrderInsert* req){
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }
    if(req->nBusinessType == TI_BusinessType_Stock){
        return orderInsertStock(req);
    }else if(req->nBusinessType == TI_BusinessType_ETF){
        return orderInsertEtf(req);
    }
    return -1;
};

int TiGtTraderClient::orderDelete(TiReqOrderDelete* req){
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }

    TiRtnOrderStatus* order = getOrderStatus(-1, req->nOrderId);

    auto iter = m_order_map.begin();
    for (; iter != m_order_map.end(); iter++)
    {

        std::cout << iter->first << ":" << iter->second->szOrderStreamId << std::endl;
        /* code */
    }
    
    std::cout << order << std::endl;
    if(!order)
    {
        return -1;
    }

    return nReqId;
};

TiRtnOrderStatus* TiGtTraderClient::getOrderStatus(int64_t req_id, int64_t order_id)
{
    if (m_order_map.find(order_id) != m_order_map.end())
    {
        return m_order_map[order_id].get();
    }
    auto iter = m_order_map.begin();
    for( ; iter != m_order_map.end(); iter++)
    {
        if (iter->second->nReqId == req_id)
        {
            return iter->second.get();
        }
    }
    return NULL;
};
    

int TiGtTraderClient::QueryAsset()
{
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }


    return nReqId;
};

int TiGtTraderClient::QueryOrders()
{
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }
    
    return nReqId;
};

int TiGtTraderClient::QueryMatches()
{
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }
    

    return nReqId;
};

int TiGtTraderClient::QueryPositions()
{
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }

    return nReqId;
};


int TiGtTraderClient::QueryETFFile()
{
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }

    return nReqId;
};

int TiGtTraderClient::QueryETFBasket()
{
    if(!m_config){
        LOG(INFO) << "[loadConfig] Do not have config info";
        return -1;
    }
    
    return nReqId;
};

