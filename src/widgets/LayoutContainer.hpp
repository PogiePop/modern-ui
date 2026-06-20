#pragma once
#include "Container.hpp"

namespace ui {

// Predefined layout container types
enum class ContainerType {
    Header,    // top bar, full width, typically 48-56px
    Main,      // main content area, fills remaining space
    Aside,     // sidebar, fixed width
    Footer,    // bottom bar, full width
    Section    // content section with padding
};

// Layout builder: creates common app layout patterns
class LayoutContainer : public Container {
public:
    // Create a specific container type
    static std::unique_ptr<Container> create(ContainerType type);
    static std::unique_ptr<Container> header()  { return create(ContainerType::Header); }
    static std::unique_ptr<Container> main()    { return create(ContainerType::Main); }
    static std::unique_ptr<Container> aside()   { return create(ContainerType::Aside); }
    static std::unique_ptr<Container> footer()  { return create(ContainerType::Footer); }
    static std::unique_ptr<Container> section() { return create(ContainerType::Section); }

    // Pre-built app layout: header + sidebar + main + footer
    static std::unique_ptr<Container> appLayout(
        std::unique_ptr<Widget> headerContent,
        std::unique_ptr<Widget> asideContent,
        std::unique_ptr<Widget> mainContent,
        std::unique_ptr<Widget> footerContent);

    const char* typeName() const override { return "LayoutContainer"; }
};

} // namespace ui
