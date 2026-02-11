#include <time.h>
#include <dirent.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>

#define WORD_TO_FIND "Background"
#define DIRECTORY_PATH "Backgrounds"

int FILE_OFFSET = 0;
int OLD_PATH = 0;

void print_images(char **images, int size){
	
	for (int i = 0; i < size; i++){

		puts(images[i]);
	}

}


void free_images(char **images, int size){
	
	for (int i = 0; i < size; i++){

		free(images[i]);
	}

	free(images);
}

char **get_image_list(const char *dir_path, int *size) {

    DIR *dir = opendir(dir_path);
    if (dir == NULL){

		puts("The directory is absent");
    	return NULL;	
    } 
    
    // count images
    struct dirent *entry;
   

    while ((entry = readdir(dir)) != NULL) {
        
        const char *ext = strrchr(entry->d_name, '.');

        if (ext && strcmp(ext, ".png") == 0)  {
            (*size)++;
        }
    }
    
    rewinddir(dir);
    
    // Allocate array
    char **images = malloc(sizeof(char *) * (*size));
    
    if (images == NULL) {

    	puts("Alloc issue");
        closedir(dir);
        return NULL;
    }
    
    //store names
    int i = 0;

	while ((entry = readdir(dir)) != NULL) {
	        
	        const char *ext = strrchr(entry->d_name, '.');
	
	        if (ext && strcmp(ext, ".png") == 0) {

	            images[i] = strdup(entry->d_name);
	            i++;
	        }
	    }
	    
    
    closedir(dir);
    return images;
}


int parser(char *line){

	char *s = line;

	//no opti
	int end = strlen(WORD_TO_FIND);
	int i = 0;
	
	
	while(*s != '\0' ){


		if (i == end) return i+2;
		
		i = *s == WORD_TO_FIND[i] ? i + 1 : 0;
			
//		putchar(*s);
		FILE_OFFSET += 1;
		s++;	
	}
	

//	printf("i=[%d] for the line [%s]\n",i,line);
//	putchar('\n');

	return 0;
}

char *get_comments(char *line){

	while(*line != '"'){

		if ( *line == '\0' || *line == '\n'){
			puts("BAD WRITTEN OF THE PATH");
			return NULL;
		}
		
		line++;
	}
	
	size_t com_len = strlen(line);
	char *comments = malloc(sizeof(char) * (com_len + 1));
	const char *source = line;
	
	strncpy(comments,source,com_len);
	
	comments[com_len] = '\0';
	
//	printf("start=%c\nend=%c\n",*start,*line);

	return comments;
}


char *get_first_pt(char* line,int offset){

	char *first_pt = malloc(sizeof(char) * (offset + 1));
	
	const char *src = line;
	
	strncpy(first_pt,src,offset);

	first_pt[offset] = '\0';
	
	return first_pt;
}

char *build_str(char *fst_part, char *image_path, char *comments) {
   
    size_t total_len = strlen(fst_part) + strlen(image_path) + strlen(comments) + 1;
    
   
    char *result = malloc(total_len);
    if (result == NULL) return NULL;
    
    strcpy(result, fst_part);
    strcat(result, image_path);
    strcat(result, comments);

   // result[total_len] = '\n';
    
    return result;
}

char *random_image(char **images,int mod){

	srand(time(NULL));

	int i = rand() % mod;

	return images[i];
}

FILE *create_tmp_file(char *tmp_file_path){
	
	FILE *file = fopen(tmp_file_path, "w+");

	return file;	
}


int main(int argc, char **argv){

	if (argc != 2){
		puts("Needs only the path");
		return 1;
	}
	
	(void)argc;
	
	FILE *tmp_file = create_tmp_file("test.txt"); // /tmp/tmp.txt
	 
	const char *path = argv[1];
	
	FILE *file = fopen(path, "r+");

	char line[256];

	 while (fgets(line, sizeof(line), file)) {

			int line_offset = parser(line);

			if (line_offset == 0){

				fputs(line, tmp_file);
				
				continue;
			}

			//Set cursor to the start of line where is the work
			FILE_OFFSET -= line_offset - 2;
			
			char *fst_part = get_first_pt(line,line_offset);

			int size = 0;
			char **images = get_image_list(DIRECTORY_PATH, &size);				

			char *new_image = random_image(images,size);
			//print_images(images, size);
			
			char *comments = get_comments(line + line_offset);

			char *new_line = build_str(fst_part,new_image,comments);

			printf("Build str = %s",new_line);
							
			//fseek(file, FILE_OFFSET , SEEK_SET);
							
			//fwrite(new_line,sizeof(char),strlen(new_line),file);

			fputs(new_line, tmp_file);

			free_images(images,size);
			free(new_line);
	 	
	      //printf("%s", line); 
	  }

	fclose(file);
	fclose(tmp_file);
	
	
	return 0;
}
