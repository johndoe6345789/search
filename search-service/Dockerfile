# services/search-service/Dockerfile - nextra-search
# Generated from docker/service-base.Dockerfile;
# per-service identity defaulted below so this
# Dockerfile builds standalone. Edit the canonical
# docker/service-base.Dockerfile, then regenerate.
# Multi-stage build for all nextra Drogon microservices.
#
# Stages toolchain and conan-deps have no per-service
# ARGs, so Docker's layer cache shares them across all
# 13 service builds — apt and conan run only once.
#
# Build args (only needed in final two stages):
#   SVC_NAME   binary name      (e.g. nextra-auth)
#   SVC_DIR    CMakeLists dir   (e.g. auth-service)
#   SVC_PORT   exposed port     (e.g. 9001)

ARG BASE_IMAGE=debian:sid
ARG RUNTIME_IMAGE=debian:sid-slim
ARG APT_PROXY=

# Per-service identity (override via --build-arg)
ARG SVC_NAME=nextra-search
ARG SVC_DIR=search-service
ARG SVC_PORT=9009

# ── Stage 1: system toolchain ───────────────────────
# Cached forever; rebuilds only when apt packages change.
FROM ${BASE_IMAGE} AS toolchain
ARG APT_PROXY

RUN if [ -n "${APT_PROXY}" ]; then \
        printf 'Acquire::http::Proxy "%s";\n' "${APT_PROXY}" \
        > /etc/apt/apt.conf.d/00proxy; \
    fi && \
    apt-get update && \
    apt-get install -y --no-install-recommends \
        bison ca-certificates cmake flex \
        g++-13 gcc-13 git libpq-dev make \
        ninja-build pkg-config \
        python3 python3-pip && \
    rm -rf /var/lib/apt/lists/* && \
    ln -sf /usr/bin/gcc-13 /usr/local/bin/cc && \
    ln -sf /usr/bin/g++-13 /usr/local/bin/g++ && \
    python3 -m pip install --break-system-packages \
        --no-cache-dir "conan>=2.0,<3.0" && \
    conan profile detect --force

ENV CC=gcc-13 CXX=g++-13 CONAN_SKIP_TEST=1

# ── Stage 2: Conan C++ dependencies ─────────────────
# Cached until conanfile.py changes (rare).
FROM toolchain AS conan-deps

WORKDIR /conan
COPY services/drogon-host/conanfile.py .

RUN conan install /conan --build=missing \
        --output-folder=/conan/out \
        -s build_type=Release \
        -s compiler.cppstd=20

# ── Stage 3: per-service compile ────────────────────
# Rebuilds when sources change. cmake only — no apt/conan.
FROM conan-deps AS build
ARG SVC_NAME
ARG SVC_DIR
# Pass --build-arg SRC_BUST=$(date +%s) to bypass the COPY
# cache when Mac Docker Desktop serves stale layers.
ARG SRC_BUST=1
RUN echo "src bust: ${SRC_BUST}"

WORKDIR /src
COPY . .

RUN cmake -B /build \
        -S "/src/services/${SVC_DIR}" \
        -G Ninja \
        -DCMAKE_BUILD_TYPE=Release \
        "-DCMAKE_TOOLCHAIN_FILE=/conan/out/build/Release/generators/conan_toolchain.cmake" && \
    cmake --build /build --clean-first \
        -j"$(( $(nproc) > 4 ? 4 : $(nproc) ))"

# ── Stage 4: slim runtime base ───────────────────────
# Cached forever; shared by all service runtime images.
FROM ${RUNTIME_IMAGE} AS runtime-base
ARG APT_PROXY

RUN if [ -n "${APT_PROXY}" ]; then \
        printf 'Acquire::http::Proxy "%s";\n' "${APT_PROXY}" \
        > /etc/apt/apt.conf.d/00proxy; \
    fi && \
    apt-get update && \
    apt-get install -y --no-install-recommends \
        ca-certificates libpq5 libssl3 && \
    rm -rf /var/lib/apt/lists/* && \
    groupadd --system nextra && \
    useradd --system --gid nextra \
        --create-home nextra

# ── Stage 5: per-service runtime image ──────────────
FROM runtime-base AS runtime
ARG SVC_NAME
ARG SVC_DIR
ARG SVC_PORT

WORKDIR /app

COPY --from=build "/build/${SVC_NAME}" ./bin/svc
COPY "services/${SVC_DIR}/config/config.json" \
    ./config/config.json

RUN mkdir -p logs && \
    chown -R nextra:nextra /app

USER nextra
EXPOSE ${SVC_PORT}

ENTRYPOINT ["./bin/svc", "config/config.json"]
