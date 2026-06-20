#pragma once
#include <string>
#include <functional>
#include <map>

namespace ui {

struct HttpResponse {
    int statusCode = 0;
    std::string body;
    std::map<std::string, std::string> headers;
    bool ok() const { return statusCode >= 200 && statusCode < 300; }
};

using HttpCallback = std::function<void(const HttpResponse&)>;

class HttpClient {
public:
    static HttpResponse get(const std::string& url);
    static HttpResponse post(const std::string& url, const std::string& body, const std::string& contentType = "application/json");

    // Async (returns immediately, calls callback when done)
    static void getAsync(const std::string& url, HttpCallback callback);
    static void postAsync(const std::string& url, const std::string& body, HttpCallback callback);
};

} // namespace ui
