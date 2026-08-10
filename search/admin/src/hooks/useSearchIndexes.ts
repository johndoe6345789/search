'use client'

/**
 * Poll the search-indexer admin API for the
 * registered-index list and expose a refresh
 * callback so sibling hooks can retrigger it
 * after a mutation.
 */

import {
  useCallback, useEffect, useState,
} from 'react'

const API = '/api/search'
const POLL_MS = 5_000

export interface IndexRow {
  id: number
  name: string
  target_table: string
  es_index: string
  last_reindex_at: string
  doc_count: number
  status: string
}

export function useSearchIndexes() {
  const [rows, setRows] = useState<IndexRow[]>([])
  const [error, setError] =
    useState<string | null>(null)

  const refresh = useCallback(async () => {
    try {
      const r = await fetch(`${API}/indexes`)
      if (!r.ok) {
        setError(`HTTP ${r.status}`)
        return
      }
      const j = (await r.json()) as {
        items: IndexRow[]
      }
      setRows(j.items)
      setError(null)
    } catch (e) {
      setError((e as Error).message)
    }
  }, [])

  useEffect(() => {
    refresh()
    const id = setInterval(refresh, POLL_MS)
    return () => clearInterval(id)
  }, [refresh])

  return { rows, error, refresh }
}
