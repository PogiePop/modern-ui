#include "Config.hpp"
#include "utils/ThreadPool.hpp"
#include <cstdio>
#include <cstring>
#include <cstdlib>

namespace ui {

Config& Config::instance() {
    static Config cfg;
    return cfg;
}

void Config::apply() {
    // Thread pool
    if (multiThreaded) ThreadPool::init(renderThreads);

    // VSync — set via GLFW, deferred to App::init
}

bool Config::loadFromFile(const char* path) {
    FILE* f = fopen(path, "rb");
    if (!f) return false;
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    std::string content(size, 0);
    fread(&content[0], 1, size, f);
    fclose(f);

    // Simple key=value parser
    const char* p = content.c_str();
    while (*p) {
        while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r' || *p == '#') {
            if (*p == '#') while (*p && *p != '\n') p++; else p++;
        }
        if (!*p) break;
        std::string key, val;
        while (*p && *p != '=' && *p != '\n') key += *p++;
        if (*p == '=') p++;
        while (*p && *p != '\n' && *p != '\r') val += *p++;
        if (!key.empty()) {
            if (key == "multiThreaded") multiThreaded = (val == "true" || val == "1");
            else if (key == "renderThreads") renderThreads = atoi(val.c_str());
            else if (key == "vsync") vsync = (val == "true" || val == "1");
            else if (key == "darkMode") darkMode = (val == "true" || val == "1");
            else if (key == "defaultFontSize") defaultFontSize = (float)atof(val.c_str());
            else if (key == "scrollStep") scrollStep = (float)atof(val.c_str());
            else if (key == "themeTransitionDuration") themeTransitionDuration = (float)atof(val.c_str());
            else if (key == "animationsEnabled") animationsEnabled = (val == "true" || val == "1");
            else if (key == "showFPS") showFPS = (val == "true" || val == "1");
            else if (key == "targetFPS") targetFPS = atoi(val.c_str());
        }
    }
    printf("[Config] loaded from '%s'\n", path);
    return true;
}

bool Config::saveToFile(const char* path) const {
    FILE* f = fopen(path, "wb");
    if (!f) return false;
    fprintf(f, "# ModernUI Configuration\n");
    fprintf(f, "multiThreaded=%s\n", multiThreaded ? "true" : "false");
    fprintf(f, "renderThreads=%d\n", renderThreads);
    fprintf(f, "vsync=%s\n", vsync ? "true" : "false");
    fprintf(f, "darkMode=%s\n", darkMode ? "true" : "false");
    fprintf(f, "defaultFontSize=%.0f\n", defaultFontSize);
    fprintf(f, "scrollStep=%.0f\n", scrollStep);
    fprintf(f, "themeTransitionDuration=%.2f\n", themeTransitionDuration);
    fprintf(f, "animationsEnabled=%s\n", animationsEnabled ? "true" : "false");
    fprintf(f, "showFPS=%s\n", showFPS ? "true" : "false");
    fprintf(f, "targetFPS=%d\n", targetFPS);
    fclose(f);
    printf("[Config] saved to '%s'\n", path);
    return true;
}

} // namespace ui
