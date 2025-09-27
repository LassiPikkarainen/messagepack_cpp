# cpp_messagepack

Simple utility for convering base 64 encoded messagepack strings to json-like strings and back. Everything needed is contained in a single header.

Note that the string generated from a messagepack may not be completely json-compliant as the messagepack specification does not have the same key:value pairing as json (except for maps). 



## Usage
- Include the messagepack.h -header 
- Call `MsgPack::MsgPackToJsonLike(std::string b64_encoded)` to decode a messagepack string to a json-like string
- Call `MsgPack::JsonLikeToMsgPack(std::string json)` to encode a json-string to a base64 encoded messagepack