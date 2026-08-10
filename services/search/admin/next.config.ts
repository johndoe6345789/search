import type { NextConfig } from 'next';
import path from 'path';

const monorepoRoot = path.resolve(
  __dirname, '../../..',
);

const nextConfig: NextConfig = {
  basePath: process.env.NEXT_BASE_PATH || '/search',
  output: 'standalone',
  reactStrictMode: true,
  sassOptions: {
    silenceDeprecations: ['legacy-js-api'],
    includePaths: [
      path.join(monorepoRoot, 'shared/scss'),
      path.join(
        monorepoRoot, 'shared/scss/m3-scss',
      ),
    ],
    loadPaths: [
      path.join(
        monorepoRoot, 'shared/scss/m3-scss',
      ),
      path.join(monorepoRoot, 'shared/scss'),
    ],
  },
  transpilePackages: [
    '@shared/m3',
    '@shared/components',
    '@shared/hooks',
    '@shared/hooks-canvas',
    '@shared/icons',
    '@shared/redux-core',
    '@shared/redux-slices',
    '@shared/scss',
    '@shared/service-adapters',
  ],
  turbopack: { root: monorepoRoot },
};

export default nextConfig;
