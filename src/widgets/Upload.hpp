#pragma once
#include <string>
#include <functional>
#include "Widget.hpp"

namespace ui {

class Upload : public Widget {
public:
    Upload();

    void setText(const std::string& t) { m_text = t; }
    void setAccept(const std::string& ext) { m_accept = ext; }
    void setOnFileSelected(std::function<void(const std::string&)> cb) { m_onFile = std::move(cb); }
    void setFont(class Font* f) { m_font = f; }
    void setSelectedFile(const std::string& path) { m_selectedFile = path; }
    const std::string& selectedFile() const { return m_selectedFile; }
    // Callback to open file picker (wired by app)
    void setOpenFilePicker(std::function<void(Upload*)> cb) { m_openPicker = std::move(cb); }

    const char* typeName() const override { return "Upload"; }
    Size measure(const Size&) const override { return {280, 80}; }
    void paint(Painter& p) override;
    bool onMouseDown(MouseEvent& e) override;
    bool onMouseEnter(MouseEvent&) override { m_hovered = true; return true; }
    bool onMouseLeave(MouseEvent&) override { m_hovered = false; return true; }

private:
    std::string m_text = "Click to select a file";
    std::string m_accept;
    std::string m_selectedFile;
    std::function<void(const std::string&)> m_onFile;
    std::function<void(Upload*)> m_openPicker;
    class Font* m_font = nullptr;
    bool m_hovered = false;
};

} // namespace ui

