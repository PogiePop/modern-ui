#include "Navigation.hpp"
#include "AsyncTask.hpp"
#include <cstdio>
#include <cstring>
#include <thread>
#include <functional>

#ifdef _WIN32
#include <windows.h>
#include <shellapi.h>
#include <commdlg.h>
#include <shlobj.h>
#else
#include <cstdlib>
#endif

namespace ui {

std::string Navigation::pickFile(const std::string& filter) {
#ifdef _WIN32
    char buf[1024] = {};
    OPENFILENAMEA ofn = {};
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFilter = filter.c_str();
    ofn.lpstrFile = buf; ofn.nMaxFile = sizeof(buf);
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
    if (GetOpenFileNameA(&ofn)) return buf;
#endif
    return "";
}

void Navigation::pickFileAsync(const std::string& filter, std::function<void(const std::string&)> onDone) {
    std::thread([filter, onDone]() {
        std::string result = pickFile(filter);
        AsyncTaskQueue::instance().post([onDone, result]() { onDone(result); });
    }).detach();
}

std::string Navigation::pickFolder() {
#ifdef _WIN32
    char buf[1024] = {};
    BROWSEINFOA bi = {};
    bi.pszDisplayName = buf;
    bi.lpszTitle = "Select Folder";
    bi.ulFlags = BIF_RETURNONLYFSDIRS;
    LPITEMIDLIST pidl = SHBrowseForFolderA(&bi);
    if (pidl) { SHGetPathFromIDListA(pidl, buf); CoTaskMemFree(pidl); return buf; }
#endif
    return "";
}

void Navigation::openUrl(const std::string& url) {
#ifdef _WIN32
    ShellExecuteA(nullptr, "open", url.c_str(), nullptr, nullptr, SW_SHOW);
#else
    std::string cmd = "xdg-open \"" + url + "\"";
    system(cmd.c_str());
#endif
}

void Navigation::openFolder(const std::string& path) {
#ifdef _WIN32
    ShellExecuteA(nullptr, "explore", path.c_str(), nullptr, nullptr, SW_SHOW);
#else
    std::string cmd = "xdg-open \"" + path + "\"";
    system(cmd.c_str());
#endif
}

void Navigation::openFile(const std::string& path) {
#ifdef _WIN32
    ShellExecuteA(nullptr, "open", path.c_str(), nullptr, nullptr, SW_SHOW);
#else
    openUrl("file://" + path);
#endif
}

} // namespace ui
