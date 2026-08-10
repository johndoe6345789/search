import '@testing-library/jest-dom';

// Default fetch mock — individual tests override.
global.fetch = jest.fn(() =>
  Promise.resolve({
    ok: true,
    status: 200,
    json: () => Promise.resolve({}),
  }),
) as unknown as typeof fetch;
