// Bundt, Josh		CS2140 - Lab 6
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <dirent.h>
#include <getopt.h>
#include <string.h>
#include <ctype.h>
#include <linux/fs.h>
#include <linux/ext2_fs.h>
#include <inttypes.h>

#define BASE_OFFSET 1024

static unsigned int block_size = 0;

/* necessary struct for getopt_long function */
const struct option long_opts[] = {
    {"help", no_argument, 0, 'h'},
    {"verbose", no_argument, 0, 'v'},
    {"input", required_argument, 0, 'i'},
    {"output", required_argument, 0, 'o'},
    {"filesystem", no_argument,  0, 'f'},
    {0, 0, 0, 0}
};

/* these strings will display usage cases for each cmd arg option */
const char *opts_desc[] = {
    ", to display this usage message.",
    ", for verbose output.",
    "=FILE, where FILE is the input file to be read",
    "=FILE, where FILE is the outut file to write to",
    ", to display filesystem information (ext2 only)"
};

/* simple function to print proper usage of the program */
void usage(const char* arg)
{
    int i;
    printf("Usage: %s [OPTIONS] ARGS, where\n", arg);
    printf("  ARGS: filenames that will be used as arguments\n");
    printf("  OPTIONS:\n");
    for (i = 0; i < sizeof(long_opts) / sizeof(struct option) - 1; i++) {
        printf("\t-%c, --%s%s\n",
            long_opts[i].val, long_opts[i].name, opts_desc[i]);
    }    
    exit(EXIT_SUCCESS);
}

// prototypes
void show_stat(const char* path);
void show_dir(const char* path);
void show_fs(const char* path);
void read_from_file(FILE* fd);

// global variables
int verbose = 0;
int filesystem = 0;

int main(int argc, char *argv[])
{
    if (argc < 2) usage(argv[0]);
    int opt, i, j;
    FILE *file_in, *file_out;		/* declare file pointers for use */
    file_in = file_out = NULL;

    while( -1 != (opt = getopt_long(argc, argv, "hvi:o:f", long_opts, &i)) ) {
        switch(opt) {
            case 'h':
                usage(argv[0]);
                break;
            case 'v':
                verbose = 1;
                printf(" --verbose enabled-- \n");
		fflush(stdout);
                break;
            case 'f':
				filesystem = 1;
                break;
            case 'i':
				file_in = fopen(optarg, "r");
				break;
            case 'o':
				file_out = freopen(optarg, "w", stdout);
				break;
            default:
                fprintf(stderr, "Invalid option.\n");
                usage(argv[0]);
                break;
        }
    } // while
    if (file_in) read_from_file(file_in);	/* if input file was specified */
    
    for (j = optind; j < argc; j++) {	/* iterate through argv starting */
		show_stat(argv[j]);		/*  with optind position */
		show_dir(argv[j]);
		if (filesystem) show_fs(argv[j]);	/* check for -f flag */
	}
	
	if (file_in) fclose(file_in);
	if (file_out) fclose(file_out);	/* be sure to close file ptrs */
	return 0;
}

void show_stat(const char* path) {
	struct stat sb;

	if (stat(path, &sb) == -1) {		/* check for stat error */
	   perror("stat");
	   return;
	}
	
	printf("\n Stat  %s\n",path);
	printf("File type:                ");

	switch (sb.st_mode & S_IFMT) {
	case S_IFBLK:  printf("block device\n");            break;
	case S_IFCHR:  printf("character device\n");        break;
	case S_IFDIR:  printf("directory\n");               break;
	case S_IFIFO:  printf("FIFO/pipe\n");               break;
	case S_IFLNK:  printf("symlink\n");                 break;
	case S_IFREG:  printf("regular file\n");            break;
	case S_IFSOCK: printf("socket\n");                  break;
	default:       printf("unknown?\n");                break;
	}

	printf("I-node number:            %ju\n", (uintmax_t) sb.st_ino);

	printf("Mode:                     %lo (octal)\n",
		   (unsigned long) sb.st_mode);

	printf("Link count:               %ld\n", (long) sb.st_nlink);
	printf("Ownership:                UID=%ld   GID=%ld\n",
		   (long) sb.st_uid, (long) sb.st_gid);

	printf("Preferred I/O block size: %ld bytes\n",
		   (long) sb.st_blksize);
	printf("File size:                %ju bytes\n",
		   (uintmax_t) sb.st_size);
	printf("Blocks allocated:         %ju\n",
		   (uintmax_t) sb.st_blocks);

	printf("Last status change:       %s", ctime(&sb.st_ctime));
	printf("Last file access:         %s", ctime(&sb.st_atime));
	printf("Last file modification:   %s", ctime(&sb.st_mtime));
	printf("\n");

}

