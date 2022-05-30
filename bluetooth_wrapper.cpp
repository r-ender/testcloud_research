#include <bluetooth_wrapper.hpp>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth.h>
#include <rfcomm.h>

int Bluetooth_Wrapper::bt_connect(int fd, const sockaddr *addr, socklen_t len)
{
    //return connect(fd, (struct sockaddr *)&addr, len);
    struct sockaddr_rc addr2;

        addr2.rc_family = AF_BLUETOOTH;
        addr2.rc_channel = (uint8_t) 1;
        str2ba("CC:3D:82:39:9B:61", &addr2.rc_bdaddr );	//für Cloud-Lab

        return (connect(fd, (struct sockaddr *)&addr2, sizeof(addr2)));	//connect to server
}

int Bluetooth_Wrapper::bt_connect(int fd)
{
    struct sockaddr_rc addr2;

    addr2.rc_family = AF_BLUETOOTH;
    addr2.rc_channel = (uint8_t) 1;
    str2ba("CC:3D:82:39:9B:61", &addr2.rc_bdaddr );	//für Cloud-Lab

    return (connect(fd, (struct sockaddr *)&addr2, sizeof(addr2)));	//connect to server
}

int Bluetooth_Wrapper::bt_connect(int fd, std::string *bt_addr)
{
    struct sockaddr_rc addr;

    addr.rc_family = AF_BLUETOOTH;
    addr.rc_channel = (uint8_t) 1;

    //convert std::string to char-array
    char btaddr[bt_addr->length()+1];
    strcpy(btaddr,bt_addr->c_str());

    /*
    for(int x = 0; x < sizeof(btaddr); x++)
    {
        btaddr[x] = bt_addr[x];
    } */

    std::cout << "converted bt-Adr.: " << btaddr << std::endl;

    str2ba(btaddr, &addr.rc_bdaddr );	//für Cloud-Lab

    return (connect(fd, (struct sockaddr *)&addr, sizeof(addr)));	//connect to server
}

int Bluetooth_Wrapper::bt_write(int fd, const void *buf, size_t n)
{
    return (write(fd,buf, n));
}
