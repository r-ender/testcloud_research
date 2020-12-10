FROM ubuntu:18.04
FROM mongo:4.2.2

RUN mkdir -p /usr/src/mongodaemon_srvc
WORKDIR /usr/src/mongodaemon_srvc
COPY . /usr/src/mongodaemon_srvc/
CMD PWD && ls -la
RUN apt-get update && apt-get -y install gcc 
RUN gcc -g3 -Wall -o mongodaemon mongodaemon.c

CMD ./mongodaemon
