#include <cstdlib>
#include <iostream>
#include <string>


class MsgPack
{
public:

    /**
    * Convert MessagePack (in base64 format) to JSON string.
    *
    * @param b64_str string containing base64 encoded MessagePack data
    * @return std::string JSON representation of the MessagePack data
    */
    static std::string MsgPackToJson(const std::string& b64_str) 
    {
        std::vector<uint8_t> decoded_msgpack = base64_decode(b64_str);
        size_t offset = 0;
        std::string json_result = parse_msgpack_var(decoded_msgpack, offset);
        return json_result;

    }


private:

    // Prevent instantiation as this is a static class
    MsgPack() = delete;


    // Base64 encoding/decoding functions
    static std::string base64_encode(const std::string &in) 
    {
        std::string out;

        int val = 0, valb = -6;
        for (u_char c : in) {
            val = (val << 8) + c;
            valb += 8;
            while (valb >= 0) {
                out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[(val>>valb)&0x3F]);
                valb -= 6;
            }
        }
        if (valb>-6) out.push_back("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[((val<<8)>>(valb+8))&0x3F]);
        while (out.size()%4) out.push_back('=');
        return out;
    }

    static std::vector<uint8_t> base64_decode(const std::string &in) 
    {
        std::vector<uint8_t> out;

        std::vector<int> T(256,-1);
        for (int i=0; i<64; i++) T["ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[i]] = i;

        int val=0, valb=-8;
        for (u_char c : in) {
            if (T[c] == -1) break;
            val = (val << 6) + T[c];
            valb += 6;
            if (valb >= 0) {
                out.push_back(uint8_t((val>>valb)&0xFF));
                valb -= 8;
            }
        }
        return out;
    }

    /*

    MessagePack specification
    -------------------------

    positive fixint 	0xxxxxxx 	0x00 - 0x7f
    fixmap 	            1000xxxx 	0x80 - 0x8f
    fixarray 	        1001xxxx 	0x90 - 0x9f
    fixstr 	            101xxxxx 	0xa0 - 0xbf
    nil 	            11000000 	0xc0
    (never used) 	    11000001 	0xc1
    false 	            11000010 	0xc2
    true 	            11000011 	0xc3
    bin 8 	            11000100 	0xc4
    bin 16 	            11000101 	0xc5
    bin 32 	            11000110 	0xc6
    ext 8 	            11000111 	0xc7
    ext 16 	            11001000 	0xc8
    ext 32 	            11001001 	0xc9
    float 32 	        11001010 	0xca
    float 64 	        11001011 	0xcb
    uint 8 	            11001100 	0xcc
    uint 16 	        11001101 	0xcd
    uint 32 	        11001110 	0xce
    uint 64 	        11001111 	0xcf
    int 8 	            11010000 	0xd0
    int 16 	            11010001 	0xd1
    int 32 	            11010010 	0xd2
    int 64 	            11010011 	0xd3
    fixext 1 	        11010100 	0xd4
    fixext 2 	        11010101 	0xd5
    fixext 4 	        11010110 	0xd6
    fixext 8 	        11010111 	0xd7
    fixext 16 	        11011000 	0xd8
    str 8 	            11011001 	0xd9
    str 16 	            11011010 	0xda
    str 32 	            11011011 	0xdb
    array 16 	        11011100 	0xdc
    array 32 	        11011101 	0xdd
    map 16 	            11011110 	0xde
    map 32 	            11011111 	0xdf
    negative fixint 	111xxxxx 	0xe0 - 0xff
    */

    // Recursive function to parse MessagePack data starting from a given offset
    static std::string parse_msgpack_var(const std::vector<uint8_t>& data, size_t& offset) {

        // check validity of given data
        if (offset >= data.size()) {
            throw std::runtime_error("Offset out of bounds");
        }
        uint8_t type = data[offset];
        
        /*
            For each basic type, read the necessary bytes, update the offset,
            and return the corresponding JSON representation as a string.
        
            For compound types (maps and arrays), recursively call this function
            to parse each element or key-value pair.

        */
    
        // positive fixint (0x00 - 0x7f)
        if (type>= 0x00 && type <= 0x7f) 
        { 
            offset += 1;
            std::cout << "Positive fixint: " << static_cast<int>(type) << std::endl;
            return std::to_string(type); // the number is stored directly in the type byte
        } 

        // fixmap
        else if (type >= 0x80 && type <= 0x8f) 
        { 
            size_t map_size = type & 0x0f;
            std::cout << "Fixmap, size: " << map_size << std::endl;
            offset += 1;

            // format of a map-object: {"key1": value1, "key2": value2, ...}
            // loop each key-value pair and append to result string
            std::string result = "{";
            for (size_t i = 0; i < map_size; ++i) {
                if (i > 0) result += ", ";

                result += parse_msgpack_var(data, offset);
                result += ": ";
                result += parse_msgpack_var(data, offset);
                
            }
            result += "}";
            std::cout << "Map ended"<< std::endl;
            return result;
        } 

        // fixarray
        else if (type >= 0x90 && type <= 0x9f) 
        { 
            size_t array_size = type & 0x0f;
            offset += 1;
            // format of an array: [value1, value2, ...]
            // can be anything, call parse_msgpack_var recursively
            std::string result = "[";
            std::cout << "Fixarray, size: " << array_size << std::endl;
            for (size_t i = 0; i < array_size; ++i) {
                if (i > 0) result += ", ";
                result += parse_msgpack_var(data, offset);
            }
            result += "]";
            std::cout << "Array ended"<< std::endl;
            return result;
        } 

        // fixstr
        else if (type >= 0xa0 && type <= 0xbf) 
        {
            size_t str_size = type & 0x1f;
            offset += 1;
            if (offset + str_size > data.size()) {
                throw std::runtime_error("String size out of bounds");
            }
            std::string str(reinterpret_cast<const char*>(&data[offset]), str_size); 
            std::cout << "Fixstr, size: " << str_size << ", value: " << str << std::endl;
            offset += str_size;
            return "\"" + str + "\""; // add double quotes for JSON string
        } 

        // nil
        else if (type == 0xc0) 
        { 
            offset += 1;
            std::cout << "Nil value"<< std::endl;
            return "null";
        } 

        // not used
        else if (type == 0xc1) 
        { 
            throw std::runtime_error("Invalid Message Pack type: 0xc1");
        }


        // false
        else if (type == 0xc2) 
        { 
            offset += 1;
            std::cout << "Boolean false"<< std::endl;
            return "false";
        } 

        // true
        else if (type == 0xc3) 
        { 
            offset += 1;
            std::cout << "Boolean true"<< std::endl;
            return "true";
        } 

        // bin 8 (array of bytes)
        else if (type == 0xc4)
        {
            if (offset + 2 > data.size()) {
                throw std::runtime_error("Not enough data for bin 8");
            }
            uint8_t bin_size = data[offset + 1];
            offset += 2;
            if (offset + bin_size > data.size()) {
                throw std::runtime_error("Binary size out of bounds");
            }
            std::string bin_data(reinterpret_cast<const char*>(&data[offset]), bin_size);
            offset += bin_size;
            std::cout << "Bin 8, size: " << static_cast<int>(bin_size) << std::endl;
            return "\"" + bin_data + "\""; 
        }

        // bin 16
        else if (type == 0xc5) 
        {
            if (offset + 3 > data.size()) {
                throw std::runtime_error("Not enough data for bin 16");
            }
            uint16_t bin_size = (data[offset + 1] << 8) | data[offset + 2];
            offset += 3;
            if (offset + bin_size > data.size()) {
                throw std::runtime_error("Binary size out of bounds");
            }
            std::string bin_data(reinterpret_cast<const char*>(&data[offset]), bin_size);
            offset += bin_size;
            std::cout << "Bin 16, size: " << bin_size << std::endl;
            return "\"" + bin_data + "\""; 
        }

        // bin 32
        else if (type == 0xc6) 
        {
            if (offset + 5 > data.size()) {
                throw std::runtime_error("Not enough data for bin 32");
            }
            uint32_t bin_size = (data[offset + 1] << 24) | (data[offset + 2] << 16) | (data[offset + 3] << 8) | data[offset + 4];
            offset += 5;
            if (offset + bin_size > data.size()) {
                throw std::runtime_error("Binary size out of bounds");
            }
            std::string bin_data(reinterpret_cast<const char*>(&data[offset]), bin_size);
            offset += bin_size;
            std::cout << "Bin 32, size: " << bin_size << std::endl;
            return "\"" + bin_data + "\"";
        }


        // TODO ext types
        else if (type == 0xc7 || type == 0xc8 || type == 0xc9) 
        {
            throw std::runtime_error("Extension types not supported");
        }

        // float32
        else if (type == 0xca) 
        {
            if (offset + 5 > data.size()) {
                throw std::runtime_error("Not enough data for float32");
            }
            uint32_t int_repr = (data[offset + 1] << 24) | (data[offset + 2] << 16) | (data[offset + 3] << 8) | data[offset + 4];
            float value;
            std::memcpy(&value, &int_repr, sizeof(float));
            std::cout << "Float32: " << value << std::endl;
            offset += 5;
            return std::to_string(value);
        
        }

        // float64
        else if (type == 0xcb) 
        {
            if (offset + 9 > data.size()) {
                throw std::runtime_error("Not enough data for float64");
            }
            uint64_t int_repr = (static_cast<uint64_t>(data[offset + 1]) << 56) | (static_cast<uint64_t>(data[offset + 2]) << 48) |
                                (static_cast<uint64_t>(data[offset + 3]) << 40) | (static_cast<uint64_t>(data[offset + 4]) << 32) |
                                (static_cast<uint64_t>(data[offset + 5]) << 24) | (static_cast<uint64_t>(data[offset + 6]) << 16) |
                                (static_cast<uint64_t>(data[offset + 7]) << 8) | static_cast<uint64_t>(data[offset + 8]);
            double value;
            std::memcpy(&value, &int_repr, sizeof(double));
            offset += 9;
            std::cout << "Float64: " << value << std::endl;
            return std::to_string(value);
        
        }

        // uint8
        else if (type == 0xcc) 
        { 
            if (offset + 2 > data.size()) {
                throw std::runtime_error("Not enough data for uint8");
            }
            uint8_t value = data[offset + 1];
            offset += 2;
            std::cout << "Uint8: " << static_cast<int>(value) << std::endl;
            return std::to_string(value);
        } 

        // uint16
        else if (type == 0xcd) 
        { 
            if (offset + 3 > data.size()) {
                throw std::runtime_error("Not enough data for uint16");
            }
            uint16_t value = (data[offset + 1] << 8) | data[offset + 2];
            offset += 3;
            std::cout << "Uint16: " << value << std::endl;
            return std::to_string(value);
        } 

        // uint32
        else if (type == 0xce) 
        { 
            if (offset + 5 > data.size()) {
                throw std::runtime_error("Not enough data for uint32");
            }
            uint32_t value = (data[offset + 1] << 24) | (data[offset + 2] << 16) | (data[offset + 3] << 8) | data[offset + 4];
            offset += 5;
            std::cout << "Uint32: " << value << std::endl;
            return std::to_string(value);
        }
        
        // uint64
        else if (type == 0xcf) 
        { 
            if (offset + 9 > data.size()) {
                throw std::runtime_error("Not enough data for uint64");
            }
            uint64_t value = (static_cast<uint64_t>(data[offset + 1]) << 56) | (static_cast<uint64_t>(data[offset + 2]) << 48) |
                            (static_cast<uint64_t>(data[offset + 3]) << 40) | (static_cast<uint64_t>(data[offset + 4]) << 32) |
                            (static_cast<uint64_t>(data[offset + 5]) << 24) | (static_cast<uint64_t>(data[offset + 6]) << 16) |
                            (static_cast<uint64_t>(data[offset + 7]) << 8) | static_cast<uint64_t>(data[offset + 8]);
            offset += 9;
            std::cout << "Uint64: " << value << std::endl;
            return std::to_string(value);
        } 

        // int8
        else if (type == 0xd0) 
        { 
            if (offset + 2 > data.size()) {
                throw std::runtime_error("Not enough data for int8");
            }
            int8_t value = static_cast<int8_t>(data[offset + 1]);
            offset += 2;
            std::cout << "Int8: " << static_cast<int>(value) << std::endl;
            return std::to_string(value);
        } 

        // int16
        else if (type == 0xd1) 
        { 
            if (offset + 3 > data.size()) {
                throw std::runtime_error("Not enough data for int16");
            }
            int16_t value = (data[offset + 1] << 8) | data[offset + 2];
            offset += 3;
            std::cout << "Int16: " << value << std::endl;
            return std::to_string(value);
        } 
        
        // int32
        else if (type == 0xd2) {
            if (offset + 5 > data.size()) {
                throw std::runtime_error("Not enough data for int32");
            }
            int32_t value = (data[offset + 1] << 24) | (data[offset + 2] << 16) | (data[offset + 3] << 8) | data[offset + 4];
            offset += 5;
            std::cout << "Int32: " << value << std::endl;
            return std::to_string(value);
        }

        // int64
        else if (type == 0xd3) 
        { 
            if (offset + 9 > data.size()) {
                throw std::runtime_error("Not enough data for int32");
            }

            int64_t value = (static_cast<int64_t>(data[offset + 1]) << 56) | (static_cast<int64_t>(data[offset + 2]) << 48) |
                            (static_cast<int64_t>(data[offset + 3]) << 40) | (static_cast<int64_t>(data[offset + 4]) << 32) |
                            (static_cast<int64_t>(data[offset + 5]) << 24) | (static_cast<int64_t>(data[offset + 6]) << 16) |
                            (static_cast<int64_t>(data[offset + 7]) << 8) | static_cast<int64_t>(data[offset + 8]);
        
            offset += 9;
            std::cout << "Int64: " << value << std::endl;
            return std::to_string(value);
        }

        else if (type == 0xd4 || type == 0xd5 || type == 0xd6 || type == 0xd7 || type == 0xd8) 
        {
            throw std::runtime_error("Fixextension types not supported");
        }

        // str 8
        else if (type == 0xd9) 
        { 
            if (offset + 2 > data.size()) {
                throw std::runtime_error("Not enough data for str 8");
            }
            uint8_t str_size = data[offset + 1];
            offset += 2;
            if (offset + str_size > data.size()) {
                throw std::runtime_error("String size out of bounds");
            }
            std::string str(reinterpret_cast<const char*>(&data[offset]), str_size);
            offset += str_size;
            std::cout << "Str 8, size: " << static_cast<int>(str_size) << ", value: " << str << std::endl;
            return "\"" + str + "\"";
        }

        // str 16
        else if (type == 0xda) 
        { 
            if (offset + 3 > data.size()) {
                throw std::runtime_error("Not enough data for str 16");
            }
            uint16_t str_size = (data[offset + 1] << 8) | data[offset + 2];
            offset += 3;
            if (offset + str_size > data.size()) {
                throw std::runtime_error("String size out of bounds");
            }
            std::string str(reinterpret_cast<const char*>(&data[offset]), str_size);
            offset += str_size;
            std::cout << "Str 16, size: " << str_size << ", value: " << str << std::endl;
            return "\"" + str + "\"";
        }

        // str 32
        else if (type == 0xdb) 
        { 
            if (offset + 5 > data.size()) {
                throw std::runtime_error("Not enough data for str 32");
            }
            uint32_t str_size = (data[offset + 1] << 24) | (data[offset + 2] << 16) | (data[offset + 3] << 8) | data[offset + 4];
            offset += 5;
            if (offset + str_size > data.size()) {
                throw std::runtime_error("String size out of bounds");
            }
            std::string str(reinterpret_cast<const char*>(&data[offset]), str_size);
            offset += str_size;
            std::cout << "Str 32, size: " << str_size << ", value: " << str << std::endl;
            return "\"" + str + "\"";
        }

        // array 16
        else if (type == 0xdc) 
        { 
            if (offset + 3 > data.size()) {
                throw std::runtime_error("Not enough data for array 16");
            }
            uint16_t array_size = (data[offset + 1] << 8) | data[offset + 2];
            std::cout << "Array 16, size: " << array_size << std::endl;
            offset += 3;
            std::string result = "[";
            for (size_t i = 0; i < array_size; ++i) {
                if (i > 0) result += ", ";
                result += parse_msgpack_var(data, offset);
            }
            result += "]";
            std::cout << "Array ended"<< std::endl;
            return result;
        } 

        // array 32
        else if (type == 0xdd) { 
            if (offset + 5 > data.size()) {
                throw std::runtime_error("Not enough data for array 32");
            }
            uint32_t array_size = (data[offset + 1] << 24) | (data[offset + 2] << 16) | (data[offset + 3] << 8) | data[offset + 4];
            std::cout << "Array 32, size: " << array_size << std::endl;
            offset += 5;
            std::string result = "[";
            for (size_t i = 0; i < array_size; ++i) {
                if (i > 0) result += ", ";
                result += parse_msgpack_var(data, offset);
            }
            result += "]";
            std::cout << "Array ended"<< std::endl;
            return result;
        }

        // map 16
        else if (type == 0xde)
        { 
            if (offset + 3 > data.size()) {
                throw std::runtime_error("Not enough data for map 16");
            }
            uint16_t map_size = (data[offset + 1] << 8) | data[offset + 2];
            std::cout << "Map 16, size: " << map_size << std::endl;
            offset += 3;
            std::string result = "{";
            for (size_t i = 0; i < map_size; ++i) {
                if (i > 0) result += ", ";
                result += parse_msgpack_var(data, offset);
                result += ": ";
                result += parse_msgpack_var(data, offset);
            }
            result += "}";
            std::cout << "Map ended"<< std::endl;
            return result;
        }

        // map 32
        else if (type == 0xdf)
        { 
            if (offset + 5 > data.size()) {
                throw std::runtime_error("Not enough data for map 32");
            }
            uint32_t map_size = (data[offset + 1] << 24) | (data[offset + 2] << 16) | (data[offset + 3] << 8) | data[offset + 4];
            std::cout << "Map 32, size: " << map_size << std::endl;
            offset += 5;
            std::string result = "{";
            for (size_t i = 0; i < map_size; ++i) {
                if (i > 0) result += ", ";
                result += parse_msgpack_var(data, offset);
                result += ": ";
                result += parse_msgpack_var(data, offset);
            }
            result += "}";
            std::cout << "Map ended"<< std::endl;
            return result;

        }
        // negative fixint
        else if ((type & 0xe0) == 0xe0) { 
            offset += 1;
            std::cout << "Negative fixint: " << static_cast<int8_t>(type) << std::endl;
            return std::to_string(static_cast<int8_t>(type));
        } 

        else {
            throw std::runtime_error("Unsupported MessagePack type: " + std::to_string(type));
        }

    }

};