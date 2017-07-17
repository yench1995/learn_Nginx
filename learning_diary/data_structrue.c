#include <unistd.h>
#include <sys/types.h>
#include <stdint.h>

//使用ngx_int_t封装有符号整型，使用ngx_uint_t封装无符号整型
typedef intptr_t ngx_int_t;
typedef uintptr_t ngx_uint_t;
//ngx_str_t就是字符串，data指针指向字符串起始地址，len表示字符串的有效长度（因为字符串未必会以'\0'结尾
typedef struct {
    size_t len;
    u_char *data;
} ngx_str_t;

//ngx_list_t数据结构
//ngx_list_part_t描述链表中的一个元素（一个数组），ngx_list_t描述整个链表
typedef struct ngx_list_part_s ngx_list_part_t;
struct ngx_list_part_s{
    void *elts;            //指向数组的起始地址
    ngx_uint_t nelts;      //表示数组中已经使用了多少个元素（必须小于nalloc)
    ngx_list_part_t *next; //下一个链表元素ngx_list_part_t的地址
};

typedef struct {
    ngx_list_part_t *last; //指向链表的最后一个数组元素
    ngx_list_part_t part;  //链表的首个数组元素
    size_t size;           //ngx_list_part_t数组中的元素大小
    ngx_uint_t nalloc;     //每个ngx_list_part_t数组的容量
    ngx_pool_t *pool;      //链表中管理内存分配的内存池对象
} ngx_list_t;

//创建链表
ngx_list_t* testlist = ngx_list_create(r->pool, 4, sizeof(ngx_str_t));
if (testlist == NULL) {
    return NGX_ERROR;
}
//遍历链表
ngx_list_part_t* part = &testlist.part;
ngx_str_t* str = part->elts;

for (int i = 0; ; ++i)
{
    if (i >= part->nelts) {
        if (part->next == NULL)
            break;

        part = part->next;
        str = part->elts;
        i = 0
    }
    printf("list element: %*s\n", str[i].len, str[i].data);
}


//ngx_table_elt_t数据结构, 为HTTP头部请求量身定做的数据结构
typedef struct {
    ngx_uint_t hash;
    ngx_str_t key;
    ngx_str_t value;
    u_char *lowcase_key;
} ngx_table_elt_t;

//ngx_buf_t数据结构
typedef struct ngx_buf_s ngx_buf_t;
typedef void * ngx_buf_tag_t;
struct ngx_buf_s {
    //pos告诉使用者本次从pos位置开始处理内存中的数据
    u_char *pos;
    //last表示有效的内存内容(pos与last之间的内存是希望处理的内容)
    u_char *last;
    //处理文件时，file_pos表示将要处理的文件位置，file_last表示截止的文件位置
    off_t file_pos;
    off_t file_last;
    //如果ngx_buf_t缓冲区用于内存，start指向这段内存的起始地址,end指向末尾
    u_char *start;
    u_char *end;
    //表示当前缓冲区的类型
    ngx_buf_tag_t tag;
    //引用的文件
    ngx_file_t *file;
    //当前缓冲区的影子缓冲区
    ngx_buf_t *shadow;
    //临时内存标识位，为1时表示数据在内存中且这段内存可以修改
    unsigned temporary:1;
    //标识位，为1表示数据在内存中且不可被修改
    unsigned memort:1;
    //标识位，为1时表示这段内存是mmap系统调用映射，不可以被修改
    unsigned mmap:1;
    //标识位，为1时表示可以被回收
    unsigned recycle:1;
    //为1表示缓冲区处理的是文件
    unsigned in_file:1;
    unsigned flush:1;
    unsigned sync:1;
    unsigned last_buf:1;
    unsigned last_in_chain:1;
    unsigned last_shadow:1;
    unsigned temp_file:1;
};

//ngx_chain_t数据结构，与ngx_buf_t配合使用
//在向用户发送HTTP包体时，要传入ngx_chain_t链表对象。如果是最后一个
//ngx_chain_t,必须将next置为NULL，否则永远不会发送成功
//定义一个HTTP模块，把type字段设置为NGX_HTTP_MODULE,还要设置ctx和commands
//ctx指针指向ngx_http_module_t接口,commands数组用于定义模块的配置文件参数
typedef struct ngx_chain_s ngx_chain_t;
struct ngx_chain_s {
    ngx_buf_t *buf;    //当前的ngx_buf_t缓冲区
    ngx_chain_t *next; //下一个ngx_chain_t
};

//ngx_module_t，一个Nginx模块的数据结构
typedef struct ngx_module_s ngx_module_t;
struct ngx_module_s {
#define NGX_MODULE_V1 0,0,0,0,0,1
    //ctx_index表示当前模块在这类模块中的序号
    ngx_uint_t ctx_index;
    //index表示当前模块在ngx_modules数组中的序号
    ngx_uint_t index;

    ngx_uint_t spare0;
    ngx_uint_t spare1;
    ngx_uint_t spare2;
    ngx_uint_t spare3;
    ngx_uint_t version;
    //ctx用于指向一类模块的上下结构体
    void *ctx;
    //commands处理nginx.conf中的配置项
    ngx_command_t *commands;
    //type表示该模块的类型，与ctx指针紧密相关
    ngx_uint_t type;

