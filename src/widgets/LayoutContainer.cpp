#include "LayoutContainer.hpp"

namespace ui {

std::unique_ptr<Container> LayoutContainer::create(ContainerType type) {
    auto c = std::make_unique<Container>(LayoutDirection::Vertical);
    switch (type) {
    case ContainerType::Header:
        c->setMinSize(0, 48); c->setPadding({12,16,12,16});
        c->setBackgroundColor(Color::fromHex(0xFF1C1C28));
        break;
    case ContainerType::Main:
        c->setPadding({16,16,16,16}); c->setMinSize(0, 100);
        break;
    case ContainerType::Aside:
        c->setMinSize(200, 0); c->setPadding({12,12,12,12});
        c->setBackgroundColor(Color::fromHex(0xFF1A1A26));
        break;
    case ContainerType::Footer:
        c->setMinSize(0, 40); c->setPadding({8,16,8,16});
        c->setBackgroundColor(Color::fromHex(0xFF1C1C28));
        break;
    case ContainerType::Section:
        c->setPadding({16,16,16,16}); c->setSpacing(8);
        break;
    }
    return c;
}

std::unique_ptr<Container> LayoutContainer::appLayout(
    std::unique_ptr<Widget> headerContent,
    std::unique_ptr<Widget> asideContent,
    std::unique_ptr<Widget> mainContent,
    std::unique_ptr<Widget> footerContent)
{
    // Outer: Vertical
    auto root = std::make_unique<Container>(LayoutDirection::Vertical);

    // Header
    auto hdr = create(ContainerType::Header);
    if (headerContent) hdr->addChild(std::move(headerContent));
    root->addChild(std::move(hdr));

    // Body: Horizontal (aside + main)
    auto body = std::make_unique<Container>(LayoutDirection::Horizontal);

    // Aside
    if (asideContent) {
        auto as = create(ContainerType::Aside);
        as->addChild(std::move(asideContent));
        body->addChild(std::move(as));
    }

    // Main
    auto mn = create(ContainerType::Main);
    if (mainContent) mn->addChild(std::move(mainContent));
    body->addChild(std::move(mn));
    body->setChildSizing(body->childCount()-1, Sizing::fill(1), Sizing::fill(1));

    root->addChild(std::move(body));

    // Footer
    auto ft = create(ContainerType::Footer);
    if (footerContent) ft->addChild(std::move(footerContent));
    root->addChild(std::move(ft));

    return root;
}

} // namespace ui
