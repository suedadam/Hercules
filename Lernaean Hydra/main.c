/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asyed <asyed@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/01 14:00:01 by asyed             #+#    #+#             */
/*   Updated: 2018/03/01 17:03:22 by asyed            ###   ########.fr       */
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
#define BACKLOG 1024

/*
** Need to still establish the (s/d)addr.
*/
static void build_ip(struct ip *ip)
{
	ip->ip_hl = 5;
	ip->ip_v = 4;
	ip->ip_tos = 0;
    ip->ip_len  = sizeof(struct ip) + sizeof(struct icmphdr);
    ip->ip_ttl = 255;
    ip->ip_p = IPPROTO_ICMP;
}

static int	enabled(const char *file)
{
	FILE	*fp;
	char	buf[10];

	if ((fp = fopen(file, "r")) == NULL)
		return (-1);
	fread(buf, sizeof(char), 1, fp);
	return ((int)(buf[0]));
}

int	main(void)
{
	int					s_sock, c_sock;
	size_t				ret;
	char				buf[(sizeof(struct ip) + sizeof(struct icmp) + 1)];
	struct icmp			*icmppacket;
	struct ip			*ipreply;
	struct sockaddr_in	conn;
	void				*packet;

	if ((ret = fork()) == -1)
		return (-1);
	else if (ret > 0)
		return (0);
	if ((s_sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1)
	{
		printf("ERR: %s\n", strerror(errno));
		return (s_sock);
	}
	if (!(packet = malloc(sizeof(struct ip) + sizeof(struct icmp))))
		return (-1);
	bzero(packet, sizeof(struct ip) + sizeof(struct icmp));
	bzero(buf, sizeof(struct ip) + sizeof(struct icmp) + 1);
	ipreply = (struct ip *)packet;
	icmppacket = (struct icmp *)(packet + sizeof(struct ip));
	build_ip(ipreply);
	icmppacket->icmp_type = ICMP_ECHOREPLY;
	if ((ret = enabled("/proc/sys/net/ipv4/icmp_echo_ignore_all")) == -1)
	{
		printf("Error!\n");
		ret = 1;
	}
	while (recv(s_sock, buf, sizeof(struct ip) + sizeof(struct icmp), 0))
	{
		if (((struct icmp *)(buf + sizeof(struct ip)))->icmp_type == ICMP_ECHO)
		{
			memcpy(&(ipreply->ip_src.s_addr), &(((struct ip *)buf)->ip_dst.s_addr), INET_ADDRSTRLEN);
			memcpy(&(ipreply->ip_dst.s_addr), &(((struct ip *)buf)->ip_src.s_addr), INET_ADDRSTRLEN);
			char str[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &(ipreply->ip_dst.s_addr), str, INET_ADDRSTRLEN);
			conn.sin_family = AF_INET;
			conn.sin_addr.s_addr = ipreply->ip_dst.s_addr;
			if (ret)
				sendto(s_sock, packet, ipreply->ip_len, 0, (struct sockaddr *)&conn, sizeof(struct sockaddr));
			sendto(s_sock, packet, ipreply->ip_len, 0, (struct sockaddr *)&conn, sizeof(struct sockaddr));
			printf("PING RECEVIED str = %s\n", str);
		}
		else
			printf("PONG\n");
		// printf("Received an ICMP packet (Type = %d | Code = %d)\n", icmppacket->icmp_type, icmppacket->icmp_code);
		// printf("Received an ICMP packet (Type = %d | Code = %d)\n", ((struct icmp *)(buf + sizeof(struct ip)))->icmp_type, ((struct icmp *)(buf + sizeof(struct ip)))->icmp_code);
	}
}