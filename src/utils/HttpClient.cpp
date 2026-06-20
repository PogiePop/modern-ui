#include "HttpClient.hpp"
#include <cstdio>
#include <cstring>
#include <thread>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#define closesocket close
#endif

namespace ui {

static bool g_wsaInit = false;

static void initWSA() {
#ifdef _WIN32
    if (!g_wsaInit) { WSADATA d; WSAStartup(MAKEWORD(2,2), &d); g_wsaInit = true; }
#endif
}

static int createSocket(const std::string& host, int port) {
    initWSA();
    struct addrinfo hints{}, *res;
    hints.ai_family = AF_INET; hints.ai_socktype = SOCK_STREAM;
    char portStr[16]; snprintf(portStr, sizeof(portStr), "%d", port);
    if (getaddrinfo(host.c_str(), portStr, &hints, &res) != 0) return -1;
    int sock = (int)socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock < 0 || connect(sock, res->ai_addr, (int)res->ai_addrlen) < 0) {
        if (sock >= 0) closesocket(sock); freeaddrinfo(res); return -1;
    }
    freeaddrinfo(res);
    return sock;
}

static HttpResponse doRequest(const std::string& method, const std::string& url, const std::string& body = "", const std::string& ct = "") {
    HttpResponse resp;
    std::string host, path;
    int port = 80;
    // Parse URL
    const char* p = url.c_str();
    bool isHttps = false;
    if (strncmp(p, "http://", 7) == 0) { p += 7; }
    else if (strncmp(p, "https://", 8) == 0) { p += 8; port = 443; isHttps = true; }
    if (isHttps) { resp.statusCode = -2; resp.body = "HTTPS not supported by HttpClient; use Navigation::openUrl for browser-based HTTPS"; return resp; }
    const char* slash = strchr(p, '/');
    if (slash) { host.assign(p, slash - p); path = slash; }
    else { host = p; path = "/"; }
    const char* colon = strchr(host.c_str(), ':');
    if (colon) { port = atoi(colon+1); host = host.substr(0, colon - host.c_str()); }

    int sock = createSocket(host, port);
    if (sock < 0) { resp.statusCode = -1; return resp; }

    std::string req = method + " " + path + " HTTP/1.1\r\nHost: " + host + "\r\n";
    if (!body.empty()) req += "Content-Type: " + ct + "\r\nContent-Length: " + std::to_string(body.size()) + "\r\n";
    req += "Connection: close\r\n\r\n" + body;

    send(sock, req.c_str(), (int)req.size(), 0);

    char buf[4096]; std::string raw;
    int n; while ((n = recv(sock, buf, sizeof(buf)-1, 0)) > 0) { buf[n] = 0; raw += buf; }
    closesocket(sock);

    // Parse response
    size_t hdrEnd = raw.find("\r\n\r\n");
    if (hdrEnd != std::string::npos) {
        std::string hdr = raw.substr(0, hdrEnd);
        resp.body = raw.substr(hdrEnd + 4);
        size_t sp = hdr.find(' '); if (sp != std::string::npos) resp.statusCode = atoi(hdr.c_str() + sp + 1);
    }
    return resp;
}

HttpResponse HttpClient::get(const std::string& url) { return doRequest("GET", url); }
HttpResponse HttpClient::post(const std::string& url, const std::string& body, const std::string& ct) { return doRequest("POST", url, body, ct); }

void HttpClient::getAsync(const std::string& url, HttpCallback cb) {
    std::thread([url, cb]() { auto r = get(url); if (cb) cb(r); }).detach();
}
void HttpClient::postAsync(const std::string& url, const std::string& body, HttpCallback cb) {
    std::thread([url, body, cb]() { auto r = post(url, body); if (cb) cb(r); }).detach();
}

} // namespace ui
