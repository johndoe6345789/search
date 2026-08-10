/// @file SearchServiceIndex.cpp — indexUser / indexChatMessage.
#include "search/backend/SearchService.h"

#include <spdlog/spdlog.h>

namespace services
{

static const std::string kUsersIndex = "nextra-users";
static const std::string kChatIndex = "nextra-chat";

void SearchService::indexUser(
    const std::string& userId, const json& userData,
    Callback onOk, ErrCallback onErr)
{
    json doc = {
        {"username",     userData.value("username", "")},
        {"display_name", userData.value("display_name", "")},
        {"email",        userData.value("email", "")},
        {"bio",          userData.value("bio", "")},
        {"role",         userData.value("role", "user")}
    };

    spdlog::debug("indexUser id={}", userId);

    es_.indexDoc(
        kUsersIndex, userId, doc,
        [onOk](json res) { onOk(std::move(res)); },
        [onErr](int code, std::string msg) {
            spdlog::warn("indexUser err {}: {}", code, msg);
            onErr(drogon::k502BadGateway, std::move(msg));
        });
}

void SearchService::indexChatMessage(
    const std::string& messageId, const json& msgData,
    Callback onOk, ErrCallback onErr)
{
    json doc = {
        {"content",   msgData.value("content", "")},
        {"sender",    msgData.value("sender", "")},
        {"channel",   msgData.value("channel", "")},
        {"timestamp", msgData.value("timestamp", "")}
    };

    spdlog::debug("indexChatMessage id={}", messageId);

    es_.indexDoc(
        kChatIndex, messageId, doc,
        [onOk](json res) { onOk(std::move(res)); },
        [onErr](int code, std::string msg) {
            spdlog::warn("indexChat err {}: {}", code, msg);
            onErr(drogon::k502BadGateway, std::move(msg));
        });
}

} // namespace services
