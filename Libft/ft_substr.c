/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obelaizi <obelaizi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/12 00:19:37 by obelaizi          #+#    #+#             */
/*   Updated: 2023/07/30 03:31:04 by obelaizi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char	*result;
	int		i;

	if (!s)
		return (0);
	if (len > ft_strlen(s))
		len = ft_strlen(s);
	if (start >= ft_strlen(s))
		return (ft_strdup(""));
	result = malloc(len + 1);
	if (!result)
		return (0);
	garbg_add_back(&g_data.garbage, garbg_new(result));
	while (start--)
		s++;
	i = -1;
	while (s[++i] && len--)
		result[i] = s[i];
	return (result[i] = 0, result);
}
