/*

Copyright (c) 2011 James Robson

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/


#include "bashapp.h"

int main(int argc, char *argv[]) {

    int i = 0, key_len;
    char *sz = NULL;
    char *script = NULL;
    char *script_e = NULL;
    char *src = NULL;
    char *tmp = NULL, *tmp2 = NULL, *tmp3 = NULL;
    char *key = NULL;


    // Handle arguments
    if (argc == 1 || argc == 2) {
        printf("%s", USAGE);
        return 0;
    }

    if (argc == 3) { // Use default key, recommended, since it's random + variable length
        srand( time(NULL) );
        key_len = (unsigned int)rand() % 
            ( ( (DEFAULT_KEY_LEN * 3) - DEFAULT_KEY_LEN ) + 1 ) + DEFAULT_KEY_LEN;
        key = rand_array(32, 254, key_len);
    }
    else { // User specifies key
        key = calloc(strlen(argv[3])+1, sizeof(char)); 
        strncpy(key, argv[3], strlen(argv[3]));
        key_len = strlen(argv[3])+1;
    }


    // Load up the script file
    i = load_script(argv[1], &script);

    // Get the script array size in a string formatted number
    sz = calloc( 10, sizeof(char) );
    sprintf(sz, "%d", i);
    

    // Encrypt the script with the given key
    script_e = xor_enc(script, i+1, key, key_len-1);
    tmp = src_hex_array(script_e, i);


    // Create code from template
    tmp2 = replace(C_TEMPLATE, "___BASH_SCR___", tmp);
    cats(&tmp, NULL);

    tmp3 = src_hex_array(key, key_len);
    tmp = replace(tmp2, "___KEY___", tmp3);
    cats(&tmp2, NULL);
    cats(&tmp3, NULL);

    tmp2 = replace(tmp, "___BASH_SCR_SIZE___", sz);
    cats(&tmp, NULL);

    sprintf(sz, "%d", key_len);
    src = replace(tmp2, "___KEY_LEN___", sz);
    cats(&tmp2, NULL);


    // Write the .c source file
    cats(&tmp, argv[2]);
    cats(&tmp, ".c");
    write_file(tmp, src);
    cats(&tmp, NULL);
    
    // Create and run our 'make' batch script
    tmp = replace(MAKE_APP, "___APPNAME___", argv[2]);
    i = system(tmp);

    return 0;
}
