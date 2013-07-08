#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
//compile: gcc main.c -Wall -Wextra -O2 -s -o jpgs
int main(int argc,char ** argv)
{
	if(argc!=2){
		puts("Usage jpgs filename");
		return 1;
	}
	FILE * fp=fopen(argv[1],"rb");
	if(!fp){
		perror("Error opening file the error was:");
		return 1;
	}
	uint32_t found=0;
	while(1){
		uint8_t marker[4];
		if(fread(marker,1,4,fp)!=4){
			perror("Error looking for jpeg start or end of file");
			printf("Read %ld bytes\n",ftell(fp));
			fclose(fp);
			return 1;
		}
		if(marker[0]==0xFF&&marker[1]==0xD8&&marker[2]==0xFF&&marker[3]==0xE0){
			//puts("Found 0xFFD8FFE0");
			uint8_t jpegunk[2];//not sure what that is but lets save it
			if(fread(jpegunk,1,2,fp)!=2){
				perror("Error reading 2 bytes");
				fclose(fp);
				return 1;
			}
			if(fread(marker,1,4,fp)!=4){
				perror("Error looking for JFIF");
				fclose(fp);
				return 1;
			}
			if(marker[0]=='J'&&marker[1]=='F'&&marker[2]=='I'&&marker[3]=='F'){
				FILE * out;
				{char temp[1024];
				sprintf(temp,"%d.jpg",found);
				printf("Found jpeg at offset 0x%lX saving as %d.jpg\n",ftell(fp)-0xA,found++);//add 1 to found for next file
				out=fopen(temp,"wb");}
				fputc(0xFF,out);
				fputc(0xD8,out);
				fputc(0xFF,out);
				fputc(0xE0,out);
				if(fwrite(jpegunk,1,2,out)!=2){
					perror("Error writting 2 bytes the error was: ");
					fclose(out);
					fclose(fp);
					return 1;
				}
				fputc('J',out);
				fputc('F',out);
				fputc('I',out);
				fputc('F',out);
				//now write rest of file
				while(1){
					uint8_t dat=fgetc(fp);
					fputc(dat,out);
					if(dat==0xFF){
						uint8_t test=fgetc(fp);
						fputc(test,out);
						if(test==0xD9)
							break;
					}
				}
				fclose(out);
			}
		}
	}
	return 0;
}
