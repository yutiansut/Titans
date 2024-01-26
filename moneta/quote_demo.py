#!/usr/bin/env python
# coding=utf-8

import pymoneta.quote as quote
import pymoneta.data as data
from datetime import datetime


def cb(type_, data_):
    print(type_, data_)
    
def main():
    #df = data.GetBar(datetime(2024,1,16), "SH", "600000", "1m")
    #df = data.GetBar(20240116, "SH", "600000", "1m")
    #df = data.GetMarket(20240116, "BNB", "*", "trades")

    #df = data.GetMarket("20230309", "SZ", "*", "matches", columns_=["timestamp", "exchange", "symbol", "price", "volume"])

    #df = data.GetDayBar("stock", ["600000", "000001"], "20210101", "20210131")
    #df = data.GetDayBar("index", "*", "20210101", "20210131")
    #df = data.GetDayBar("etf", "*", "20210101", "20210131")

    #df = quote.GetMarket(20240116, "SH", "*", "matches")

    df = data.GetBarBySymbol("BNB", "BTCUSDT", "1m", "20230101", "20240111 12:00:00")

    print(df)
    exit()
    quote.Init('127.0.0.1', 20184, 'lw', "P7pO48Lw4AZTOLXKlR")

    quote.SubQuote("snapshot", "SH", [
        "600000",
        "600004",
        "688788"
    ])

    quote.SubQuote("snapshot", "SZ", [
        "000001"
    ])

    quote.SubQuote("orders", "SH", [
        "600000",
        "600004",
        "688788"
    ])

    quote.SubQuote("orders", "SZ", [
        "000001"
    ])

    quote.SubQuote("matches", "SH", [
        "600000",
        "600004",
        "688788"
    ])

    quote.SubQuote("matches", "SZ", [
        "000001"
    ])


    quote.ReadLoop(cb)
        #time.sleep(1)
    pass

main()