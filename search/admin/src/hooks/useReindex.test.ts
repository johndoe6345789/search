import { renderHook, act } from
  '@testing-library/react';
import { useReindex } from './useReindex';

type FM = jest.Mock<Promise<Response>>;
const ok = (): Response =>
  ({ ok: true, status: 200 }) as Response;

describe('useReindex', () => {
  let fm: FM;
  let done: jest.Mock;
  beforeEach(() => {
    fm = jest.fn().mockResolvedValue(ok());
    global.fetch = fm as unknown as typeof fetch;
    done = jest.fn();
  });

  it('POSTs /reindex/:name and calls onDone',
    async () => {
    const { result } =
      renderHook(() => useReindex(done));
    await act(async () => {
      await result.current.trigger('posts');
    });
    const [url, init] = fm.mock.calls[0];
    expect(String(url)).toContain(
      '/api/search/reindex/posts',
    );
    expect((init as RequestInit).method).toBe('POST');
    expect(done).toHaveBeenCalled();
  });

  it('records HTTP error and skips onDone',
    async () => {
    fm.mockResolvedValue(
      { ok: false, status: 500 } as Response,
    );
    const { result } =
      renderHook(() => useReindex(done));
    await act(async () => {
      await result.current.trigger('x');
    });
    expect(result.current.error).toMatch(/500/);
    expect(done).not.toHaveBeenCalled();
  });
});