void show_dir(const char* path) {
	DIR *d_ptr;		/* we neod both a DIR ptr, and a dirent */
	struct dirent *db;	/*  struct ptr to return the dir info */
	
	if ((d_ptr = opendir(path)) == NULL) {	/* check for opendir error */
		if (verbose) perror("opendir");
		return;
	}
		
	printf("Directories under %s\n", path);
	
	/* iterate through list of dirents */
	while ((db = readdir(d_ptr)) != NULL) {	
		printf("Name:  %s\n", db->d_name);
		
		printf("   Inode: %ju\t\tType: ", (uintmax_t) db->d_ino);
		switch(db->d_type) {
			case DT_BLK:	printf("block device\n"); 	break;
			case DT_CHR:	printf("character device\n"); 	break;
			case DT_DIR:	printf("directory\n");		break;
			case DT_FIFO:	printf("named pipe\n");		break;
			case DT_LNK:	printf("symbolic link\n");	break;
			case DT_REG:	printf("regular file\n");	break;
			case DT_SOCK:	printf("Unix domain socket\n");	break;
			default:		printf("unknown\n");	break;
		}
		
		printf("   Record Len: %d\tNext dirent:  %ju\n", db->d_reclen, \
				(uintmax_t) db->d_off);
	}
	
	if (closedir(d_ptr) == -1) {	/* maybe error checking is not */
		perror("closedir");	/*  necessary here, but closedir */
		exit(EXIT_FAILURE);	/*  provides feedback */
	}
}

void show_fs (const char* path) {
	int fd;
	struct ext2_super_block super;	/* this will store all ext2 info */

	fd = open(path, O_RDONLY);	/* open the filesystem read only */
	if (fd < 0) {				/* check for open error */
		if (verbose) perror("open");
		return;
	}

	/* seek to the first known superblock loccation */
	lseek(fd, BASE_OFFSET, SEEK_SET); 
	/* read in the superblock */
	read(fd, &super, sizeof(struct ext2_super_block));


	if(super.s_magic != EXT2_SUPER_MAGIC){		/* check for ext2 */
		if (verbose) fprintf(stderr, "Not an Ext2 filesystem!\n");
		return;
	}

	block_size = BASE_OFFSET << super.s_log_block_size; /* bit shift left */

	printf("\n EXT2 filesystem at %s\n", path);
	printf("Magic:            0x%x\n", super.s_magic);
	printf("Total inodes:     %ld\n", (long) super.s_inodes_count);
	printf("Free inodes:      %ld\n", (long) super.s_free_inodes_count);
	printf("Blocks:           %ld\n", (long) super.s_blocks_count);
	printf("Free blocks:      %ld\n", (long) super.s_free_blocks_count);
	printf("First block:      %ld\n", (long) super.s_first_data_block);
	printf("Block size:       %ld\n", (long) block_size);
	printf("Mount count:      %ld\n", (long) super.s_mnt_count);
	printf("Mount time:       %s", ctime((const time_t* ) &super.s_mtime));
	printf("Write time:       %s", ctime((const time_t* ) &super.s_wtime));
	
	close(fd);
}

void read_from_file(FILE* fd) {
	char buf[FILENAME_MAX], c;		/* buf is our input buffer */
	int i;
	
	/* read in each line from file, fgets returns NULL when done */
	while (fgets(buf, FILENAME_MAX, fd)) { 
		i = 0;
		c = buf[i];
		while(c != '\0') {	/* check for terminating character */
			/* check for # or terminating char, 
			 * fill remaining buffer with 0's */
			if (c== '#' || isspace(c)) bzero(&buf[i], FILENAME_MAX-i);
			i++;			/* count suspected good input */
			c = buf[i];
		}
		if (buf[0] == '\0') continue;	/* no good input found */
		if (verbose) fprintf(stderr, "file arg: %s\n",buf);
		/* call show_stat, show_dir and show_fs from here as required,
		 * this will save having to pass muliple buffers back to main,
		 * terminate when done. */
		show_stat(buf);
		show_dir(buf);
		if (filesystem) show_fs(buf);
		bzero(&buf, FILENAME_MAX);	/* reset our buffer to all zero's */
	}
	fclose(fd);
	exit(EXIT_SUCCESS);
}
