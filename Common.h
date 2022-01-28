#pragma once

enum class METHOD {
    GET,POST,PUT,PATCH,DEL,INVALID
};

enum class HTTP_PROTOCOL {
    HTTP1_0,HTTP1_1,HTTP2_0,HTTP3_0,INVALID
};

#define RUN_FOREVER while(1)

typedef unsigned char byte;