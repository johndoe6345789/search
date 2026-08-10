import { renderHook, waitFor } from
  '@testing-library/react';
import { useSearchIndexes } from './useSearchIndexes';

type FM = jest.Mock<Promise<Response>>;
const ok = (b: unknown): Response =>
  ({ ok: true, status: 200, json: async () => b })
    as Response;

describe('useSearchIndexes', () => {
  let fm: FM;
  beforeEach(() => {
    fm = jest.fn();
    global.fetch = fm as unknown as typeof fetch;
  });

  it('loads index rows from /api/search/indexes',
    async () => {
    fm.mockResolvedValue(
      ok({ items: [{ id: 1, name: 'posts' }] }),
    );
    const { result } =
      renderHook(() => useSearchIndexes());
    await waitFor(() =>
      expect(result.current.rows.length).toBe(1),
    );
    expect(fm.mock.calls[0][0]).toContain(
      '/api/search/indexes',
    );
  });

  it('records HTTP error', async () => {
    fm.mockResolvedValue(
      { ok: false, status: 502 } as Response,
    );
    const { result } =
      renderHook(() => useSearchIndexes());
    await waitFor(() =>
      expect(result.current.error).toMatch(/502/),
    );
  });
});
