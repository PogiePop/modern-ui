#pragma once
#include <string>
#include <functional>

namespace ui {

class Navigation {
public:
    static void openUrl(const std::string& url);
    static void openFolder(const std::string& path);
    static void openFile(const std::string& path);

    // Open native file picker dialog (sync — use async version for non-blocking)
    static std::string pickFile(const std::string& filter = "All Files (*.*)\0*.*\0");
    static std::string pickFolder();
    // Async: runs in background thread, callback on main thread via AsyncTaskQueue
    static void pickFileAsync(const std::string& filter, std::function<void(const std::string&)> onDone);
};

} // namespace ui
