#!/bin/bash
#script for setting up the testcloud 
#prerequisites: docker has been installed previously, the network has been setup to port-forwarding etc.

#anmeldung in Dokcerhub nötig?
docker pull 0891860/testcloud_image:frontend_tls_srv
docker pull 0891860/testcloud_image:mongodb_service
docker pull 0891860/testcloud_image:endecrypt_service
docker pull 0891860/testcloud_image:mongodaemon_service

docker volume create vol_tmp
docker volume create vol_data

docker container run -d --name=tls_server_ctr -v vol_tmp:/tmp/ -p8082:8082 --network="host" $(docker images --filter=reference='0891860/testcloud_image:frontend_tls_srv' -q)
docker container run -d --name=endecrypt_ctr -v vol_tmp:/tmp/ -v vol_data:/data/db/ --network="host" $(docker images --filter=reference='0891860/testcloud_image:endecrypt_service' -q)
docker container run -d --name=mongodaemon_ctr -v vol_tmp:/tmp/ -v vol_data:/data/db/ -p27017:27017 --network="host" $(docker images --filter=reference='0891860/testcloud_image:mongodaemon_service' -q)
docker container run -d --name=mongodbserv_ctr -v vol_tmp:/tmp/ -v vol_data:/data/db/ --network="host" $(docker images --filter=reference='0891860/testcloud_image:mongodb_service' -q)
