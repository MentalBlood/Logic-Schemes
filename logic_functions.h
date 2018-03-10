void AND(vector<bool*> &in, vector<bool*> &out)
{
	for (int i = 0; i < in.size(); i++)
		if (!*in[i])
		{
			*out[0] = 0;
			return;
		}
	*out[0] = 1;
}

void OR(vector<bool*> &in, vector<bool*> &out)
{
	for (int i = 0; i < in.size(); i++)
		if (*in[i])
		{
			*out[0] = 1;
			return;
		}
	*out[0] = 0;
}

void NOT(vector<bool*> &in, vector<bool*> &out)
{
	for (int i = 0; i < in.size(); i++) *out[i] = !*in[i];
}
