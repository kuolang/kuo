FROM gcc:13 AS builder

WORKDIR /kuo

RUN apt-get update && apt-get install -y \
    cmake \
    make \
    && rm -rf /var/lib/apt/lists/*

COPY include/ ./include/
COPY src/      ./src/
COPY Makefile  ./

RUN make

FROM gcc:13-slim AS runtime

LABEL maintainer="kuo-lang"
LABEL description="Kuo programming language compiler"
LABEL version="0.0.1"

RUN apt-get update && apt-get install -y \
    g++ \
    && rm -rf /var/lib/apt/lists/*

COPY --from=builder /kuo/kuo /usr/local/bin/kuo

COPY examples/ /kuo/examples/

WORKDIR /workspace

RUN kuo --help

CMD ["/bin/bash"]