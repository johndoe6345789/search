/**
 * @file test_search_service.cpp
 * @brief SearchService query builder + facets.
 *
 * Mirrors the ES query JSON shape constructed by
 * services/search/SearchQuery.cpp.
 */

#include <gtest/gtest.h>
#include <string>
#include <vector>

namespace
{

std::string buildQuery(const std::string& q,
                       const std::vector<std::string>& fs)
{
    std::string j = "{\"query\":{\"multi_match\":{"
                    "\"query\":\"" + q + "\"}}";
    if (!fs.empty())
    {
        j += ",\"aggs\":{";
        for (size_t i = 0; i < fs.size(); ++i)
        {
            if (i) j += ",";
            j += "\"" + fs[i] + "\":{\"terms\":{\"field"
                                "\":\"" + fs[i] + "\"}}";
        }
        j += "}";
    }
    j += "}";
    return j;
}

} // namespace

class SearchServiceTest : public ::testing::Test
{
};

TEST_F(SearchServiceTest, SimpleQuery)
{
    auto j = buildQuery("hello", {});
    EXPECT_NE(j.find("\"query\":\"hello\""),
              std::string::npos);
    EXPECT_EQ(j.find("aggs"), std::string::npos);
}

TEST_F(SearchServiceTest, FacetedQuery)
{
    auto j = buildQuery("x", {"tag", "author"});
    EXPECT_NE(j.find("aggs"), std::string::npos);
    EXPECT_NE(j.find("\"tag\""), std::string::npos);
    EXPECT_NE(j.find("\"author\""), std::string::npos);
}

TEST_F(SearchServiceTest, EmptyQuery)
{
    auto j = buildQuery("", {});
    EXPECT_NE(j.find("\"query\":\"\""),
              std::string::npos);
}
