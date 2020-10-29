FROM ubuntu:18.04

RUN mkdir -p /usr/src/cloud_ssl_server
WORKDIR /usr/src/cloud_ssl_server
COPY . /usr/src/cloud_ssl_server/
RUN apt-get update && apt-get -y install gcc && apt-get -y install libssl-dev
RUN gcc -o ssl_server ssl_server.c -L/usr/lib -lssl -lcrypto

EXPOSE 8082/tcp

CMD ./ssl_server 8082
