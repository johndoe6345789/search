# `search.reindex` Kafka event shape

Producers (forum, wiki, blog, shop, gallery, users) emit
JSON messages to the Kafka topic `search.reindex`. The
search-indexer daemon consumes them and applies upserts /
deletes against the corresponding Elasticsearch index.

## Wire format

```json
{
  "op":    "upsert",
  "index": "forum_posts",
  "id":    "12345",
  "doc": {
    "target_type": "forum_thread",
    "target_id":   "abc-slug",
    "author_id":   "uuid...",
    "body":        "post body markdown...",
    "title":       "thread title (only for forum_board rows)",
    "created_at":  "2026-05-02T10:00:00Z"
  }
}
```

## Fields

- `op` — required. One of:
  - `"upsert"` — index or replace the document at `id`.
  - `"delete"` — remove the document at `id`.
- `index` — logical index name. Must match a
  `name` from `services/search/constants.json`
  (`forum_posts`, `wiki_pages`, `articles`,
  `products`, `gallery_items`, `users`).
- `id` — document id as a **string**. The indexer
  uses this verbatim as the Elasticsearch `_id`.
- `doc` — required for `upsert`, ignored on
  `delete`. The shape mirrors the per-index mapping
  in `SearchIndexMappings.h`. Unknown fields are
  ignored by Elasticsearch dynamic mapping.

## Phase 1 status

The current `KafkaConsumerStub` logs receipts but
does not yet apply them. The full upsert/delete
dispatcher lands in Phase 2 once producers exist.
Boot-time backfill (`reindexAll`) and the periodic
resync timer keep the indexes warm in the meantime.
