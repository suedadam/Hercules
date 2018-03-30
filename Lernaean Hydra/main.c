/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asyed <asyed@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/01 14:00:01 by asyed             #+#    #+#             */
/*   Updated: 2018/03/03 18:01:51 by asyed            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lernaen.h"

static int		p_listen(t_packet_info *packet)
{
	if ((packet->s_sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1)
	{
		printf("socket(%s)\n", strerror(errno));
		return (-1);
	}
	if ((packet->ret =
		enabled("/proc/sys/net/ipv4/icmp_echo_ignore_all")) == -1)
	{
		printf("Failed to read sys file for ICMP!!\n");
		packet->ret = 1;
	}
	packet->kern = 1;
	if (setsockopt(packet->s_sock, IPPROTO_IP,
		IP_HDRINCL, &(packet->kern), sizeof(int)) < 0)
	{
		printf("setsockopt(%s)\n", strerror(errno));
		return (-1);
	}
	return (0);
}

static int		copy_ipheader(t_packet_info *packet)
{
	uint32_t	tmp;

	packet->ippacket = (struct ip *)packet->packetbuf;
	packet->icmppacket = (struct icmp *)(packet->packetbuf
							+ sizeof(struct ip));
	tmp = 0;
	tmp = packet->ippacket->ip_src.s_addr;
	memcpy(&(packet->ippacket->ip_src.s_addr),
		&(packet->ippacket->ip_dst.s_addr), sizeof(uint32_t));
	memcpy(&(packet->ippacket->ip_dst.s_addr), &tmp, sizeof(uint32_t));
	return (0);
}

static int		send_packet(t_packet_info *packet)
{
	copy_ipheader(packet);
	packet->packet_size = ntohs(((struct ip *)packet->packetbuf)->ip_len);
	bzero(packet->packetbuf + packet->packet_size, 1024 - packet->packet_size);
	packet->icmppacket->icmp_type = ICMP_ECHOREPLY;
	packet->icmppacket->icmp_id = ((struct icmp *)
							(packet->packetbuf + sizeof(struct ip)))->icmp_id;
	packet->icmppacket->icmp_cksum = 0;
	packet->icmppacket->icmp_cksum = csum((unsigned short *)packet->icmppacket,
							packet->packet_size - sizeof(struct ip));
	packet->ippacket->ip_sum = 0;
	packet->ippacket->ip_sum = csum((unsigned short *)packet->ippacket,
							sizeof(struct ip));
	packet->conn.sin_family = AF_INET;
	packet->conn.sin_addr.s_addr = packet->ippacket->ip_dst.s_addr;
	if (packet->ret == 1)
		if (sendto(packet->s_sock, packet->packetbuf, packet->packet_size,
			0, (struct sockaddr *)&(packet->conn), sizeof(struct sockaddr)) < 0)
			return (-1);
	if (sendto(packet->s_sock, packet->packetbuf, packet->packet_size,
			0, (struct sockaddr *)&(packet->conn), sizeof(struct sockaddr)) < 0)
		return (-1);
	return (0);
}

int				main(int argc, char *argv[])
{
	t_packet_info	packet;

	bzero(&packet, sizeof(t_packet_info));
	p_listen(&packet);
	if (argc >= 2 && !strcmp(argv[1], "-D"))
		if ((packet.ret = daemon(0, 0)) == -1)
			return (packet.ret);
	while (recv(packet.s_sock, packet.packetbuf, 1024, 0))
	{
		if (((struct icmp *)(packet.packetbuf +
			sizeof(struct ip)))->icmp_type == ICMP_ECHO)
		{
			if (send_packet(&packet) == -1)
			{
				printf("Failed to send packet(s) %s\n", strerror(errno));
				return (-1);
			}
		}
	}
}
