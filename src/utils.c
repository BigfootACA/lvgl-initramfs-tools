#define _ATFILE_SOURCE
#include<ctype.h>
#include<errno.h>
#include<stdio.h>
#include<fcntl.h>
#include<dirent.h>
#include<stdarg.h>
#include<stdlib.h>
#include<stddef.h>
#include<string.h>
#include<unistd.h>
#include<limits.h>
#include<stdbool.h>
#include<sys/stat.h>
#include<sys/types.h>
#include"utils.h"
#define BUFFER_SIZE 4096
bool is_virt_dir(const struct dirent*d){return strcmp(d->d_name,".")==0||strcmp(d->d_name,"..")==0;}
size_t get_fd_path(int fd,char*buff,size_t size){
	if(fd<0||!buff)return -1;
	memset(buff,0,size);
	char path[64]={0};
	snprintf(path,63,"/proc/fd/%d",fd);
	return readlink(path,buff,size);
}
int write_file(int dir,char*file,const char*str,size_t len,mode_t mode,bool lf,bool create,bool trunc){
	if(!file||!str)return -1;
	int fd,i,e,opt;
	errno=0;
	opt=O_WRONLY;
	if(create)opt|=O_CREAT;
	if(trunc)opt|=O_TRUNC;
	fd=openat(dir<0?AT_FDCWD:dir,file,opt,mode);
	if(fd<0)return fd;
	errno=0;
	i=write(fd,str,len>0?len:strlen(str));
	e=errno;
	if(lf)write(fd,"\n",1);
	close(fd);
	errno=e;
	return i;
}
char*new_string(size_t size){
	char*a=malloc(size);
	if(!a)return NULL;
	memset(a,0,size);
	return a;
}
static void _fd_perror(int fd,const char*format,va_list a){
	char buff[BUFFER_SIZE];
	memset(&buff,0,BUFFER_SIZE);
	if(format)vsnprintf(buff,BUFFER_SIZE-1,format,a);
	dprintf(fd,errno==0?"%s: %m\n":"%s.\n",buff);
}
void fd_perror(int fd,const char*format,...){
	va_list a;
	if(format){
		va_start(a,format);
		_fd_perror(fd,format,a);
		va_end(a);
	}
}
int ret_perror(int err,bool quit,const char*format,...){
	va_list a;
	if(format){
		va_start(a,format);
		_fd_perror(STDERR_FILENO,format,a);
		va_end(a);
	}
	if(quit)exit(err);
	return err;
}
int ret_stdout_perror(int err,bool quit,const char*format,...){
	stdout_perror(format);
	if(quit)exit(err);
	return err;
}
int ret_printf(int err,bool quit,int fd,const char*format,...){
	va_list a;
	char buff[BUFFER_SIZE];
	memset(&buff,0,BUFFER_SIZE);
	va_start(a,format);
	vsnprintf(buff,BUFFER_SIZE-1,format,a);
	va_end(a);
	dprintf(fd,"%s",buff);
	if(quit)exit(err);
	return err;
}
int close_fd(int fd){
	const int fsync_fail=(fsync(fd)!=0);
	const int close_fail=(close(fd)!=0);
	if(fsync_fail||close_fail)return EOF;
	return 0;
}
bool contains_of(char*source,size_t size,char x){
	size_t idx=0;
	char c=0;
	if(!source)return false;
	while(idx<size&&(c=source[idx++])>0)if(c==x)return true;
	return false;
}
bool check_nvalid(char*source,size_t size_source,char*valid,size_t size_valid){
	size_t idx=0;
	char c=0;
	if(!source||!valid)return false;
	while(idx<size_source&&(c=source[idx++])>0)if(!contains_of(valid,size_valid,c))return false;
	return true;
}
bool check_valid(char*source,char*valid){
	return check_nvalid(source,strlen(source),valid,strlen(valid));
}
bool check_nvalid_default(char*source,size_t size){
	return check_nvalid(
		source,
		size,
		"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890_",
		63
	);
}
bool check_valid_default(char*source){
	return check_nvalid_default(source,strlen(source));
}
char*array2args(char**arr,char*d){
	if(!arr||!d)return NULL;
	char*b,*c;
	size_t s=BUFFER_SIZE*sizeof(char),idx=0,len=0,x,dc=strlen(d);
	if(!(b=malloc(s)))return NULL;
	memset(b,0,s);
	while((c=arr[idx++])&&len<s-2){
		if(idx>1){
			strcat(b,d);
			len+=dc;
		}
		x=strlen(c);
		if(x+len>=s-1)break;
		len+=x;
		strcat(b,c);
	}
	c=strdup(b);
	free(b);
	return c;
}
int char_array_len(char**arr){
	int i=-1;
	if(arr)while(arr[++i]);
	return i;
}
void free_chars(char**c){
	if(!c)return;
	size_t l=-1;
	while((c[++l]))if(c[l])free(c[l]);
	free(c);
}
char**args2array(char*source,char del){
	if(!source)return NULL;
	int item=0;
	size_t sz=sizeof(char)*(strlen(source)+1);
	char**array=NULL,*buff=NULL;
	if(!(buff=malloc(sz)))goto fail;
	if(!(array=malloc((sizeof(char*)*(item+2)))))goto fail;
	char*scur=source,*dcur=buff,**arr=NULL,b;
	memset(buff,0,sz);
	array[item]=dcur;
	while(*scur>0)if(del==0?isspace(*scur):del==*scur){
		scur++;
		if(*array[item]){
			dcur++,item++;
			if(!(arr=realloc(array,(sizeof(char*)*(item+2)))))goto fail;
			array=arr;
			array[item]=dcur;
		}
	}else if(*scur=='"'||*scur=='\''){
		b=*scur;
		while(*scur++&&*scur!=b){
			if(*scur=='\\')scur++;
			*dcur++=*scur;
		}
		dcur++,scur++;
	}else *dcur++=*scur++;
	if(!*array[item]){
		array[item]=NULL;
		if(item==0){
			free(buff);
			buff=NULL;
		}
	}
	array[++item]=NULL;
	if(!(arr=malloc((sizeof(char*)*(item+2)))))goto fail;
	memset(arr,0,(sizeof(char*)*(item+2)));
	for(int t=0;array[t];t++)arr[t]=array[t];
	free(array);
	return arr;
	fail:
	if(buff)free(buff);
	if(array)free(array);
	buff=NULL,array=NULL;
	return NULL;
}
static bool is_fail(char*buff,size_t idx){
	buff[idx]=0;
	bool p=true;
	if(idx<=0)p=false;
	else for(size_t k=0;k<idx;k++)if(buff[k]!='.')p=false;
	return p;
}
bool is_invalid_path(char*path){
	if(!path)return false;
	char*buff=NULL,*pb=NULL;
	size_t x=0,l=strlen(path),idx=0,ks=16;
	if(l<=0)return false;
	if(!(buff=malloc(ks)))return false;
	else memset(buff,0,ks);
	bool valid=true;
	while(x<l){
		if(idx>=ks-1){
			ks*=2;
			if((pb=realloc(buff,ks)))buff=pb;
			else{
				free(buff);
				return false;
			}
		}
		switch(path[x]){
			case '\n':case '\r':case '/':case '\\':
				if(is_fail(buff,idx))valid=false;
				idx=-1;
			break;
			default:buff[idx]=path[x];
		}
		x++;idx++;
	}
	if(is_fail(buff,idx))valid=false;
	free(buff);
	return valid;
}
char dec2hex(int dec,bool upper){
	if(dec>=0&&dec<=9)return '0'+dec;
	else if(dec>=10&&dec<=15)return (upper?'A':'a')+dec-10;
	else return 0;
}
char*gen_rand_hex(char*buff,int size,bool upper){
	if(size<0||!buff)return NULL;
	int idx=0;
	while(idx<size)buff[idx++]=dec2hex(rand()%16,upper);
	buff[idx]=0;
	return buff;
}
int wait_exists(char*path,long time,long step){
	long cur=0;
	do{
		if(access(path,F_OK)==0)return 0;
		if(errno!=ENOENT)return -1;
		cur+=step;
		usleep(step*1000);
	}while(cur<time);
	errno=ETIME;
	return -2;
}