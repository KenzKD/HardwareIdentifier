#include<netinet/in.h>
#include<errno.h>
#include<netdb.h>
#include<stdio.h>   
#include<stdlib.h>  
#include<string.h>  
#include<netinet/ip_icmp.h>  //For ICMP header
#include<netinet/udp.h>	     //For UDP header
#include<netinet/tcp.h>	     //For TCP header
#include<netinet/ip.h>	     //For IP header
#include<netinet/if_ether.h> //For ETH_P_ALL
#include<net/ethernet.h>     //For Ether header
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/ioctl.h>
#include<sys/time.h>
#include<sys/types.h>
#include<unistd.h>

void ProcessPacket(unsigned char* , int);//To obtain the IP header to detect what protocol is being used
void print_device_info(unsigned char* , int); //To print MAC Address and device type
struct sockaddr_in source,dest;

void ProcessPacket(unsigned char* buffer, int size)	//To obtain the IP header to detect what protocol is being used
{
	struct iphdr *iph = (struct iphdr*)(buffer + sizeof(struct ethhdr));
	switch (iph->protocol) //Check the Protocol and do accordingly...
	{
		case 1:  //ICMP Protocol
		case 2:  //IGMP Protocol
		case 6:  //TCP Protocol
		case 17: //UDP Protocol
			print_device_info(buffer , size);
			break;
		default: //Some Other Protocol like ARP etc.
			break;
	}
}
int getType(int a, int b, int c)	//To get the type of device for the corresponding MAC Address
{
    char mac[7];
    snprintf(mac, sizeof(mac), "%.2X%.2X%.2X", a, b, c);
    // return 1 for mobile, 0 for desktop
    if(strncmp(mac, "F81654", 3)==0)
        return 0;
    else if(strncmp(mac, "00065B", 3)==0) //Dell Laptop
        return 0;
    else if(strncmp(mac, "000AD9", 3)==0) //Sony Mobile
        return 1;
    else if(strncmp(mac, "00061B", 3)==0) //Lenovo Notebook
        return 0;
    else if(strncmp(mac, "0012FE", 3)==0) //Lenovo Mobile
        return 1;
    else if(strncmp(mac, "000124", 3)==0) //Acer Incorporated
        return 0;
    else if(strncmp(mac, "001321", 3)==0) //HP Laptops
        return 0;
    else if(strncmp(mac, "001320", 3)==0) //Intel Corporate
        return 0;
    else if(strncmp(mac, "ECF342", 3)==0) //Oppo Mobile
        return 1;
    else if(strncmp(mac, "009EC8", 3)==0) //Xiaomi Comm.
        return 1;
    else if(strncmp(mac, "0000F0", 3)==0) //Samsung Mobile
        return 1;
    else if(strncmp(mac, "FCFC48", 3)==0) //Apple Mobile
        return 1;
    else if(strncmp(mac, "0050F2", 3)==0) //Microsoft Corporate
        return 0;
    else if(strncmp(mac, "FCE557", 3)==0) //Nokia Mobile
        return 1;
    else if(strncmp(mac, "000039", 3)==0) //Toshiba Laptop
        return 0;
    else if(strncmp(mac, "F80CF3", 3)==0) //LG Mobile
        return 1;
    else if(strncmp(mac, "000255", 3)==0) //IBM PC
        return 0;
    else if(strncmp(mac, "000C76", 3)==0) //Micro-Star International Co
        return 0;
    else
        return (rand()%2);
}

void print_device_info(unsigned char* Buffer, int Size)		//To print MAC Address and device type
{
    struct ethhdr *eth = (struct ethhdr *)Buffer;
    int typeDest = getType(eth->h_dest[0], eth->h_dest[1], eth->h_dest[2]);
    int typeSrc = getType(eth->h_source[0] , eth->h_source[1] , eth->h_source[2]);

    printf("\n******************************************************************\n\n");

    //Destination Mac Address
    printf("Destination Mac Address\t: %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n", eth->h_dest[0] , eth->h_dest[1] , eth->h_dest[2] , eth->h_dest[3] , eth->h_dest[4] , eth->h_dest[5] );
    
    //Destination Device Type
    if(typeDest == 1)
	printf("Destination Device is\t: %s\n", "Mobile");
    else
	printf("Destination Device is\t: %s\n", "Desktop");

    printf("\n");

    //Source Mac Address
    printf("Source Mac Address\t: %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n", eth->h_source[0] , eth->h_source[1] , eth->h_source[2] , eth->h_source[3] , eth->h_source[4] , eth->h_source[5] );
    
    // Source Device Type
    if(typeSrc == 1)
	printf("Source Device is\t: %s\n", "Mobile");
    else
	printf("Source Device is\t: %s\n", "Desktop");
}


int main()
{
    int saddr_size , data_size;
    struct sockaddr saddr;
    unsigned char *buffer = (unsigned char *) malloc(65536);

    printf("Starting...\n");
    int sock_raw = socket( AF_PACKET , SOCK_RAW , htons(ETH_P_ALL)) ;
    //Detect Error while connecting to Raw Socket
    if(sock_raw < 0) 
    {
	perror("Socket Error");
	return 1;
    }

    while(1)
    {
	saddr_size = sizeof saddr;
	    
	//Receive the packet
	data_size = recvfrom(sock_raw , buffer , 65536 , 0 , &saddr , (socklen_t*)&saddr_size);
	
	//Detect Error while Recieving packet
	if(data_size <0 )
	{
	    printf("Recvfrom error , failed to get packets\n");
	    return 1;
	}
	    
	//Now we can process the packet
	ProcessPacket(buffer , data_size);
    }
    close(sock_raw);
    printf("Finished");
    return 0;
}
