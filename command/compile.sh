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

BIN="cat"
CFLAGS="-m32 -Wall -c -fno-pic -fno-stack-protector -fno-builtin -W -Wstrict-prototypes \
      -Wmissing-prototypes -Wsystem-headers"
OBJS="../build/lib/stdio.o ../build/lib/string.o ../build/lib/user/assert.o ../build/lib/user/syscall.o start.o"
DD_IN=$BIN
DD_OUT="../hd60M.img" 

nasm -f elf32 ./start.S -o ./start.o
ar rcs simple_crt.a $OBJS start.o
gcc $CFLAGS -I $proj_dir/include -I $proj_dir/include/lib -o ${BIN}.o ${BIN}.c
ld -m elf_i386 ${BIN}.o simple_crt.a -o $BIN
SEC_CNT=$(ls -l $BIN|awk '{printf("%d", ($5+511)/512)}')

if [[ -f $BIN ]];then
    set -x
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
