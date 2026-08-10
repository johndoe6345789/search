/// @file SearchServiceDelete.cpp — removeDoc implementation.
#include "search/backend/SearchService.h"

#include <spdlog/spdlog.h>

namespace services
{

void SearchService::removeDoc(
    const std::string& index, const std::string& docId,
    Callback onOk, ErrCallback onErr)
{
    if (index.empty() || docId.empty()) {
        onErr(drogon::k400BadRequest,
              "index and docId are required");
        return;
    }

    spdlog::debug("removeDoc {}/{}", index, docId);

    es_.deleteDoc(
        index, docId,
        [onOk](json res) { onOk(std::move(res)); },
        [onErr, index, docId](int code, std::string msg) {
            spdlog::warn("removeDoc {}/{} err {}: {}",
                         index, docId, code, msg);
            onErr(drogon::k502BadGateway, std::move(msg));
        });
}

} // namespace services
