import type { ReactNode } from 'react'
import type { Metadata } from 'next'

/**
 * Root layout for the search operator tool.
 * Uses the shared M3 dark theme via data-theme.
 */
export const metadata: Metadata = {
  title: 'Search — Nextra',
  description:
    'Elasticsearch index health + reindex',
}

export default function RootLayout({
  children,
}: {
  children: ReactNode
}) {
  return (
    <html lang="en" data-theme="dark">
      <body>{children}</body>
    </html>
  )
}
