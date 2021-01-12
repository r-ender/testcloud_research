Mit den Services
- frontend_tls_srv wird der https-Server
- endecrypt_srvc die Verschlüsselungsfunktion
- mongodb_srvc der MongoDB-Client
- mongo_srvc der MongoDB-Server

zur Verfügung gestellt. Diese befinden sich im DockerHub 0891860/testcloud_images

zum Download der Images:
~$: sudo docker pull 0891860/hyworld:testcloud_images
~$: sudo docker images


(optional)
Um mit Docker-engine aus images containers zu machen:
(sudo docker-compose build --no-cache)
sudo docker volume create vol_tmp && sudo docker volume create vol_data
(sudo docker volume create vol_log)
( sudo docker images | grep frontend_tls_srv)


sudo docker container run -d --name=tls_server_ctr --cap-add=SYS_ADMIN --cap-add=NET_ADMIN -v vol_tmp:/tmp/ -p8082:8082 --network="host" <image-id>
sudo docker container run -d --name=endecrypt_ctr -v vol_tmp:/tmp/ -v vol_data:/data/db/ --network="host" <image-id>
sudo docker container run -d --name=mongodaemon_ctr -v vol_tmp:/tmp/ -v vol_data:/data/db/ -p27017:27017 --network="host" <image-id>
sudo docker container run -d --name=mongodbserv_ctr -v vol_tmp:/tmp/ -v vol_data:/data/db/ --network="host" <image-id>

mit "sudo docker ps -a" checken, ob container laufen, falls Fehler:
"sudo docker restart tls_server_ctr"


Die Verzeichnisse befinden sich unter /var/lib/docker

Falls der laufende Container gesteuert werden soll: 
sudo docker exec -ti tls_server_ctr bash


Netzwerk:
Die Container wurden so gebaut, dass NAT vom Host verwendet werden, darum auch die Weiterleitung durch das Argument -p8082:8082.
NAT erkennt man anhand der IP-Adressen der eigenen Rechner aus einem der privaten Adressbereiche: 
10.0.0.0 bis 10.255.255.255 oder CIDR: 10.0.0.0/8 (bloß die ersten 8 Bit sind fest hier: 0xFF000000), 
172.16.0.0 bis 172.31.255.255 oder CIDR: 172.16.0.0/12,
192.168.0.0 bis 192.168.255.255 oder CIDR: 192.168.0.0/16
