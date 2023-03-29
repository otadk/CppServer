#include <stdio.h>

int main() {

    FILE* fp;
    char buffer[1024];

    fp = fopen("FopenDemoFile.txt", "rw");

    fread(buffer, 1024, 1024, fp);
    printf("%s", buffer);
    
    // 可以用fgets替代fread
    // while (fgets(buffer, sizeof(buffer), fp) != NULL) {
    //     printf("%s", buffer);
    // }
    
    fprintf(fp, "This is some text written to the file.\n");

    fclose(fp);

    return 0;
}