/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obelaizi <obelaizi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/08 22:22:06 by obelaizi          #+#    #+#             */
/*   Updated: 2023/07/30 02:55:32 by obelaizi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*env_value(char *key)
{
	t_env	*node;
	char	*tmp;
	int		i;

	node = g_data.env;
	if (!ft_strncmp(key, "?", 1) && !(ft_isalnum(key[1]) || key[1] == '_'))
		return (ft_itoa(g_data.exit_status));
	tmp = malloc(ft_strlen(key) + 1);
	i = 0;
	while (key[i] && (ft_isalnum(key[i]) || key[i] == '_'))
	{
		tmp[i] = key[i];
		i++;
	}
	tmp[i] = '\0';
	if (!ft_strcmp(tmp, "?"))
		return (free(tmp), ft_itoa(g_data.exit_status));
	while (node)
	{
		if (!ft_strcmp(node->key, tmp))
			return (free(tmp), node->value);
		node = node->next;
	}
	return (free(tmp), NULL);
}

void	trim_it(char *s)
{
	int		i;
	int		j;

	i = -1;
	j = -1;
	if (!ft_strncmp(s, "?", 1))
	{
		while (s[++i])
			s[i] = s[i + 1];
		return ;
	}
	while (s[++i] && (ft_isalnum(s[i]) || s[i] == '_'))
		;
	while (s[i])
		s[++j] = s[i++];
	s[++j] = '\0';
}

bool	is_export(t_cmd *cmd)
{
	t_cmd	*node;

	node = cmd;
	while (node)
	{
		if (node->type == CMD)
		{
			if (!ft_strcmp(node->s, "export"))
				return (true);
			else
				return (false);
		}
		node = node->next;
	}
	return (false);
}

void	token_expand(t_cmd *cmd)
{
	while (cmd)
	{
		cmd->type = CMD;
		cmd = cmd->next;
	}
}

void	expand_it(t_cmd *node, int *i, int *before_equal, int *expanded)
{
	char	*val;
	char	*rest;
	char	*tmp;

	if (i && node->s[*i - 1] == '=')
		*before_equal = 1;
	else
		*before_equal = 0;
	*expanded = 1;
	(*i)++;
	rest = ft_substr(node->s, *i, ft_strlen(node->s));
	trim_it(rest);
	val = env_value(&node->s[*i]);
	node->s[*i - 1] = '\0';
	tmp = node->s;
	if (val)
		node->s = ft_strjoin(node->s, val);
	tmp = node->s;
	node->s = ft_strjoin(node->s, rest);
	(*i)--;
}

void	link_the_expanded(t_cmd *node, t_pars **pars)
{
	t_cmd	*tmp_next;
	t_cmd	*tmp_node;

	tmp_next = node->next;
	tmp_node = NULL;
	cust_split(node->s, &tmp_node);
	token_expand(tmp_node);
	cmd_add_back(&tmp_node, tmp_next);
	if (!node->prev)
		(*pars)->cmd = tmp_node;
	else
		node->prev->next = tmp_node;
}

void	is_it_expanded(t_cmd *node, t_pars **pars)
{
	int		expanded;
	int		i;
	int		flg;
	int		before_equal;

	expanded = 0;
	flg = 0;
	i = -1;
	while (node->s[++i] && node->type != DELIM)
	{
		if ((node->s[i] == '\'' || node->s[i] == '"') && !flg)
			flg = i + 1;
		else if (flg && node->s[i] == node->s[flg - 1])
			flg = 0;
		else if (node->s[i] == '$' && node->s[i + 1]
			&& (!flg || (flg && node->s[flg - 1] == '"')))
			expand_it(node, &i, &before_equal, &expanded);
	}
	if ((!is_export((*pars)->cmd)
			|| (is_export((*pars)->cmd) && !before_equal)) && expanded)
		link_the_expanded(node, pars);
}

void	expand(t_pars *pars)
{
	t_cmd	*node;
	t_pars	*tmp;

	tmp = pars;
	while (pars)
	{
		pars->in = FD_INIT;
		pars->out = FD_INIT;
		node = pars->cmd;
		while (node)
		{
			is_it_expanded(node, &pars);
			node = node->next;
		}
		pars = pars->next;
	}
}
