import { render, screen } from
  '@testing-library/react';
import userEvent from '@testing-library/user-event';
import { ReindexButton } from './ReindexButton';

describe('ReindexButton', () => {
  it('fires onClick with name when idle',
    async () => {
    const onClick = jest.fn();
    render(
      <ReindexButton
        name="posts"
        busy={false}
        onClick={onClick}
      />,
    );
    await userEvent.click(
      screen.getByRole('button', {
        name: 'Reindex posts',
      }),
    );
    expect(onClick).toHaveBeenCalledWith('posts');
  });

  it('shows busy label when busy', () => {
    render(
      <ReindexButton
        name="posts"
        busy={true}
        onClick={jest.fn()}
      />,
    );
    expect(
      screen.getByText('Queueing…'),
    ).toBeTruthy();
  });
});
