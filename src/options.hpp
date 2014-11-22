#pragma once


struct unknown_option_error : std::exception {
	unknown_option_error(char sopt) {
		char xx[3] = { '-', sopt, 0 };
		opt = xx;
	}
	unknown_option_error(const string& sopt) {
		opt = sopt;
	}

	virtual ~unknown_option_error() throw() {
	}

	virtual const char* what() const throw() {
		return opt.c_str();
	}

	string opt;
};

struct option_item {
	option_item() {
		_flag = false;
	}

	bool operator==(const option_item& other) const {
		if (this == &other)
			return true;
		return _name == other._name;
	}

	bool operator!=(const option_item& other) const {
		return !(*this == other);
	}

	bool flag() const {
		return _flag;
	}

	const string& value() const {
		return _value;
	}

	string _doc;
	string _name; // --name
	string _value; // -n <value> OR --name=<value>
	bool _flag; // true if option is set
};

struct option : public option_item {
	enum Type {
		None,
		Flag,
		Value
	};

	option() {
		_type = None;
		_shortname = 0;
	}

	option& doc(const char* docstr) {
		_doc = docstr;
		return *this;
	}

	option& default_(bool flag) {
		_flag = flag;
		return *this;
	}

	option& default_(const char* val) {
		_value = val;
		_flag = true;
		return *this;
	}

	const char& shortname() const {
		return _shortname;
	}

	Type _type;
	char _shortname; // -n
};

struct argument : public option_item {
	argument& doc(const char* docstr) {
		_doc = docstr;
		return *this;
	}
	argument& default_(const char* val) {
		_value = val;
		_flag = true;
		return *this;
	}
};

struct command_desc {
	typedef dict<string, option> optionlist;
	typedef vector<argument> arglist;

	command_desc& doc(const char* docstr)
	{
		_doc = docstr;
		return *this;
	}

	argument& add_arg(const char* name)
	{
		argument arg;
		arg._name = name;
		_arg.push_back(arg);
		return _arg.back();
	}

	option& add_opt(const char* shortform, const char* name, option::Type type)
	{
		option opt;
		if (name[0] == '-' && name[1] == '-')
			name += 2;
		opt._name = name;
		opt._type = type;
		if (shortform) {
			if (shortform[0] == '-')
				shortform++;
			opt._shortname = shortform[0];
		}
		auto x = _opt.emplace(name, opt);
		if (x.second == false) {
			error("Option %s added twice.", name);
		}
		return x.first->second;
	}

	option& add_opt(const char* name, option::Type type) {
		return add_opt(0, name, type);
	}

	option& operator[](const char* name) {
		auto i = _opt.find(name);
		if (i == _opt.end())
			throw unknown_option_error(name);
		return i->second;
	}

	argument& arg(int index) {
		if (index >= (int)_arg.size())
			throw unknown_option_error(string(strfmt<128>("positional argument #%d", index)));
		return _arg[index];
	}

	option& get_short(char shortname) {
		for (auto& o : _opt)
			if (o.second.shortname() == shortname)
				return o.second;
		throw unknown_option_error(shortname);
	}

	optionlist _opt;
	arglist _arg;
	string _doc;
};

struct optionparser {
	typedef dict<string, command_desc> commandmap;

	bool break_longopt(char* str, string& key, string& value) {

		char* split = strchr(str, '=');
		if (split) {
			key.assign(str, split-str);
			value = split+1;
		}
		else {
			key = str;
		}
		return true;
	}

	bool parse(const char* command, int argc, char* argv[]) {
		command_desc& desc = _set[command];
		int curr_positional = 0;
		for (int i = 0; i < argc; ++i) {
			char* argn = argv[i];
			if (*argn == '-' && *(argn+1) == '-') {
				argn += 2;
				try {
					// long opt
					string key;
					string value;
					if (break_longopt(argn, key, value)) {
						option& opt = desc[key.c_str()];
						if (opt._type == option::Flag) {
							opt._flag = true;
						}
						else if (opt._type == option::Value) {
							opt._value = value;
							opt._flag = true;
						}
					}
					else {
						error("Unable to parse %s", argn);
						return false;
					}
				}
				catch (const unknown_option_error& opt) {
					error("Unknown option: %s", opt.what());
					return false;
				}
			}
			else if (*argn == '-') {
				argn += 1;
				// short opt
				try {
					option& opt = desc.get_short(*argn);
					if (opt._type == option::Flag) {
						opt._flag = true;
					}
					else if (opt._type == option::Value) {
						opt._value = argv[i+1];
						opt._flag = true;
						i++;
					}
				}
				catch (const unknown_option_error& opt) {
					error("Unknown option: %s", opt.what());
					return false;
				}
			}
			else {
				// positional
				if (curr_positional >= (int)desc._arg.size()) {
					error("Too many arguments to %s", command);
					return false;
				}
				else {
					desc._arg[curr_positional]._value = argn;
					desc._arg[curr_positional]._flag = true;
					curr_positional++;
				}
			}
		}
		return true;
	}

	command_desc& operator()(const char* name) {
		return _set[name];
	}

	commandmap _set;
};
