/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   functionsHelpers2.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obelaizi <obelaizi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/11 01:25:51 by obelaizi          #+#    #+#             */
/*   Updated: 2023/07/22 23:36:42 by obelaizi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

int	redirect_error(t_cmd *node)
{
	if (node->type == HEREDOC && (!node->next || node->next->type != DELIM))
		return (1);
	return ((node->type == OUT || node->type == IN
			|| node->type == APPEND) && (!node->next
			|| node->next->type != FD));
}

int	is_syntax_error(void)
{
	t_cmd	*tmp;

	tmp = g_data.cmds;
	if (tmp->type == PIPE)
		return (g_data.exit_status = 258);
	while (tmp)
	{
		if (tmp->type == PIPE && !tmp->next)
			return (g_data.exit_status = 258);
		if (tmp->type == PIPE && tmp->next->type == PIPE)
			return (g_data.exit_status = 258);
		if (tmp->type == PIPE && tmp->prev && (tmp->prev->type == PIPE
				|| tmp->prev->type == OUT || tmp->prev->type == IN
				|| tmp->prev->type == APPEND || tmp->prev->type == HEREDOC))
			return (g_data.exit_status = 258);
		if (redirect_error(tmp))
			return (g_data.exit_status = 258);
		tmp = tmp->next;
	}
	return (0);
}

void	make_cmd_prev(void)
{
	t_cmd	*tmp;

	g_data.cmds->prev = 0;
	tmp = g_data.cmds;
	while (tmp->next)
	{
		tmp->next->prev = tmp;
		tmp = tmp->next;
	}
}

void	make_pars_prev(void)
{
	t_pars	*tmp;

	g_data.pars->prev = 0;
	tmp = g_data.pars;
	while (tmp->next)
	{
		tmp->next->prev = tmp;
		tmp = tmp->next;
	}
}
