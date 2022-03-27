#include <iostream>

#include "ConverterJSON.h"

int main() {

    auto config_file = std::ifstream("config.json");

    if(!config_file.is_open()){
        std::cerr << "config file is missing." << std::endl;
        return 1;
    }

    nlohmann::json conf;
    config_file >> conf;

    auto cf = config_json();

    if(conf["config"].is_null()){
        std::cerr << "config file is empty." << std::endl;
        return 1;
    }

    if(!conf["config"]["name"].is_null()){
        cf.config.name = conf["config"]["name"];
    }else{
        cf.config.name = "Undefined";
    }
    if(!conf["config"]["version"].is_null()) {
        cf.config.version = conf["config"]["version"];
    }
    cf.config.max_responses = conf["config"]["max_responses"];

    for (auto f: conf["files"]) {
        cf.files.push_back(f);
    }

    std::cout << cf.config.name << ((cf.config.version == "")? "" : " version: ") << cf.config.version << std::endl;

//    for (auto f: cf.files) {
//        std::cout << f << std::endl;
//    }

    return 0;

}
