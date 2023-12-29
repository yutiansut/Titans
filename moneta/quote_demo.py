#!/usr/bin/env python
# coding=utf-8

import pymoneta.quote as quote
from datetime import datetime


def cb(type_, data_):
    print(type_, data_)
    
def main():
    quote.Init('127.0.0.1', 20184, 'lw')

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