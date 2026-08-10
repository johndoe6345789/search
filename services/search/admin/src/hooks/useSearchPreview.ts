'use client'

/**
 * Live query tester for the public /api/search
 * endpoint.  Drives the QueryTester component.
 */

import { useCallback, useState } from 'react'

const API = '/api/search'

export interface PreviewHit {
  _index: string
  _id: string
  _score: number
  _source: Record<string, unknown>
}

export function useSearchPreview() {
  const [hits, setHits] = useState<PreviewHit[]>([])
  const [total, setTotal] = useState<number>(0)
  const [error, setError] =
    useState<string | null>(null)
  const [loading, setLoading] =
    useState<boolean>(false)

  const run = useCallback(async (q: string) => {
    if (!q.trim()) {
      setHits([])
      setTotal(0)
      return
    }
    setLoading(true)
    setError(null)
    try {
      const r = await fetch(
        `${API}?q=${encodeURIComponent(q)}`,
      )
      if (!r.ok) {
        setError(`HTTP ${r.status}`)
        return
      }
      const j = (await r.json()) as {
        hits?: PreviewHit[]
        total?: number
      }
      setHits(j.hits ?? [])
      setTotal(j.total ?? 0)
    } catch (e) {
      setError((e as Error).message)
    } finally {
      setLoading(false)
    }
  }, [])

  return { hits, total, error, loading, run }
}
