#include <iostream>

#include <boost/algorithm/string.hpp>

#include <redox/redox.hpp>

#include <json/json.hpp>

#include <nghttp2/asio_http2_server.h>


using namespace nghttp2::asio_http2;
using namespace nghttp2::asio_http2::server;
using json = nlohmann::json;


generator_cb loading_file(std::string path, const response &res) {
    auto fd = open(path.c_str(), O_RDONLY);

    if (fd == -1) {
        return 0;
    }

    return file_generator_from_fd(fd);
}

void server_push(std::string path, std::string type, const response &res) {
    boost::system::error_code ec;

    std::string content_type = (type == "js") ? "application/javascript; charset=utf-8" : "text/css";
    if (path == "service-worker.js") {
        auto serviceWorker = res.push(ec, "GET", "/service-worker.js");
        serviceWorker -> write_head(200, {{"Content-Type", {content_type}}});
        serviceWorker -> end(loading_file("../frontend/react-unique/build/service-worker.js", res));
    } else {
        auto push = res.push(ec, "GET", "/static/" + type + "/" + path);
        push -> write_head(200, {{"Content-Type", {content_type}}});
        push -> end(loading_file("../frontend/react-unique/build/static/" + type + "/" + path, res));

        auto srcMap = res.push(ec, "GET", "/static/" + type + "/" + path + ".map");
        srcMap -> write_head(200, {{"Content-Type", {content_type}}});
        srcMap -> end(loading_file("../frontend/react-unique/build/static/" + type + "/" + path + ".map", res));
    }
}

void server_push_js(std::string path, const response &res) {
    server_push(path, "js", res);
}

void server_push_css(std::string path, const response &res) {
    server_push(path, "css", res);
}

std::string getParamters(std::string uri, int count) {
    std::vector<std::string> strs;
    boost::split(strs, uri, boost::is_any_of("/"));

    return strs[count];
}

int main(int argc, char *argv[]) {
    boost::system::error_code ec;
    boost::asio::ssl::context tls(boost::asio::ssl::context::sslv23);

    tls.use_private_key_file("../certificate/privkey1.pem", boost::asio::ssl::context::pem);
    tls.use_certificate_chain_file("../certificate/cert1.pem");

    configure_tls_context_easy(ec, tls);

    http2 server;

    redox::Redox rdx;
    rdx.connect("172.17.0.3", 6379);

    //config
    std::string port = "443";

    std::cout << "uniquehttp2 is loading..." << std::endl;

    // std::string style_css = "h2 { color: #5e5e5e; }";

    server.handle("/asset-manifest.json", [](const request &req, const response &res) {
        auto body = loading_file("../frontend/react-unique/build/asset-manifest.json", res);

        if (!body) {
            res.write_head(404);
            res.end();
        }
        
        res.write_head(200, {{"Content-Type", {"application/json"}}});
        res.end(body);
    });

    server.handle("/", [](const request &req, const response &res) {

        auto body = loading_file("../frontend/react-unique/build/index.html", res);

	// boost::system::error_code ec;
	// auto push = res.push(ec, "GET", "/static/css/main.938b0da0.css");

	// push -> write_head(200, {{"Content-Type", {"text/css"}}});
	// push -> end(style_css);
        server_push_js("main.b2846986.js", res);
        server_push_js("service-worker.js", res);
        server_push_js("0.8c823e06.chunk.js", res);
        server_push_js("1.133299bd.chunk.js", res);
        server_push_js("2.52d08a14.chunk.js", res);
        server_push_js("3.621a5132.chunk.js", res);
        server_push_js("4.4d09f838.chunk.js", res);
        server_push_css("main.938b0da0.css", res);

        if (!body) {
            res.write_head(404);
            res.end();
        }

	std::cout << "_(///w///」∠)_·´ `·⊂З| || 没钱好痛苦啊...\n" << std::endl;

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

        json data;

        data["method"] = method;
        data["request_params"] = article_id;
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

        header_map cors_header;
        if (method == "OPTIONS") {
            cors_header = {
                {"Access-Control-Allow-Origin", {"*"}},
                {"Access-Control-Allow-Methods", {"GET, POST, OPTIONS"}},
                {"Access-Control-Allow-Headers", {"DNT,X-CustomHeader,Keep-Alive,User-Agent,X-Requested-With,If-Modified-Since,Cache-Control,Content-Type,Content-Range,Range"}},
                {"Access-Control-Max-Age", {"1728000"}},
                {"Content-Type", {"text/plain charset=UTF-8"}},
                {"Content-Length", {"0"}}
            };
            res.write_head(204, cors_header);
            res.end();
        }else {
            cors_header = {
                {"Access-Control-Allow-Origin", {"*"}},
                {"Access-Control-Allow-Methods", {"GET, POST, OPTIONS"}},
                {"Access-Control-Allow-Headers", {"DNT,X-CustomHeader,Keep-Alive,User-Agent,X-Requested-With,If-Modified-Since,Cache-Control,Content-Type,Content-Range,Range"}},
                {"Content-Type", {"application/json"}}
            };
            res.write_head(200, cors_header);
            res.end(data.dump());
        }
    });

    std::cout << "welcome to uniquehttp2, master." << std::endl;

    if (server.listen_and_serve(ec, tls, "0.0.0.0", port)) {
        std::cout << "my http port is " << port <<" , master." << std::endl;
        std::cerr << "error: " << ec.message() << std::endl;
    }else {
        std::cout << "server.listen_and_serv has undefined." << std::endl;
    }
}
