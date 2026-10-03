/// @file UrlBuilder.cpp — esIndex -> frontend URL.
#include "search/backend/UrlBuilder.h"

namespace services
{

namespace
{

std::string strField(const json& src,
                     const std::string& k)
{
    auto it = src.find(k);
    if (it == src.end() || !it->is_string()) {
        return {};
    }
    return it->get<std::string>();
}

} // namespace

std::string UrlBuilder::build(
    const std::string& esIndex,
    const std::string& id, const json& src)
{
    if (esIndex == "nextra-forum") {
        auto t = strField(src, "target_id");
        return "/forum/threads/" +
               (t.empty() ? id : t);
    }
    if (esIndex == "nextra-wiki") {
        auto s = strField(src, "slug");
        return "/wiki/" + (s.empty() ? id : s);
    }
    if (esIndex == "nextra-blog") {
        auto s = strField(src, "slug");
        return "/blog/" + (s.empty() ? id : s);
    }
    if (esIndex == "nextra-products") {
        auto s = strField(src, "sku");
        return "/shop/" + (s.empty() ? id : s);
    }
    if (esIndex == "nextra-gallery") {
        auto s = strField(src, "slug");
        return "/gallery/" + (s.empty() ? id : s);
    }
    if (esIndex == "nextra-users") {
        auto u = strField(src, "username");
        return "/u/" + (u.empty() ? id : u);
    }
    // LaunchPad index set.
    if (esIndex == "launchpad-kb") {
        auto st = strField(src, "startup_type");
        std::string base = "/knowledge";
        if (!st.empty()) base += "/" + st;
        return base + "/" + id;
    }
    if (esIndex == "launchpad-planner") {
        auto st = strField(src, "startup_type");
        auto step = strField(src, "step_id");
        std::string base = "/planner";
        if (!st.empty()) base += "/" + st;
        if (!step.empty())
            base += "?step=" + step;
        return base;
    }
    if (esIndex == "launchpad-community") {
        auto tid = strField(src, "target_id");
        return "/community/threads/" +
               (tid.empty() ? id : tid);
    }
    if (esIndex == "launchpad-founders") {
        auto u = strField(src, "username");
        return "/u/" + (u.empty() ? id : u);
    }
    return "#";
}

std::string UrlBuilder::typeOf(
    const std::string& esIndex)
{
    if (esIndex == "nextra-forum")    return "forum_posts";
    if (esIndex == "nextra-wiki")     return "wiki_pages";
    if (esIndex == "nextra-blog")     return "articles";
    if (esIndex == "nextra-products") return "products";
    if (esIndex == "nextra-gallery")  return "gallery_items";
    if (esIndex == "nextra-users")    return "users";
    if (esIndex == "launchpad-kb")    return "kb_content";
    if (esIndex == "launchpad-planner")
        return "planner_steps";
    if (esIndex == "launchpad-community")
        return "community_posts";
    if (esIndex == "launchpad-founders")
        return "founders";
    return esIndex;
}

} // namespace services
