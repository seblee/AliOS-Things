/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#ifndef __APP_JSON_PARSE_H__
#define __APP_JSON_PARSE_H__

#include "aos/kernel.h"
int property_set_json_parse(const char *json_src);

int property_json_build(char **json);

#endif