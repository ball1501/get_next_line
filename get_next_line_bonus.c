/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_bonus.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: wngamkri <wngamkri@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/12 10:44:00 by wngamkri          #+#    #+#             */
/*   Updated: 2026/09/12 10:46:24 by wngamkri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

static char	*ft_read_to_stash(int fd, char *stash)
{
	char	*buffer;
	int		bytes_read;

	buffer = (char *)malloc(BUFFER_SIZE + 1);
	if (!buffer)
		return (free(stash), NULL);
	bytes_read = 1;
	while (!ft_strchr(stash, '\n') && bytes_read > 0)
	{
		bytes_read = read(fd, buffer, BUFFER_SIZE);
		if (bytes_read == -1)
		{
			free(buffer);
			free(stash);
			return (NULL);
		}
		buffer[bytes_read] = '\0';
		stash = ft_strjoin(stash, buffer);
	}
	free(buffer);
	return (stash);
}

static char	*ft_extract_line(char *stash)
{
	size_t	i;
	size_t	j;
	char	*ptr;

	if (!stash || !stash[0])
		return (NULL);
	i = 0;
	while (stash[i] != '\0' && stash[i] != '\n')
		i++;
	if (stash[i] == '\n')
		i++;
	ptr = (char *)malloc(sizeof(char) * (i + 1));
	if (!ptr)
		return (NULL);
	j = 0;
	while (j < i)
	{
		ptr[j] = stash[j];
		j++;
	}
	ptr[j] = '\0';
	return (ptr);
}

static char	*ft_clean_stash(char *stash)
{
	size_t	i;
	size_t	j;
	char	*new_stash;

	i = 0;
	while (stash && stash[i] && stash[i] != '\n')
		i++;
	if (!stash || !stash[i] || !stash[i + 1])
	{
		free(stash);
		return (NULL);
	}
	new_stash = (char *)malloc(sizeof(char) * (ft_strlen(stash) - i));
	if (!new_stash)
		return (free(stash), NULL);
	j = 0;
	i++;
	while (stash[i])
		new_stash[j++] = stash[i++];
	new_stash[j] = '\0';
	free(stash);
	return (new_stash);
}

char	*get_next_line(int fd)
{
	static char	*stash[1024];
	char		*line;

	if (fd < 0 || fd >= 1024 || BUFFER_SIZE <= 0)
		return (NULL);
	stash[fd] = ft_read_to_stash(fd, stash[fd]);
	if (!stash[fd])
		return (NULL);
	line = ft_extract_line(stash[fd]);
	stash[fd] = ft_clean_stash(stash[fd]);
	return (line);
}
