/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: obelaizi <obelaizi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/13 01:47:36 by obelaizi          #+#    #+#             */
/*   Updated: 2023/07/22 00:33:06 by obelaizi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	set_builtins(void)
{
	g_data.builtins = malloc(sizeof(char *) * 8);
	g_data.builtins[0] = "echo";
	g_data.builtins[1] = "cd";
	g_data.builtins[2] = "pwd";
	g_data.builtins[3] = "export";
	g_data.builtins[4] = "unset";
	g_data.builtins[5] = "env";
	g_data.builtins[6] = "exit";
	g_data.builtins[7] = NULL;
}

// t_pars	*set_parsed(t_cmd *cmd)
// {
// 	t_pars	*parsed;
// 	t_cmd	*tmp;
// 	t_cmd	*tmp2;

// 	parsed = g_data.pars;
// 	parsed = malloc(sizeof(t_pars));
// 	parsed->cmd = cmd;
// 	tmp2 = parsed->cmd;
// 	while (parsed->cmd)
// 	{
// 		if (parsed->cmd->type == PIPE)
// 		{
// 			tmp = parsed->cmd->next;
// 			parsed->cmd->prev->next = NULL;
// 			break ;
// 		}
// 		parsed->cmd = parsed->cmd->next;
// 	}
// 	parsed->cmd = tmp2;
// 	parsed->next = NULL;
// 	return (parsed);
// }

int	calc_cmd_size(t_cmd *cmd)
{
	int		i;

	i = 0;
	while (cmd)
	{
		if (cmd->type == CMD)
			i++;
		cmd = cmd->next;
	}
	return (i);
}

char	**get_cmds_args(t_cmd *cmd)
{
	char	**args;
	int		i;

	i = 0;
	if (!calc_cmd_size(cmd))
		return (NULL);
	args = malloc(sizeof(char *) * (calc_cmd_size(cmd) + 1));
	while (cmd)
	{
		if (cmd->type == CMD)
			args[i++] = cmd->s;
		cmd = cmd->next;
	}
	args[i] = NULL;
	return (args);
}

void	lunch_herdoc(t_pars *parsed)
{
	t_cmd	*cmd;
	int		in;

	cmd = parsed->cmd;
	while (cmd)
	{
		if (cmd->type == DELIM)
			in = here_doc(cmd->s);
		cmd = cmd->next;
	}
	if (parsed->in == -200)
		parsed->in = in;
}

int	check_builtins(char ***targs)
{
	int		i;
	int		j;
	char	**args;

	i = 0;
	args = *targs;
	while (g_data.builtins[i] && ft_strcmp(g_data.builtins[i], args[0]))
		i++;
	if (!g_data.builtins[i])
		return (0);
	j = -1;
	while (args[++j])
	{
		if (i == 0 && j)
		{
			echo(args + 1);
			return (1);
		}
		else if (i == 1 && j)
			return (cd(args[j]), 1);
		else if (i == 2)
			return (pwd(), 1);
		else if (i == 3 && j)
			export(args[j]);
		else if (i == 4 && j)
			unset(args[j]);
		else if (i == 5 && j)
			return (*targs = *targs + 1, 2);
		else if (i == 6)
		{
			printf("exit\n");
			kill(0, SIGINT);
			exit(1);
		}
	}
	if (i == 3 && j == 1)
		env(0);
	else if (i == 5 && j == 1)
		env(1);
	return (1);
}

void	execute(void)
{
	t_pars	*parsed;
	t_cmd	*cmd;
	char	**args;
	int		ret;
	int		fd[2];
	int		tmp;

	parsed = g_data.pars;
	tmp = -2;
	while (parsed)
	{
		lunch_herdoc(parsed);
		args = get_cmds_args(parsed->cmd);
		if (args && parsed->in != FILE_NOT_FOUND)
		{
			ret = check_builtins(&args);
			if (ret != 1)
			{
				if (pipe(fd) == -1)
				{
					perror("pipe");
					return ;
				}
				if (tmp == -2)
					tmp = fd[0];
				if (fork() == 0)
				{
					if (parsed->next && parsed->out == FD_INIT)
					{
						close(fd[0]);
						dup2(fd[1], 1);
						close(fd[1]);
					}
					if (parsed->prev && parsed->in == FD_INIT)
					{
						close(fd[1]);
						dup2(tmp, 0);
						close(tmp);
						close(fd[0]);
					}
					which_fd(parsed);
					if (ret == 2)
						args[0] = path_cmd(args[0], NO_SUCH_FILE);
					else
						args[0] = path_cmd(args[0], CMD_NT_FND);
					execve(args[0], args, NULL);
					perror("execve");
					exit(1);
				}
				if (tmp != fd[0])
					close(tmp);
				tmp = fd[0];
				close(fd[1]);
			}
		}
		parsed = parsed->next;
	}
	if (tmp != -2)
		close(tmp);
	while (waitpid(-1, NULL, 0) != -1)
		;
	unlink(".temp_file");
}
