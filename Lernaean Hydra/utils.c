/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asyed <asyed@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/03 17:54:25 by asyed             #+#    #+#             */
/*   Updated: 2018/03/03 18:01:22 by asyed            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "lernaen.h"

int		enabled(const char *file)
{
	FILE	*fp;
	char	buf[10];

	if ((fp = fopen(file, "r")) == NULL)
		return (-1);
	fread(buf, sizeof(char), 1, fp);
	return ((int)(buf[0] - '0'));
}

u_short	csum(u_short *addr, size_t len)
{
	unsigned long	sum;

	sum = 0;
	while (len > 0)
	{
		sum += *addr++;
		len--;
	}
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	return ((unsigned short)~sum);
}
