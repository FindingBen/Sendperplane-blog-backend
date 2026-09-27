FROM debian:bookworm-slim AS build

RUN apt-get update && apt-get install -y --no-install-recommends \
    postgresql-client \
    libpq-dev \
    libmicrohttpd-dev \
    libssl-dev \
    cmake \
    ninja-build \
    build-essential \
    gcc \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src

COPY . .

RUN cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release . \
    && cmake --build build

FROM debian:bookworm-slim

RUN apt-get update && apt-get install -y --no-install-recommends \
    libpq5 \
    libmicrohttpd12 \
    libssl3 \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY --from=build /src/build/backend-blog /app/app

EXPOSE 8080

CMD ["/app/app"]
