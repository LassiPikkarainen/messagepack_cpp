#include <cstdlib>
#include <iostream>
#include <string>
#include "messagepack.h"

std::string b64_str = "3gAUoTOVpmFzMTIzZAMBA8pApmZmojEylaRhc2QxAwEDykCmZmaiMTOVo2FzZAMBA8pApmZmojIzlaRhczJkAwEDykCmZmajMTEylaRhc2QxAwEDykCmZmajMTIzlaZhczEyM2QDAQPKQKZmZqMyMTOVpmFzZDEyMwMBA8pApmZmozIzM5WkYXNkMgMBA8pApmZmpDExMTKVpGFzZDEDAQPKQKZmZqQxMTIylaRhc2QxAwEDykCmZmakMTEyM5WjYXNkAwEDykCmZmakMTIxM5WmYXNkMTIzAwEDykCmZmakMTIzM5WkYXNkMgMBA8pApmZmpDIzMTOVo2FzZAMBA8pApmZmpTExMjEzlaZhczEyM2QDAQPKQKZmZqUxMTIzM5WkYXNkMgMBA8pApmZmpTEyMzEzlaNhc2QDAQPKQKZmZqYxMTEyMTOVpmFzZDEyMwMBA8pApmZmpjExMjEzM5WkYXNkMgMBA8pApmZmpjExMjIyM5WkYXMyZAMBA8pApmZm";


int main(int argc, char** argv)
{

    std::cout << "B64: " << b64_str << std::endl;
    std::string json = MsgPack::MsgPackToJsonLike(b64_str);
    std::cout << "JSON: " << json << std::endl;
    std::string b64 = MsgPack::JsonLikeToMsgPack(json);
    std::cout << "B64: " << b64 << std::endl;
    json = MsgPack::MsgPackToJsonLike(b64);
    std::cout << "JSON: " << json << std::endl;
    return 0;
}
