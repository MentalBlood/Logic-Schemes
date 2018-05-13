class Color
{
	public:
		unsigned char R, G, B;
		Color(unsigned char R, unsigned char G, unsigned char B): R(R), G(G), B(B)
		{}
};

void strcpy(unsigned char **dest, unsigned char **src)
{
	//counting name length and allocating memory
	unsigned char *in = *src;
	for (; *in; in++) {}
	*dest = (unsigned char*)malloc(sizeof(unsigned char) * (in-*src+1));

	//copying name
	in = *src; unsigned char *out = *dest;
	for (; *in; in++, out++) *out = *in; *out = *in;
}

class Function
{
	public:
		void (*evaluate)(vector<bool*> &, vector<bool*> &);
		unsigned char *name;
		int max_inputs_number, max_outputs_number,
			min_inputs_number, min_outputs_number;
		
		class Functions_Pack
		{
			public:
				unsigned char *name;
				Color background_color, contour_color;
				vector<Function*> functions;

				Functions_Pack(unsigned char *name_arg, Color background_color, Color contour_color):
					background_color(background_color), contour_color(contour_color)
				{ strcpy(&name, &name_arg); }
		};

		Functions_Pack *pack;

		Function(unsigned char *name_arg, void (*evaluate)(vector<bool*> &, vector<bool*> &), int min_inputs_number, int max_inputs_number, int min_outputs_number, int max_outputs_number, Functions_Pack *pack): 
			evaluate(evaluate), min_inputs_number(min_inputs_number), max_inputs_number(max_inputs_number), min_outputs_number(min_outputs_number), max_outputs_number(max_outputs_number), pack(pack)
		{ strcpy(&name, &name_arg); }
};

class Functions_Base
{
	public:
		vector<Function::Functions_Pack> packs;
		vector<Function> functions;

		bool add_function(unsigned char *name_arg, void (*evaluate)(vector<bool*> &, vector<bool*> &), int min_inputs_number, int max_inputs_number, int min_outputs_number, int max_outputs_number, unsigned char *pack_name)
		{
			for (int i = 0; i < packs.size(); i++)
				if (!strcmp((char*)packs[i].name, (char*)pack_name))
				{
					functions.push_back(Function(name_arg, evaluate, min_inputs_number, max_inputs_number, min_outputs_number, max_outputs_number, &packs[i]));
					packs[i].functions.push_back(&functions[functions.size()-1]);
					return true;
				}
			return false;
		}

		bool remove_pack(int index)
		{
			if (!packs[index].functions.empty()) return false; //if there are functions in this pack
			packs.erase(packs.begin()+index);
			return true;
		}

		void remove_pack_with_functions(Function::Functions_Pack *pack)
		{
			//removing functions
			for (int i = 0; i < pack->functions.size(); i++)
			{
				free(pack->functions[i]->name);
				free(pack->functions[i]);
			}
			pack->functions.clear();
			
			//removing pack
			free(pack->name);
			free(pack);
		}

		void write_functions_by_parameters_to_vector(int inputs, int outputs, vector<Function*> &results)
		{
			results.clear();

			if (!inputs && !outputs)
			{
				for (int i = 0; i < functions.size(); i++) results.push_back(&functions[i]);
				return;
			}

			for (int i = 0; i < functions.size(); i++)
			{
				if (inputs)
					if ((functions[i].max_inputs_number && inputs > functions[i].max_inputs_number) || inputs < functions[i].min_inputs_number) continue;
				if (outputs)
					if ((functions[i].max_outputs_number && outputs > functions[i].max_outputs_number) || outputs < functions[i].min_inputs_number) continue;
				results.push_back(&functions[i]);
			}
		}
};

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
	*out[0] = !*in[0];
}