    //在Nginx的启动，停止过程中，分别调用的七种函数指针方法
    ngx_int_t (*init_master)(ngx_log_t *log);
    ngx_int_t (*init_module)(ngx_cycle_t *cycle);
    ngx_int_t (*init_process)(ngx_cycle_t *cycle);
    ngx_int_t (*init_thread)(ngx_cycle_t *cycle);
    ngx_int_t (*exit_thread)(ngx_cycle_t *cycle);
    ngx_int_t (*exit_process)(ngx_cycle_t *cycle);
    ngx_int_t (*exit_master)(ngx_cycle_t *cycle);

    //以下8个保留字段
    uintptr_t spare_hook0;
    uintptr_t spare_hook1;
    uintptr_t spare_hook2;
    uintptr_t spare_hook3;
    uintptr_t spare_hook4;
    uintptr_t spare_hook5;
    uintptr_t spare_hook6;
    uintptr_t spare_hook7;
};

//ngx_http_module_t结构
//HTTP框架在启动过程中会在每个阶段中调用相应的方法
typedef struct {
    //解析配置文件前调用
    ngx_int_t (*preconfiguration)(ngx_conf_t *cf);
    //完成配置文件的解析后调用
    ngx_int_t (*postconfiguration)(ngx_conf_t *cf);
    //创建数据结构用于存储main级别的全局配置项
    void *(*create_main_conf)(ngx_conf_t *cf);
    //初始化main级别配置项
    char *(*init_main_conf)(nax_conf_t *cf, void *conf);
    //创建数据结构用于存储srv级别的配置项
    void *(*create_srv_conf)(ngx_conf_t *cf);
    //合并main级别和srv级别下的同名配置项
    char *(*merge_srv_conf)(ngx_conf_t *cf, void *prev, void *conf);
    //创建数据结构用于存储loc级别的配置项
    void *(*create_loc_conf)(ngx_conf_t *cf);
    //合并srv级别和loc级别下的同名配置项
    chr *(*merge_loc_conf)(ngx_conf_t *cf, void *prev, void *conf);
} ngx_http_module_t;

//ngx_command_t数据结构
typedef struct ngx_command_s ngx_command_t;
struct ngx_command_s {
    ngx_str_t name;
    ngx_uint_t type;
    //出现了name中指定的配置项后，调用set方法处理配置项的参数
    char *(*set)(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
    //在配置文件中的偏移量
    ngx_uint_t conf;
    //用于使用预设的解析方法解析配置项
    ngx_uint_t offset;
    //配置项读取后的处理方法，必须是ngx_conf_post结构指针
    void *post;
};

//ngx_file_t结构
typedef struct ngx_file_s ngx_file_t;
struct ngx_file_s {
    ngx_fd_t fd;
    ngx_str_t name;
    //文件大小等资源信息，实际就是linux系统定义的stat结构
    ngx_file_info_t info;
    //偏移量，一般不用设置
    off_t offset;
    off_t sys_offset;
    ngx_log_t *log;
    unsigned valid_info:1;
    unsigned directio:1;
};

//打开文件时的操作
ngx_buf_t *b;
b = ngx_palloc(r->pool, sizeof(ngx_buf_t));
u_char* filename = (u_char*)"/tmp/test.txt";
b->in_file = 1;
b->file = ngx_pcalloc(r->pool, sizeof(ngx_file_t));
b->file->fd = ngx_open_file(filename, NGX_FILE_RDONLY|NGX_FILE_NONBLOCK, NGX_FILE_OPEN, 0);
b->file->log = r->connection->log;
b->file->name.data = filename;
b->file-<name.len = strlen(filename);
if (b->file->fd <= 0)
    return NGX_HTTP_NOT_FOUND;
//还要告知Nginx文件的大小，设置Content_Length头部，设置ngx_buf_t缓冲区的file_pos和file_last
if (ngx_file_info(filename, &b->file->info) == NGX_FILE_ERROR)
    return NGX_HTTP_INTERNAL_SERVER_ERROR;
r->headers_out.content_length_n = b->file->info.st_size;
b->file_pos = 0;
b->file_last = b->file->info.st_size;

//ngx_pool_cleanup_t结构体
typedef struct ngx_pool_cleanup_s ngx_pool_cleanup_t;
struct ngx_pool_cleanup_s {
    //执行清理资源工作的回调方法
    ngx_pool_cleanup_pt handler;
    //handler回调方法需要的参数
    void *data;
    ngx_pool_cleanup_t *next;
};

//ngx_pool_clean_file_t结构，即上面的data所需要的
typedef struct {
    ngx_fd_t fd;
    u_char *name;
    ngx_log_t *log;
} ngx_pool_cleanup_file_t;

//清理文件句柄的完整代码
ngx_pool_cleanup_t* cln = ngx_pool_cleanup_add(r->pool, sizeof(ngx_pool_cleanup_file_t));
if (cln == NULL)
    return NGX_ERROR;
cln->handler = ngx_pool_cleanup_file;
ngx_pool_cleanup_file_t *clnf = cln->data;
clnf->data = b->file->fd;
clnf->name = b->file->name.data;
clnf->log = r->pool->log;
