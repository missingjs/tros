#include "device/console.h"
#include "device/ioqueue.h"
#include "device/tty.h"
#include "fs/file.h"
#include "kernel/memory.h"
#include "thread/sync.h"

#define N_TTY_LINE_BUF_SIZE 1024

struct n_tty_data {
    struct ioqueue buffer;
    unsigned char line_buf[N_TTY_LINE_BUF_SIZE];
    int line_size;
};

static void n_tty_data_init(struct n_tty_data *data) {
    ioqueue_init(&data->buffer);
}

static int n_tty_open(struct tty_struct *tty) {
    struct n_tty_data *tdata = (struct n_tty_data *)kmalloc(sizeof(struct n_tty_data));
    n_tty_data_init(tdata);
    tdata->line_size = 0;
    tty->disc_data = tdata;
    return 0;
}

static void n_tty_close(struct tty_struct *tty) {
    struct n_tty_data *tdata = (struct n_tty_data *)tty->disc_data;
    ioq_close(&tdata->buffer);
    kfree(tdata);
    tty->disc_data = NULL;
}

static int32_t n_tty_read(struct tty_struct *tty, struct file *file UNUSED, unsigned char *buf, uint32_t nr) {
    struct n_tty_data *tdata = (struct n_tty_data *)tty->disc_data;
    int r = ioq_read_some(&tdata->buffer, (char *)buf, nr);
    return r;
}

static int32_t n_tty_write(struct tty_struct *tty UNUSED, struct file *file UNUSED, const unsigned char *buf, uint32_t nr) {
    console_put_str_n((const char *)buf, nr);
    return (int32_t) nr;
}

static void n_tty_receive_buf(struct tty_struct *tty, const unsigned char *buf, uint32_t count) {
    const unsigned char *p = buf, *end = buf + count;
    struct n_tty_data *tdata = (struct n_tty_data *)tty->disc_data;
    while (p != end) {
        unsigned char ch = *p++;
        switch (ch) {
            case '\b':
                --tdata->line_size;
                break;
            case '\r':
                ch = '\n';
                __attribute__ ((fallthrough));
            default:
                tdata->line_buf[tdata->line_size] = ch;
                ++tdata->line_size;
                break;
        }

        // ECHO to output
        console_put_char((uint8_t) ch);

        if (tdata->line_size == N_TTY_LINE_BUF_SIZE || ch == '\n') {
            // copy line content to buffer
            int s = ioq_write_some(&tdata->buffer, (const char *)tdata->line_buf, tdata->line_size);
            int len = tdata->line_size - s;
            unsigned char *src = &tdata->line_buf[s], *dst = &tdata->line_buf[0], *src_end = src + len;
            while (src != src_end) {
                *dst++ = *src++;
            }
            tdata->line_size = len;
        }
    }
}

static struct tty_ldisc_ops n_tty_ops = {
    .name         = "n_tty",
    .open         = n_tty_open,
    .close        = n_tty_close,
    .read         = n_tty_read,
    .write        = n_tty_write,
    .receive_buf  = n_tty_receive_buf,
};

static void init_tty_struct(struct tty_struct *tty) {
    struct tty_ldisc *ldisc = (struct tty_ldisc *)kmalloc(sizeof(struct tty_ldisc));
    tty->ldisc = ldisc;
    ldisc->ops = &n_tty_ops;
    ldisc->tty = tty;
    ldisc->ops->open(tty);
}

static int32_t tty_read(struct file *filp, char *buf, uint32_t count) {
    struct tty_struct *tty = (struct tty_struct *)filp->private_data;
    return tty->ldisc->ops->read(tty, filp, (unsigned char *)buf, count);
}

static int32_t tty_write(struct file *filp, const char *buf, uint32_t count) {
    struct tty_struct *tty = (struct tty_struct *)filp->private_data;
    return tty->ldisc->ops->write(tty, filp, (const unsigned char *)buf, count);
}

static struct tty_struct *current_tty;

static struct file_operations tty_input_ops = {
   .llseek  = no_llseek_fn,
   .read    = tty_read,
   .write   = no_write_fn,
   .release = no_release_fn,
};

static struct file_operations tty_output_ops = {
   .llseek  = no_llseek_fn,
   .read    = no_read_fn,
   .write   = tty_write,
   .release = no_release_fn,
};

void tty_init(void) {
    struct tty_struct *tty = (struct tty_struct *)kmalloc(sizeof(struct tty_struct));
    init_tty_struct(tty);
    current_tty = tty;

    struct file *stdin = &file_table[0];
    init_file_struct(stdin);
    atomic_inc(&stdin->count);
    stdin->op = &tty_input_ops;
    stdin->fd_flag = O_RDONLY;
    stdin->private_data = current_tty;

    struct file *stdout = &file_table[1];
    init_file_struct(stdout);
    atomic_inc(&stdout->count);
    stdout->op = &tty_output_ops;
    stdout->fd_flag = O_WRONLY;
    stdout->private_data = current_tty;

    struct file *stderr = &file_table[2];
    init_file_struct(stderr);
    atomic_inc(&stderr->count);
    stderr->op = &tty_output_ops;
    stderr->fd_flag = O_WRONLY;
    stderr->private_data = current_tty;
}

void tty_data_arrived(const unsigned char *buf, uint32_t count) {
    current_tty->ldisc->ops->receive_buf(current_tty, buf, count);
}