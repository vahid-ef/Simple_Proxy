#include <stdio.h>
#include "./inc/proxy_lib.h"

int main(int argc, char* argv[]){

    if(argc < 2){
        printf("%s\n", "Usage: proxy1 <URL>");
        return EXIT_FAILURE;
    }

    proxy_proces_url(argv[1]);

    return EXIT_SUCCESS;

}
