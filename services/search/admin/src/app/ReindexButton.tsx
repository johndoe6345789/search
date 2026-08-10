'use client'

/**
 * Single reindex trigger — wired to
 * useReindex.  M3 Button only.
 */

import { Button } from '@shared/m3'

interface Props {
  name: string
  busy: boolean
  onClick: (name: string) => void
}

export function ReindexButton({
  name, busy, onClick,
}: Props) {
  return (
    <Button
      variant="filled"
      size="small"
      disabled={busy}
      data-testid={`reindex-${name}`}
      aria-label={`Reindex ${name}`}
      onClick={() => onClick(name)}
    >
      {busy ? 'Queueing…' : 'Reindex'}
    </Button>
  )
}
