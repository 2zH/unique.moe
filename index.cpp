#include <iostream>

#include <boost/algorithm/string.hpp>

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

std::string getParamters(std::string uri, int count) {
    std::vector<std::string> strs;
    boost::split(strs, uri, boost::is_any_of("/"));

    return strs[count];
}

int main(int argc, char *argv[]) {
    boost::system::error_code ec;
    boost::asio::ssl::context tls(boost::asio::ssl::context::sslv23);

    tls.use_private_key_file("../static/cert/privkey1.pem", boost::asio::ssl::context::pem);
    tls.use_certificate_chain_file("../static/cert/cert1.pem");

    configure_tls_context_easy(ec, tls);

    http2 server;

    redox::Redox rdx;
    rdx.connect("172.17.0.3", 6379);

    //config
    std::string port = "443";

    std::cout << "uniquehttp2 is loading..." << std::endl;

    std::string style_css = "h2 { color: #5e5e5e; }";

    server.handle("/", [&style_css](const request &req, const response &res) {

        auto body = loading_templete("../static/index.html", res);

	boost::system::error_code ec;
	auto push = res.push(ec, "GET", "/style.css");

	push -> write_head(200, {{"Content-Type", {"text/css"}}});
	push -> end(style_css);

        if (!body) {
            res.write_head(404);
            res.end();
        }

	std::cout << "some one find me!\n" << std::endl;

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

    server.handle("/api/article/", [&rdx](const request &req, const response &res) {
        std::string article_id = getParamters(req.uri().path, 3);

        auto method = req.method();

        if (method == "POST") {
            //data
        }

        json data;

        data["method"] = method;

        // if (!article_id) {
            // json articles;
            // std::vector<std::string> tips = {"author", "content", "date"};
            // int total = stoi(rdx.get("article-count"));

            // for (int i = 0;i < total; i ++) {
            //     json article;
            //     std::string n = std::to_string(i + 1);
            //     for (const auto& tip : tips) {
            //         article[tip] = rdx.get("article-" + n + "-" + tip);
            //     }
            //     articles[i] = article;
            // }
            // data["data"] = articles;
            // data["total"] = rdx.get("article-count");
            // data["msg"] = "hello " + rdx.get("hello");
        // }

        // data["article_id"] = article_id;
        // data["article_header"] = "第一个篇章";
        // data["article_info"] = {
        //     {"date", "2017/4/6"},
        //     {"author", "zH"},
        //     {"weather", "sunny"}
        // };
        // data["article_content"] = "这是unique.moe的第一篇测试用文章，也许今后不会有第二篇测试用文章了，所以请好好珍惜我！";

        res.write_head(200);
        res.end(data.dump());
    });

    std::cout << "welcome to uniquehttp2, master." << std::endl;

    if (server.listen_and_serve(ec, tls, "0.0.0.0", port)) {
        std::cout << "my http port is " << port <<" , master." << std::endl;
        std::cerr << "error: " << ec.message() << std::endl;
    }else {
        std::cout << "server.listen_and_serv has undefined." << std::endl;
    }
}
