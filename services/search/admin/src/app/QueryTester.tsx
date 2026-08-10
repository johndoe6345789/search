'use client'

/**
 * Small "live search" playground wired to
 * /api/search — lets operators sanity-check
 * that reindex actually produced hits.
 */

import { useState } from 'react'
import {
  Input, Button, Heading, Text,
} from '@shared/m3'

import {
  useSearchPreview,
} from '@/hooks/useSearchPreview'

export function QueryTester() {
  const [q, setQ] = useState<string>('')
  const { hits, total, error, loading, run } =
    useSearchPreview()

  return (
    <section
      className="search-tester"
      aria-label="Live query tester"
    >
      <Heading level={2}>Query tester</Heading>
      <Input
        value={q}
        onChange={(e) => setQ(
          (e.target as HTMLInputElement).value,
        )}
        placeholder="Type a search term…"
        aria-label="Search term"
        data-testid="query-input"
      />
      <Button
        variant="filled"
        onClick={() => run(q)}
        disabled={loading}
        data-testid="query-run"
      >
        {loading ? 'Searching…' : 'Search'}
      </Button>
      {error && (
        <Text
          sm
          data-testid="query-error"
        >
          {error}
        </Text>
      )}
      <Text sm>
        {total} results
      </Text>
      <ul data-testid="query-hits">
        {hits.map((h) => (
          <li key={`${h._index}:${h._id}`}>
            <code>{h._index}</code> —{' '}
            {h._id}
          </li>
        ))}
      </ul>
    </section>
  )
}
