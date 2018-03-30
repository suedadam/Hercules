/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lernaen.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asyed <asyed@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/03 17:24:31 by asyed             #+#    #+#             */
/*   Updated: 2018/03/03 18:01:23 by asyed            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_LERNAEN_H
#define _GNU_SOURCE
#define FT_LERNAEN_H
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

typedef struct	s_packet_info
{
	int					ret;
	int					kern;
	struct icmp			*icmppacket;
	struct ip			*ippacket;
	struct sockaddr_in	conn;
	int					s_sock;
	char				packetbuf[1024];
	size_t				packet_size;
}				t_packet_info;

u_short			csum(u_short *addr, size_t len);
int				enabled(const char *file);

#endif
