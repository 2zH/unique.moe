#include <iostream>
#include <redox/redox.hpp>

#include <json/json.hpp>

#include <nghttp2/asio_http2_server.h>


using namespace nghttp2::asio_http2;
using namespace nghttp2::asio_http2::server;
using json = nlohmann::json;


generator_cb loading_templete(std::string path, const response &res) {
    auto fd = open(path.c_str(), O_RDONLY);

    if (fd == -1) {
        return 0;
    }

    return file_generator_from_fd(fd);
}

int main(int argc, char *argv[]) {
    boost::system::error_code ec;
    http2 server;

    //config
    std::string port = "3000";

    std::cout << "uniquehttp2 is loading..." << std::endl;

    server.handle("/", [](const request &req, const response &res) {

        auto body = loading_templete("../static/index.html", res);

        if (!body) {
            res.write_head(404);
            res.end();
        }

        res.write_head(200);
        res.end(body);

        // header_map mmp = req.header();
        
        // header_map::iterator pos;

        // for(pos = mmp.begin(); pos != mmp.end(); ++pos) {
        //     std::cout << "value for " << pos->first <<" can be " << (pos->second).value << std::endl;
        // }

        // log:
        // accept */*
        // user-agent curl/7.53.1

        // ***  another workaround *** //

        // auto headers = req.header();

        // for(const auto &header : headers) {
        //     std::cout << "value for " << header.first <<" can be " << (header.second).value << std::endl;
        // }
    });

    server.handle("/article/", [](const request &req, const response &res) {

        std::string uri = req.uri().path;

        json data;

        data["article_num"] = 1;
        data["article_header"] = "第一个篇章";
        data["article_info"] = {
            {"date", "2017/4/6"},
            {"author", "zH"},
            {"weather", "sunny"}
        };
        data["article_content"] = "这是unique.moe的第一篇测试用文章，也许今后不会有第二篇测试用文章了，所以请好好珍惜我！";

        res.write_head(200);
        res.end(data.dump());
    });

    std::cout << "welcome to uniquehttp2, master." << std::endl;

    if (server.listen_and_serve(ec, "localhost", port)) {
        std::cout << "my http port is " << port <<" , master." << std::endl;
        std::cerr << "error: " << ec.message() << std::endl;
    }else {
        std::cout << "server.listen_and_serv has undefined." << std::endl;
    }
}