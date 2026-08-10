-- 001_search_indexes.sql
-- Registry of Elasticsearch indexes maintained by the
-- search-indexer daemon (Phase 4.5).
--
-- Each row describes one logical index: where its source
-- rows come from (target_table), which ES index receives
-- the documents (es_index), and the last reindex / count
-- / status snapshot as reported by the daemon.

CREATE TABLE IF NOT EXISTS search_indexes (
    id              BIGSERIAL PRIMARY KEY,
    name            TEXT NOT NULL UNIQUE,
    target_table    TEXT NOT NULL,
    es_index        TEXT NOT NULL,
    last_reindex_at TIMESTAMPTZ,
    doc_count       BIGINT NOT NULL DEFAULT 0,
    status          TEXT NOT NULL DEFAULT 'idle',
    created_at      TIMESTAMPTZ NOT NULL DEFAULT now(),
    updated_at      TIMESTAMPTZ NOT NULL DEFAULT now()
);

CREATE INDEX IF NOT EXISTS idx_search_indexes_status
    ON search_indexes (status);

-- Drop any rows from the old 4-index registry that are not
-- in the new authoritative 6-index list. Safe on first run.
DELETE FROM search_indexes
 WHERE name IN ('comments', 'blog_articles', 'packages');

-- Authoritative list — keep in sync with
-- services/search/constants.json.
INSERT INTO search_indexes (name, target_table, es_index)
VALUES
    ('forum_posts',   'comments_v2', 'nextra-forum'),
    ('wiki_pages',    'wiki_pages',  'nextra-wiki'),
    ('articles',      'articles',    'nextra-blog'),
    ('products',      'products',    'nextra-products'),
    ('gallery_items', 'galleries',   'nextra-gallery'),
    ('users',         'users',       'nextra-users')
ON CONFLICT (name) DO UPDATE
   SET target_table = EXCLUDED.target_table,
       es_index     = EXCLUDED.es_index,
       updated_at   = now();
