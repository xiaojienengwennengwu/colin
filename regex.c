#include<sys/mman.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<regex.h>

int main()
{
    int fd = open("url.html", O_RDONLY);
    int fsize = lseek(fd, 0, SEEK_END);
    char* mmap_ptr = mmap(NULL, fsize, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);

    // REG_NEWLINE关闭换行限制，BRE正则，捕获href、标题
    char* regstr = "<a[^>]*href=\"\\([^\"]*\\)\"[^>]*>\\([^<]*\\)</a>";
    regex_t reg;
    int regnum = 3;
    regmatch_t match[regnum];
    regcomp(&reg, regstr, REG_NEWLINE);

    char title[1024];
    char url[1024];

    while (regexec(&reg, mmap_ptr, regnum, match, 0) == 0)
    {
        bzero(title, sizeof(title));
        bzero(url, sizeof(url));

        snprintf(url, match[1].rm_eo - match[1].rm_so + 1, "%s", mmap_ptr + match[1].rm_so);
        snprintf(title, match[2].rm_eo - match[2].rm_so + 1, "%s", mmap_ptr + match[2].rm_so);

        printf("url:%s | title:%s\n", url, title);

        mmap_ptr += match[0].rm_eo;
    }

    regfree(&reg);
    munmap(mmap_ptr - match[0].rm_eo, fsize);
    printf("done.\n");
    return 0;
}