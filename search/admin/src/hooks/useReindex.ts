'use client'

/**
 * Issue POST /api/search/reindex/:name and
 * surface any error to the caller.
 */

import { useCallback, useState } from 'react'

const API = '/api/search'

export function useReindex(
  onDone: () => void,
) {
  const [busy, setBusy] = useState<string | null>(
    null,
  )
  const [error, setError] =
    useState<string | null>(null)

  const trigger = useCallback(
    async (name: string): Promise<boolean> => {
      setBusy(name)
      setError(null)
      try {
        const r = await fetch(
          `${API}/reindex/${name}`,
          { method: 'POST' },
        )
        if (!r.ok) {
          setError(`HTTP ${r.status}`)
          return false
        }
        onDone()
        return true
      } catch (e) {
        setError((e as Error).message)
        return false
      } finally {
        setBusy(null)
      }
    },
    [onDone],
  )

  return { trigger, busy, error }
}
