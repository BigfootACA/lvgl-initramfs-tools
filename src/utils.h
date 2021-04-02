#ifndef _UTILS_H
#define _UTILS_H
#include<sys/types.h>
#include<stdbool.h>
#include<dirent.h>
extern void fd_perror(int fd,const char*format,...);
extern int ret_stdout_perror(int err,bool quit,const char*format,...);
extern int ret_perror(int err,bool quit,const char*format,...);
extern int ret_printf(int err,bool quit,int fd,const char*format,...);
#define stdout_perror(s,...)                 fd_perror(STDOUT_FILENO,s)
#define stderr_perror(s,...)                 fd_perror(STDERR_FILENO,s)
#define ret_stderr_perror                    ret_perror
#define return_stderr_perror(err,s,...)      ret_stderr_perror(err,false,s)
#define exit_stderr_perror(err,s,...)        ret_stderr_perror(err,true,s)
#define return_stdout_perror(err,s,...)      ret_stdout_perror(err,false,s)
#define exit_stdout_perror(err,s,...)        ret_stdout_perror(err,true,s)
#define return_perror(err,s,...)             ret_perror(err,false,s)
#define exit_perror(err,s,...)               ret_perror(err,true,s)
#define return_printf(err,fd,format,...)     ret_printf(err,false,fd,format)
#define exit_printf(err,fd,format,...)       ret_printf(err,true,fd,format)
#define return_stdout_printf(err,format,...) ret_printf(err,false,STDOUT_FILENO,format)
#define exit_stdout_printf(err,format,...)   ret_printf(err,true,STDOUT_FILENO,format)
#define return_stderr_printf(err,format,...) ret_printf(err,false,STDERR_FILENO,format)
#define exit_stderr_printf(err,format,...)   ret_printf(err,true,STDERR_FILENO,format)
extern size_t fd_printf(int fd,char*format,...);
extern int close_fd(int fd);
extern bool contains_of(char*source,size_t size,char x);
extern bool check_nvalid(char*source,size_t size_source,char*valid,size_t size_valid);
extern bool check_valid(char*source,char*valid);
extern bool check_nvalid_default(char*source,size_t size);
extern bool check_valid_default(char*source);
extern char**array_add_entry(char**array,char*entry);
extern char*array2args(char**arr,char*d);
extern char**args2array(char*source,char del);
extern char*new_string(size_t size);
extern int write_file(int dir,char*file,const char*str,size_t len,mode_t mode,bool lf,bool create,bool trunc);
extern size_t get_fd_path(int fd,char*buff,size_t size);
extern bool is_invalid_path(char*path);
extern bool is_virt_dir(const struct dirent*d);
extern int char_array_len(char**arr);
extern void free_chars(char**c);
extern int wait_exists(char*path,long time,long step);
extern char dec2hex(int dec,bool upper);
extern char*gen_rand_hex(char*buff,int size,bool upper);
static inline int min(int m1,int m2){return m1>m2?m2:m1;}
static inline int max(int m1,int m2){return m2>m1?m2:m1;}
#define dec2uhex(dec)dec2hex(dec,true)
#define dec2lhex(dec)dec2hex(dec,true)
#define gen_rand_uhex(buff,size)gen_rand_hex(buff,size,true)
#define gen_rand_lhex(buff,size)gen_rand_hex(buff,size,false)
#endif