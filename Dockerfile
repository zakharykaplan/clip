FROM ubuntu:latest

RUN apt update && apt install -y \
        build-essential \
        libfmt-dev

WORKDIR /root/clip

COPY . .

CMD ["make"]
