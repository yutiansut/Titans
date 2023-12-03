﻿#include <iostream>
#include <unistd.h>
#include <glog/logging.h>

#include "ia_etf_factors_ipc_output.h"


int set_thread_pool_num(){
    char value[32] = {0};
    int kernel_num = std::thread::hardware_concurrency();
    kernel_num *= 0.6;
    if(kernel_num > 8)
        kernel_num = 8;
    sprintf(value, "%d", kernel_num);
    //sprintf(value, "%d", kernel_num);
    setenv("UV_THREADPOOL_SIZE", value, 0);
    char *p; 
    if((p = getenv("UV_THREADPOOL_SIZE")))
        printf("UV_THREADPOOL_SIZE = %s\n",p); 
    return kernel_num;
}

int main(int argc, char* argv[]) {
    int thread_num = set_thread_pool_num();
	FLAGS_log_dir = "./log/";
    google::InitGoogleLogging(argv[0]);
	
    uv_loop_t* loop = uv_default_loop();
    
    IaETFFactorOutput* pilot = new IaETFFactorOutput(thread_num, loop, "./config.ini");
  
	return uv_run(loop, UV_RUN_DEFAULT);
}
