'use client'

/**
 * Search operator root page — lists every
 * registered Elasticsearch index, lets an
 * operator trigger a reindex, and embeds the
 * QueryTester playground.  All interactive
 * controls are M3 components.
 */

import { Heading, Text } from '@shared/m3'

import {
  useSearchIndexes,
} from '@/hooks/useSearchIndexes'
import { useReindex } from '@/hooks/useReindex'
import { IndexCard } from './IndexCard'
import { QueryTester } from './QueryTester'

export default function SearchPage() {
  const { rows, error, refresh } =
    useSearchIndexes()
  const { trigger, busy } = useReindex(refresh)

  return (
    <main className="search-shell">
      <Heading level={1}>Search Indexer</Heading>
      {error && (
        <Text
          sm
          data-testid="indexes-error"
        >
          Failed to load indexes: {error}
        </Text>
      )}
      <section
        aria-label="Registered indexes"
        data-testid="index-list"
      >
        {rows.length === 0 && (
          <Text >
            No registered indexes.
          </Text>
        )}
        {rows.map((r) => (
          <IndexCard
            key={r.id}
            row={r}
            busy={busy}
            onReindex={trigger}
          />
        ))}
      </section>
      <QueryTester />
    </main>
  )
}
