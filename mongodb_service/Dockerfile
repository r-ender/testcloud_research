FROM ubuntu:18.04
FROM mongo:4.2.2

COPY bson_mongo_libs/. /usr/local/include/
COPY bson_mongo_libs/. /usr/local/lib/

RUN pwd && ls -la /usr/local/include
RUN ls -la /usr/local/lib

RUN mkdir -p /usr/src/testmongosrvc
WORKDIR /usr/src/testmongosrvc
COPY CRUDinsert_service.c /usr/src/testmongosrvc/
COPY Dockerfile /usr/src/testmongosrvc/
RUN pwd && ls -la

RUN apt-get update && apt-get -y install gcc libssl-dev libsasl2-dev
RUN ldconfig --verbose

RUN gcc -g3 -Wall -o CRUDinsert_service CRUDinsert_service.c -I/usr/local/include/libbson-1.0 -I/usr/local/include/libmongoc-1.0 -lmongoc-1.0 -lbson-1.0

EXPOSE 27017/tcp

CMD ./CRUDinsert_service
