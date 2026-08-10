import { render, screen } from
  '@testing-library/react';
import userEvent from '@testing-library/user-event';
import { IndexCard } from './IndexCard';

const row = {
  id: 1,
  name: 'posts',
  target_table: 'posts',
  es_index: 'nx_posts',
  last_reindex_at: '2026-01-02',
  doc_count: 1234,
  status: 'ready',
};

describe('IndexCard', () => {
  it('renders key fields and fires onReindex',
    async () => {
    const onReindex = jest.fn();
    render(
      <IndexCard
        row={row}
        busy={null}
        onReindex={onReindex}
      />,
    );
    expect(screen.getByText('posts')).toBeTruthy();
    expect(screen.getByText(/nx_posts/)).toBeTruthy();
    await userEvent.click(
      screen.getByRole('button', {
        name: 'Reindex posts',
      }),
    );
    expect(onReindex).toHaveBeenCalledWith('posts');
  });
});
