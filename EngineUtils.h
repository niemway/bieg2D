//
//  EngineUtils.h
//  game_engine
//
//  Created by Nicholas Way on 1/29/24.
//

#ifndef EngineUtils_h
#define EngineUtils_h

#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include <filesystem>
#include <iostream>

#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"

class EngineUtils {
    
public:
    
    static void ReadJsonFile(const std::string& path, rapidjson::Document& out_document)
    {
        FILE* file_pointer = nullptr;
    #ifdef _WIN32
        fopen_s(&file_pointer, path.c_str(), "rb");
    #else
        file_pointer = fopen(path.c_str(), "rb");
    #endif
        char buffer[65536];
        rapidjson::FileReadStream stream(file_pointer, buffer, sizeof(buffer));
        out_document.ParseStream(stream);
        std::fclose(file_pointer);

        if (out_document.HasParseError()) {
//            rapidjson::ParseErrorCode errorCode = out_document.GetParseError();
            std::cout << "error parsing json at [" << path << "]";
            exit(0);
        }
    }
    
    static bool DirectoryExists(const std::string& path)
    {
        return std::filesystem::exists(path);
    }
    
    static uint64_t create_composite_key(int x, int y) {
        // cast to ensure the ints become exactly 32 bits in size.
        uint32_t ux = static_cast<uint32_t>(x);
        uint32_t uy = static_cast<uint32_t>(y);
        // place x into right 32 bits.
        uint64_t result = static_cast<uint64_t>(ux);
        // move x to left 32 bits.
        result = result << 32;
        // place y into right 32 bits.
        result = result | static_cast<uint64_t>(uy);
        return result;
    }
    
    static std::vector<std::string> GetFilesInDirectory(std::string directory){
        std::vector<std::string> files;
        for (const auto & entry : std::filesystem::directory_iterator(directory)){
            // ignore hidden files
            if (entry.path().filename().string().at(0) == '.'){
                continue;
            }
            files.push_back(entry.path().string());
        }
        return files;
    }
    
    static void ReportError(const std::string & actor_name, const luabridge::LuaException & e){
        std::string error_message = e.what();
        /* Normalize file paths across platforms */
        std::replace(error_message.begin(), error_message.end(), '\\', '/');
        /* Display (with color codes) */
        std:: cout << "\033[31m" << actor_name << " : " << error_message << "\033[0m" << std::endl;
    }
    
    static void ReportError(const std::string & message){
        std:: cout << "\033[31m" << message << "\033[0m" << std::endl;
    }
};
#endif /* EngineUtils_h */
