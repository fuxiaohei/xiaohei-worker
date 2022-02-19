/*
 * Copyright (c) 2021 fuxiaohei. All rights reserved.
 * Licensed under the Apache-2.0 License. See License file in the project root for
 * license information.
 */

#include <core/main_context.h>
#include <hv/hlog.h>
#include <stdio.h>

int main(int argc, char **argv) {
  // set logger handler
  hlog_set_level(LOG_LEVEL_INFO);
  hlog_set_handler(stdout_logger);
  // init main, use for commandline
  return core::MainContext::Run(argc, argv);
}
