FROM ubuntu:latest

RUN apt-get update && apt-get install -y \
    g++ \
    cmake \
    libsdl2-dev \
    libsdl2-image-dev \
    libsdl2-mixer-dev \
    libsdl2-ttf-dev

COPY . /app/

WORKDIR /app


RUN cd /app \
    cmake -S . -B build \
    cmake --build build

CMD [ "./build/gbvm" ]
