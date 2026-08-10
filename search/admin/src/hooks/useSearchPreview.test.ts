import { renderHook, act, waitFor } from
  '@testing-library/react';
import { useSearchPreview } from './useSearchPreview';

type FM = jest.Mock<Promise<Response>>;
const ok = (b: unknown): Response =>
  ({ ok: true, status: 200, json: async () => b })
    as Response;

describe('useSearchPreview', () => {
  let fm: FM;
  beforeEach(() => {
    fm = jest.fn();
    global.fetch = fm as unknown as typeof fetch;
  });

  it('URL-encodes query param and loads hits',
    async () => {
    fm.mockResolvedValue(
      ok({ hits: [{ _id: '1' }], total: 1 }),
    );
    const { result } =
      renderHook(() => useSearchPreview());
    await act(async () => {
      await result.current.run('a b');
    });
    expect(String(fm.mock.calls[0][0])).toContain(
      'q=a%20b',
    );
    await waitFor(() =>
      expect(result.current.total).toBe(1),
    );
  });

  it('skips fetch on empty query', async () => {
    const { result } =
      renderHook(() => useSearchPreview());
    await act(async () => {
      await result.current.run('   ');
    });
    expect(fm).not.toHaveBeenCalled();
  });
});
