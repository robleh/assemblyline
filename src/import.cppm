export module al:import_; // import keyword is reserved 
import :hash;

export namespace al {
extern "C" {

void* get_module_handle(const wchar_t* name);
void* get_module_hash(unsigned long hash, unsigned long (*hasher)(const wchar_t* name));
void* get_proc_address(void* module, const char* name);
void* get_proc_hash(void* module, unsigned long hash, unsigned long (*hasher)(const char* name));

} // extern "C"


void* get_module(const wchar_t* name) {
    return get_module_handle(name);
}

void* get_module(Hash<wchar_t> auto hash) {
    return get_module_hash(hash, hash.alg());
}

void* get_proc(void* mod, const char* name) {
    return get_proc_address(mod, name);
}

void* get_proc(void* mod, Hash<char> auto hash) {
    return get_proc_hash(mod, hash, hash.alg());
}

} // namespace al

