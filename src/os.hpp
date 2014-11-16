#pragma once

namespace util {
	namespace os {
		const char* environ(const char* var);
		void set_environ(const char* var, const char* val);

		namespace path {
			const char* basename(const char* full_path);
			const char* extension(const char* full_path);
			string path(const char* full_path);

			bool isdir(const char* fname);
			bool isexec(const char* fname);
			bool exists(const char* filename);

			bool makedir(const char* dir);
			bool makepath(const char* dir);

			inline const char* basename(const string& full_path) { return basename(full_path.c_str()); }
			inline const char* extension(const string& full_path) { return extension(full_path.c_str()); }
			inline string path(const string& full_path) { return path(full_path.c_str()); }

			inline bool isdir(const string& fname) { return isdir(fname.c_str()); }
			inline bool isexec(const string& fname) { return isexec(fname.c_str()); }
			inline bool exists(const string& filename) { return exists(filename.c_str()); }
		}
	}
}
