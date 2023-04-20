#include "assert.h"
#include "stdio.h"
#include "string.h"
#include "syscall.h"

#define va_start(ap, v) ap = (va_list)&v  // 把ap指向第一个固定参数v
#define va_arg(ap, t) *((t*)(ap += 4))	  // ap指向下一个参数并返回其值
#define va_end(ap) ap = NULL		  // 清除ap

#define READ_BUF_SIZE 1024

static char __stdin_buffer[2048];
static FILE __stdin = {
   .fileno = 0,
   .flags = O_RDONLY,
   .status = 0,
   .read_buf = __stdin_buffer,
   .read_ptr = __stdin_buffer,
   .read_end = __stdin_buffer,
};
FILE *stdin = &__stdin;

static FILE __stdout = {
   .fileno = 1,
   .flags = O_WRONLY,
   .status = 0,
};
FILE *stdout = &__stdout;

static FILE __stderr = {
   .fileno = 2,
   .flags = O_WRONLY,
   .status = 0,
};
FILE *stderr = &__stderr;

/* 将整型转换成字符(integer to ascii) */
static void itoa(uint32_t value, char** buf_ptr_addr, uint8_t base) {
   uint32_t m = value % base;	    // 求模,最先掉下来的是最低位
   uint32_t i = value / base;	    // 取整
   if (i) {			    // 如果倍数不为0则递归调用。
      itoa(i, buf_ptr_addr, base);
   }
   if (m < 10) {      // 如果余数是0~9
      *((*buf_ptr_addr)++) = m + '0';	  // 将数字0~9转换为字符'0'~'9'
   } else {	      // 否则余数是A~F
      *((*buf_ptr_addr)++) = m - 10 + 'A'; // 将数字A~F转换为字符'A'~'F'
   }
}

/* 将参数ap按照格式format输出到字符串str,并返回替换后str长度 */
int vsprintf(char* str, const char* format, va_list ap) {
   char* buf_ptr = str;
   const char* index_ptr = format;
   char index_char = *index_ptr;
   int32_t arg_int;
   char* arg_str;
   while(index_char) {
      if (index_char != '%') {
         *(buf_ptr++) = index_char;
         index_char = *(++index_ptr);
         continue;
      }
      index_char = *(++index_ptr);	 // 得到%后面的字符
      switch(index_char) {
         case 's':
            arg_str = va_arg(ap, char*);
            strcpy(buf_ptr, arg_str);
            buf_ptr += strlen(arg_str);
            index_char = *(++index_ptr);
            break;

         case 'c':
            *(buf_ptr++) = va_arg(ap, char);
            index_char = *(++index_ptr);
            break;

         case 'd':
            arg_int = va_arg(ap, int);
            /* 若是负数, 将其转为正数后,再正数前面输出个负号'-'. */
            if (arg_int < 0) {
               arg_int = 0 - arg_int;
               *buf_ptr++ = '-';
            }
            itoa(arg_int, &buf_ptr, 10);
            index_char = *(++index_ptr);
            break;

         case 'x':
            arg_int = va_arg(ap, int);
            itoa(arg_int, &buf_ptr, 16);
            index_char = *(++index_ptr); // 跳过格式字符并更新index_char
            break;
      }
   }
   return (int) strlen(str);
}

/* 同printf不同的地方就是字符串不是写到终端,而是写到buf中 */
int sprintf(char* buf, const char* format, ...) {
   va_list args;
   int retval;
   va_start(args, format);
   retval = vsprintf(buf, format, args);
   va_end(args);
   return retval;
}

/* 格式化输出字符串format */
int printf(const char* format, ...) {
   va_list args;
   int n;
   va_start(args, format);	       // 使args指向format
   n = vfprintf(stdout, format, args);
   va_end(args);
   return n;
}

FILE *fopen(const char *filename, const char *mode) {
   FILE *fp = (FILE *)malloc(sizeof(FILE));
   if (mode[0] == 'r') {
      fp->flags = O_RDONLY;
   } else if (mode[0] == 'w') {
      fp->flags = O_WRONLY | O_CREAT;
   } else {
      free(fp);
      return NULL;
   }

   fp->fileno = open((char*) filename, fp->flags);
   fp->read_buf = (char*)malloc(READ_BUF_SIZE);
   fp->read_ptr = fp->read_buf;
   fp->read_end = fp->read_buf;
   fp->status = 0;

   return fp;
}

void fclose(FILE *fp) {
   if (!fp || fp->status == -1) {
      return;
   }
   close(fp->fileno);
   free(fp->read_buf);
   fp->status = -1;
   free(fp);
}

static int load_read_buffer(FILE *fp) {
   assert (fp->read_ptr == fp->read_end);

   char *const rdbuf_end = fp->read_buf + READ_BUF_SIZE;
   if (fp->read_end == rdbuf_end) {
      fp->read_ptr = fp->read_buf;
      fp->read_end = fp->read_buf;
   }

   int size = rdbuf_end - fp->read_ptr;
   int r = read(fp->fileno, fp->read_buf, size);
   if (r > 0) {
      fp->read_end = fp->read_ptr + r;
   }
   return r;
}

char *fgets(char *str, int count, FILE *fp) {
   char *out = str, *out_end = str + count;
   if (count <= 0) {
      *str = 0;
      return str;
   }

   while (1) {
      if (fp->read_ptr == fp->read_end) {
         int n = load_read_buffer(fp);
         if (n < 0) {
            *out = 0;
            return NULL;
         } else if (n == 0) {
            break;
         }
      }

      char *in = fp->read_ptr, *end = fp->read_end;
      while (in != end && out != out_end && *in != '\n') {
         *out++ = *in++;
      }
      fp->read_ptr = in;

      if (in != end) {
         if (*in == '\n' && out != out_end) {
            *out++ = '\n';
            ++fp->read_ptr;
         }
         break;
      }

      if (out == out_end) {
         break;
      }
   }
   *out = 0;
   return out != str ? str : NULL;
}

int fputs(const char *str, FILE *fp) {
   int block_size = 512;
   while (*str) {
      const char *p = str, *end = p + block_size;
      while (*p && p != end) {
         ++p;
      }
      write(fp->fileno, str, p - str);
      str = p;
   }
   return 1;
}

int fprintf(FILE *fp, const char* format, ...) {
   va_list args;
   int n;
   va_start(args, format);	       // 使args指向format
   n = vfprintf(fp, format, args);
   va_end(args);
   return n;
}

int vfprintf(FILE *fp, const char *format, va_list ap) {
   char buf[1024] = {0};
   int n = vsprintf(buf, format, ap);
   return (int) write(fp->fileno, buf, n);
}