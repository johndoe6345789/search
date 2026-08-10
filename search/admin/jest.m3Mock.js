const React = require('react');
const h = React.createElement;

const btn = (p) =>
  h('button', {
    onClick: p.onClick,
    disabled: p.disabled,
    'aria-label': p['aria-label'],
    'data-testid': p['data-testid'],
  }, p.children ?? p.label);

const tf = (p) =>
  h('label', null, p.label,
    h('input', {
      value: p.value ?? '',
      'aria-label': p.label,
      onChange: p.onChange,
    }));

const ta = (p) =>
  h('label', null, p.label,
    h('textarea', {
      value: p.value ?? '',
      'aria-label': p.label,
      onChange: p.onChange,
    }));

const chk = (p) =>
  h('input', {
    type: 'checkbox',
    checked: !!p.checked,
    disabled: p.disabled,
    'aria-label': p['aria-label'],
    onChange: p.onChange,
  });

const rng = (p) =>
  h('input', {
    type: 'range',
    value: p.value,
    disabled: p.disabled,
    'aria-label': p['aria-label'],
    onChange: (e) =>
      p.onChange?.(e, Number(e.target.value)),
  });

const span = (p) =>
  h('span', {
    'aria-label': p['aria-label'],
    'data-testid': p['data-testid'],
  }, p.label ?? p.children);

const SPECIAL = {
  Button: btn, M3Button: btn, IconButton: btn,
  ButtonGroup: (p) => h('div', null, p.children),
  Chip: (p) => h('span', null, p.label),
  TextField: tf, M3TextField: tf,
  TextArea: ta, M3TextArea: ta,
  Switch: chk, Checkbox: chk,
  Slider: rng, Typography: span,
};

const div = (p) =>
  h('div', {
    role: p.role,
    'aria-label': p['aria-label'],
    'data-testid': p['data-testid'],
    onClick: p.onClick,
  }, p.children);

module.exports = new Proxy({}, {
  get: (_t, key) => {
    if (key === '__esModule') return true;
    if (SPECIAL[key]) return SPECIAL[key];
    return div;
  },
});
