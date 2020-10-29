FROM ubuntu:18.04

RUN mkdir -p /usr/src/cloud_ssl_server/hello_world3_service
WORKDIR /usr/src/cloud_ssl_server/hello_world3_service
COPY . /usr/src/cloud_ssl_server/hello_world3_service/
RUN apt-get update && apt-get -y install gcc
RUN gcc -o helloworld3 helloworld3.c

CMD ./helloworld3
