####  此脚本应该在command目录下执行

# if [[ ! -d "../lib" || ! -d "../build" ]];then
#    echo "dependent dir don\`t exist!"
#    cwd=$(pwd)
#    cwd=${cwd##*/}
#    cwd=${cwd%/}
#    if [[ $cwd != "command" ]];then
#       echo -e "you\`d better in command dir\n"
#    fi 
#    exit
# fi

self_dir=$(cd $(dirname $0) && pwd)
proj_dir=$(realpath $self_dir/..)

cd $self_dir

BIN="prog_no_arg"
CFLAGS="-m32 -Wall -c -fno-pic -fno-stack-protector -fno-builtin -W -Wstrict-prototypes \
      -Wmissing-prototypes -Wsystem-headers"
# LIB="../lib/"
OBJS="../build/lib/string.o \
      ../build/lib/stdio.o ../build/lib/user/assert.o ../build/kernel/debug.o ../build/lib/kernel/print.o ../build/kernel/interrupt.o ../build/lib/user/syscall.o ../build/kernel/kernel.o ../build/userprog/syscall-init.o"
DD_IN=$BIN
DD_OUT="../hd60M.img" 

gcc $CFLAGS -I $proj_dir/include -I $proj_dir/include/lib -o ${BIN}.o ${BIN}.c
ld -m elf_i386 -e main ${BIN}.o $OBJS -o $BIN
SEC_CNT=$(ls -l $BIN|awk '{printf("%d", ($5+511)/512)}')

if [[ -f $BIN ]];then
   dd if=./$DD_IN of=$DD_OUT bs=512 \
   count=$SEC_CNT seek=300 conv=notrunc
fi

##########   以上核心就是下面这三条命令   ##########
#gcc -Wall -c -fno-builtin -W -Wstrict-prototypes -Wmissing-prototypes \
#   -Wsystem-headers -I ../lib -o prog_no_arg.o prog_no_arg.c
#ld -e main prog_no_arg.o ../build/string.o ../build/syscall.o\
#   ../build/stdio.o ../build/assert.o -o prog_no_arg
#dd if=prog_no_arg of=/home/work/my_workspace/bochs/hd60M.img \
#   bs=512 count=10 seek=300 conv=notrunc
