FROM ubuntu:18.04

RUN mkdir -p /usr/src/endecrypt_srvc
WORKDIR /usr/src/endecrypt_srvc
COPY . /usr/src/endecrypt_srvc/
RUN apt-get update && apt-get -y install gcc && apt-get -y install libssl-dev
RUN gcc -g3 -Wall -o endecrypt endecrypt.c -lcrypto

CMD ./endecrypt
