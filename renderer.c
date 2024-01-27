#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>

#define COMPILE_STRING "clang -shared -undefined dynamic_lookup src/*.c -o bin/game_module.so modules/game_module.c -F/Library/Frameworks -framework SDL2 -Iinclude"

int main(void)
{
    void *state = NULL;
    while(true) {
        while(system(COMPILE_STRING) != 0) {
            fprintf(stderr, "Whoops! Failed to compile!\n");
            fprintf(stderr, "Press return to try again.\n");
            getchar();
	}

        void *module = dlopen("./bin/game_module.so", RTLD_NOW);
        if (module == NULL) {
	    fprintf(stderr, "Failed to load library. (%s)\n", dlerror());
	    fprintf(stderr, "Press return to try again.\n");
	    getchar();
			
	    continue;
        }

        typedef void* module_main_function(void* saved_state);
        module_main_function* module_main = dlsym(module, "GameModule_Main");
        
        state = module_main(state);

        if (state == NULL) { return 0; }
    }
}
