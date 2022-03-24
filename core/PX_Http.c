#include "PX_Http.h"

px_int PX_HttpReadToken(const px_char content[], px_char token[],px_int token_size)
{
	px_int size = 0;
	if (token_size)
	{
		token[0] = 0;
	}

	while (token_size>0)
	{
		token_size--;
		if (*content!='\0'&& *content != ' '&& *content != '\r'&& *content != '\n')
		{
			*token = *content;
			token++;
			content++;
			size++;
		}
	}
	*token = 0;

	while (*content == '\0' || *content == ' ' || *content == '\r' || *content == '\n')
	{
		content++;
		size++;
	}
	return size;
}

px_int PX_HttpGetLineDistance(const px_char content[])
{
	px_int i = 0;
	if (!(*content))
	{
		return 0;
	}
	while (PX_TRUE)
	{
		if (content[i] == '\r')
		{
			if (content[i+1] == '\n')
			{
				i+=2;
				break;
			}
		}
		i++;
	}
	return i;
}

px_bool PX_HttpCheckContent(const px_char content[])
{
	px_char token[64] = { 0 };
	if (!content)
	{
		return PX_FALSE;
	}
	PX_HttpReadToken(content, token, sizeof(token));
	if (PX_memequ(token,"GET",3))
	{
		return PX_TRUE;
	}
	if (PX_memequ(token, "POST", 4))
	{
		return PX_TRUE;
	}
	if (PX_memequ(token, "HTTP", 4))
	{
		return PX_TRUE;
	}
	return PX_FALSE;
}

px_bool PX_HttpGetContent(const px_char content[], const px_char requestHeader[], px_char payload[],px_int payload_size)
{
	if (!PX_HttpCheckContent(content))
	{
		return PX_FALSE;
	}
	content += PX_HttpGetLineDistance(content);

	while (PX_TRUE)
	{
		if (PX_memequ(content,requestHeader,PX_strlen(requestHeader)))
		{
			content += PX_strlen(requestHeader);
			while (PX_TRUE)
			{
				if (*content==0)
				{
					break;
				}
				if (*content== ':')
				{
					content++;
					break;
				}
				content++;
			}


			while (PX_TRUE)
			{
				if (*content == 0)
				{
					return PX_FALSE;
				}
				if (content[0] == '\r'&& content[1]=='\n')
				{
					return PX_TRUE;
				}
				if (payload_size>1)
				{
					if (*content!=' ')
					{
						payload[0] = *content;
						payload[1] = 0;
						payload++;
						payload_size--;
					}
				}
				else
				{
					return PX_FALSE;
				}
				content++;
			}

		}
		else
		{
			px_int inc=PX_HttpGetLineDistance(content);
			if (!inc)
			{
				break;
			}
			else
			{
				content += inc;
			}
		}
	}

	return PX_FALSE;
}

px_int PX_HttpGetPacketSize(const px_char content[])
{
	px_int i;
	px_int size = PX_strlen(content);
	for (i=0;i<size-3;i++)
	{
		if (content[i] == 0)
		{
			return 0;
		}

		if (content[i]=='\r'&&content[i+1]=='\n'&& content[i+2] == '\r' && content[i + 3] == '\n')
		{
			return i+4;
		}
		
	}
	return 0;
}

