#define FILENAME "big.png"
#define MIMETYPE "image/png"
#include <sys/types.h>
#include <string.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <winsock2.h>
#include <microhttpd.h>
#include <windows.h>
#include "handler/handler.h"


#define PORT 8888

enum MHD_Result answer_to_connection (void *cls, struct MHD_Connection *connection, const char *url, const char *method, const char *version, const char *upload_data,
size_t *upload_data_size, void **req_cls){

    const char *page = return_string();
    const char *another_page = return_another_page();
    const char *error_page = return_not_found();
    const char *image_page_file = images_page();
    const char *filename = FILENAME;

    unsigned char *buffer = NULL;

    struct MHD_Response *response;
    int ret;

    
    if(strcmp(url,"/index.html")==0){
        response = MHD_create_response_from_buffer(strlen(page),(void*)page, MHD_RESPMEM_PERSISTENT);
        ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    
        MHD_destroy_response(response);
        MHD_get_connection_values(connection,MHD_HEADER_KIND, &print_out_key, NULL);

        printf("New %s request for %s using version %s\n", method, url, version);
        
        return ret;
    }
    else if(strcmp(url,"/another.html")==0){
        response = MHD_create_response_from_buffer(strlen(another_page),(void*)another_page, MHD_RESPMEM_PERSISTENT);
        ret = MHD_queue_response(connection,MHD_HTTP_OK,response);

        MHD_destroy_response(response);
        MHD_get_connection_values(connection,MHD_HEADER_KIND, &print_out_key, NULL);

        printf("New %s request for %s using version %s\n", method, url, version);
        
        return ret;

    }
    else if(strcmp(url,"/images.html")==0){
        response = MHD_create_response_from_buffer(strlen(image_page_file),  (void*)image_page_file,MHD_RESPMEM_PERSISTENT);
        ret = MHD_queue_response(connection,MHD_HTTP_OK,response);
        MHD_destroy_response(response);
        return ret;
        
    }
    else{
        response = MHD_create_response_from_buffer(strlen(error_page), (void*)error_page, MHD_RESPMEM_PERSISTENT);
        ret = MHD_queue_response(connection,MHD_HTTP_NOT_FOUND,response);

        MHD_destroy_response(response);
        return ret;
        
    }
}

const char *return_string(void){
    const char *test_string = "<html><body><a href='/another.html'>Go to another page</a><br><a href='/images.html'>Go to images here</a></body></html>";

    return test_string;
}

const char *return_another_page(void){
    const char *test_string = "<html><body>Yep thats another...</body></html>";

    return test_string;
}

const char *return_not_found(void){
    const char *test_string = "<html><body>404 NOT FOUND</body></html>";

    return test_string;
}

const char *images_page(void){
    const char *images_page_string = "<html><body><a href='/image.html'>Go to normal image</a><br><a href='/big.html'>Go to bigger image</a></body></html>";
    
    return images_page_string;
}

enum MHD_Result print_out_key(void *cls, enum MHD_ValueKind kind, const char *key, const char *value){
    printf("%s: %s\n", key, value);
    return MHD_YES;
}

int handle_image_request(const char *method, const char *url, struct MHD_Connection *connection){
    enum MHD_Result ret;
    int fd;
    unsigned char *buffer;
    struct MHD_Response *response;
    struct stat sbuf;
    char *filename;

    if(0 != strcmp(method, "GET")){
        return MHD_NO;
    }
    
    filename = file_to_read(url);

    if((-1 == (fd=open(filename, O_RDONLY)))||(0 != fstat(fd,&sbuf))){
    
        close(fd);
        const char *errorstr = "<html><body>An internal server error has occurred!</body></html>";
        response = MHD_create_response_from_buffer(strlen(errorstr),(void*)errorstr,MHD_RESPMEM_PERSISTENT);
        if(response){
            ret = MHD_queue_response(connection,MHD_HTTP_INTERNAL_SERVER_ERROR,response);
            MHD_destroy_response(response);
            return MHD_YES;
        }
        else{
            return MHD_NO;
        }
        
        if(!ret){
            const char *errorstr = "<html><body>An internal server error has occurred!</body></html>";
            if(buffer) free(buffer);

            response = MHD_create_response_from_buffer(strlen(errorstr), (void*)errorstr, MHD_RESPMEM_PERSISTENT);
            if(response){
                ret = MHD_queue_response (connection,
                MHD_HTTP_INTERNAL_SERVER_ERROR,
                response);
                MHD_destroy_response (response);
                return MHD_YES;
            }
            else{
                return MHD_NO;
            }
        }
    }
   
    response = MHD_create_response_from_fd_at_offset64(sbuf.st_size,fd,0);
    if(strcmp(filename,"big.png")==0){
        Sleep(30);
    }
    const char *header = "Content-Type";
    MHD_add_response_header(response,header,MIMETYPE);
    ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);
    return ret;
}

const char *file_to_read(const char *url){
    if(strcmp(url,"/big.html")==0){
        const char *file_name="big.png";
        return file_name;
    }
    else if(strcmp(url,"/image.html")==0){
        
        const char *file_name="picture.png";
        return file_name;
    }
}


static enum MHD_Result on_client_connect(void *cls, const struct sockaddr *addr, socklen_t addrlen){
    //sockaddr = C data structure used in netwrok programming to store socket address and connection info
    //socaddr_in = _in stands for Internet, specifically the traditional IPv4 socket address structure.
    //addr_in->sin_addr = Go to the sockaddr_in structure and retrieve its sin_addr member
    //const struct sockaddr *addr = Means this variable is a pointer, the addr variable points to a struct sockaddr,because the strucs can contain quite bit of data
    //this way we tell it to give us the address of it only, then we use that address to access what we need.
    //addr → points to the structure.

    //*addr → the structure itself.

    //&addr → where the pointer variable addr itself is stored.

    //the part below explained:
    //                         SAME MEMORY
    //                          │
    //                          ↓

    // addr ──────────────→ ┌──────────────────────┐
    //                      │ sockaddr_in data     │
    //                      │                      │
    //                      │ sin_family           │
    //                      │ sin_port              │
    //                      │ sin_addr             │
    //                      └──────────────────────┘
    //                          ↑
    //                          │
    // addr_in ─────────────────┘
    //What we do is we are telling C how to look at things, since addr is generic data structure for network, we need C to look at it from perspective that it can retrieve
    //some members that we need, where we configure to sockaddr_in, essentially: 
    //"I know addr is declared as a pointer to struct sockaddr, but I want to interpret the memory it points to as a struct sockaddr_in"
    const struct sockaddr_in *addr_in = (const struct sockaddr_in *)addr;

    
    // printf("HEY\n");

    // printf("cls: %p\n", cls);
    // printf("addr:  %d\n",addr_in);
    // printf("addrlen: %d\n", (int)addrlen);
    // printf("IP address: %s\n", inet_ntoa(addr_in->sin_addr));
    // printf("Address family: %d\n", addr->sa_family);
    
    return MHD_YES;

}