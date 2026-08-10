const run = jest.fn();

jest.mock('@/hooks/useSearchPreview', () => ({
  useSearchPreview: () => ({
    hits: [{
      _index: 'nx_posts',
      _id: '42',
      _score: 1,
      _source: {},
    }],
    total: 1,
    error: null,
    loading: false,
    run,
  }),
}));

import { render, screen } from
  '@testing-library/react';
import userEvent from '@testing-library/user-event';
import { QueryTester } from './QueryTester';

describe('QueryTester', () => {
  beforeEach(() => run.mockClear());

  it('runs query on button click', async () => {
    render(<QueryTester />);
    await userEvent.type(
      screen.getByLabelText('Search term'),
      'hello',
    );
    await userEvent.click(
      screen.getByRole('button', { name: 'Search' }),
    );
    expect(run).toHaveBeenCalledWith('hello');
  });

  it('renders hits and total', () => {
    render(<QueryTester />);
    expect(screen.getByText('1 results')).toBeTruthy();
    expect(screen.getByText(/nx_posts/)).toBeTruthy();
  });
});
