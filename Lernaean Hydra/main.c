/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asyed <asyed@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/01 14:00:01 by asyed             #+#    #+#             */
/*   Updated: 2018/03/02 16:09:41 by asyed            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define _GNU_SOURCE

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <unistd.h>

static int	enabled(const char *file)
{
	FILE	*fp;
	char	buf[10];

	if ((fp = fopen(file, "r")) == NULL)
		return (-1);
	fread(buf, sizeof(char), 1, fp);
	return ((int)(buf[0] - '0'));
}

/*
 * in_cksum --
 * Checksum routine for Internet Protocol
 * family headers (C Version)
 */
unsigned short in_cksum(unsigned short *addr, int len)
{
    register int sum = 0;
    u_short answer = 0;
    register u_short *w = addr;
    register int nleft = len;
    /*
     * Our algorithm is simple, using a 32 bit accumulator (sum), we add
     * sequential 16 bit words to it, and at the end, fold back all the
     * carry bits from the top 16 bits into the lower 16 bits.
     */
    while (nleft > 1)
    {
      sum += *w++;
      nleft -= 2;
    }
    /* mop up an odd byte, if necessary */
    if (nleft == 1)
    {
      *(u_char *) (&answer) = *(u_char *) w;
      sum += answer;
    }
    /* add back carry outs from top 16 bits to low 16 bits */
    sum = (sum >> 16) + (sum & 0xffff);       /* add hi 16 to low 16 */
    sum += (sum >> 16);               /* add carry */
    answer = ~sum;              /* truncate to 16 bits */
    return (answer);
}

int	main(void)
{
	int					s_sock;
	int					ret;
	int					kern;
	char				buf[1024];
	struct icmp			*icmppacket;
	struct ip			*ipreply;
	struct sockaddr_in	conn;
	void				*packet;
	size_t				packet_size;

	if ((ret = fork()) == -1)
		return (-1);
	else if (ret > 0)
		return (0);
	if ((s_sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1)
	{
		printf("socket(%s)\n", strerror(errno));
		return (s_sock);
	}
	if (!(packet = malloc(1024)))
		return (-1);
	ipreply = (struct ip *)packet;
	icmppacket = (struct icmp *)(packet + sizeof(struct ip));
	if ((ret = enabled("/proc/sys/net/ipv4/icmp_echo_ignore_all")) == -1)
	{
		printf("Failed to read sys file for ICMP!!\n");
		ret = 1;
	}
	kern = 1;
	if(setsockopt(s_sock, IPPROTO_IP, IP_HDRINCL, &kern, sizeof(int)) < 0)
	{
		printf("setsockopt(%s)\n", strerror(errno));
		exit(-1);
	}
	while (recv(s_sock, buf, 1024, 0))
	{
		if (((struct icmp *)(buf + sizeof(struct ip)))->icmp_type == ICMP_ECHO)
		{
			bzero(packet, 1024);
			packet_size = ntohs(((struct ip *)buf)->ip_len);
			memcpy(packet, buf, packet_size);
			memcpy(&(ipreply->ip_src.s_addr), &(((struct ip *)buf)->ip_dst.s_addr), sizeof(uint32_t));
			memcpy(&(ipreply->ip_dst.s_addr), &(((struct ip *)buf)->ip_src.s_addr), sizeof(uint32_t));
			icmppacket->icmp_type = ICMP_ECHOREPLY;
			icmppacket->icmp_id = ((struct icmp *)(buf + sizeof(struct ip)))->icmp_id;
			icmppacket->icmp_cksum = 0;
			icmppacket->icmp_cksum = in_cksum((unsigned short *)icmppacket, packet_size - sizeof(struct ip));
			ipreply->ip_sum = 0;
			ipreply->ip_sum = in_cksum((unsigned short *)ipreply, sizeof(struct ip));
			conn.sin_family = AF_INET;
			conn.sin_addr.s_addr = ipreply->ip_dst.s_addr;
			if (ret == 1)
				if (sendto(s_sock, packet, packet_size, 0, (struct sockaddr *)&conn, sizeof(struct sockaddr)) < 0)
					printf("Failed to send %s\n", strerror(errno));
			if (sendto(s_sock, packet, packet_size, 0, (struct sockaddr *)&conn, sizeof(struct sockaddr)) < 0)
				printf("Failed to send %s\n", strerror(errno));
			// printf("Received:\n\tIPHL: %d, IPV: %d, IPTOS: %d, IPLEN: %d, IP_ID: %d, IP_OFF: %d, IP_TTL: %d, IP_P: %d, IP_SUM: %d\n\tICMP: Type: %d, Code: %d, cksum: %d, data: \"%s\"\n", ((struct ip *)buf)->ip_hl, ((struct ip *)buf)->ip_v, ((struct ip *)buf)->ip_tos, ((struct ip *)buf)->ip_len, ((struct ip *)buf)->ip_id, ((struct ip *)buf)->ip_off, ((struct ip *)buf)->ip_ttl, ((struct ip *)buf)->ip_p, ((struct ip *)buf)->ip_sum, ((struct icmp *)(buf + sizeof(struct ip)))->icmp_type, ((struct icmp *)(buf + sizeof(struct ip)))->icmp_code, ((struct icmp *)(buf + sizeof(struct ip)))->icmp_cksum, ((struct icmp *)(buf + sizeof(struct ip)))->icmp_data);
		}
		// else
		// 	printf("PONG:\nIPHL: %d, IPV: %d, IPTOS: %d, IPLEN: %d, IP_ID: %d, IP_OFF: %d, IP_TTL: %d, IP_P: %d, IP_SUM: %d\nICMP: Type: %d, Code: %d, cksum: %d, data: \"%s\"\n", ((struct ip *)buf)->ip_hl, ((struct ip *)buf)->ip_v, ((struct ip *)buf)->ip_tos, ((struct ip *)buf)->ip_len, ((struct ip *)buf)->ip_id, ((struct ip *)buf)->ip_off, ((struct ip *)buf)->ip_ttl, ((struct ip *)buf)->ip_p, ((struct ip *)buf)->ip_sum, ((struct icmp *)(buf + sizeof(struct ip)))->icmp_type, ((struct icmp *)(buf + sizeof(struct ip)))->icmp_code, ((struct icmp *)(buf + sizeof(struct ip)))->icmp_cksum, ((struct icmp *)(buf + sizeof(struct ip)))->icmp_data);
	}
}
