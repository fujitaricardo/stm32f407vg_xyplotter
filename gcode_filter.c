/**
 *  filtro para converter arquivo de perfuração .drill em txt interpretavel pelo arm
 */
 
#include <stdio.h>
#include <stdlib.h>

void filtra_xy(FILE *f, FILE *newf);
int filtra_unit(FILE *f);
float *cria_arr_xy(int lines, FILE *f, char ch[]);

void main(){
   FILE *gcode_ori, *gcode_filt_w, *gcode_filt_r;
   int unit, tempo, line = 118;
   char coord_xy_str[2];
   float *coord_xy_float;

   gcode_ori = fopen("singlesidedmm.top.drill.tap" , "r");
   gcode_filt_w = fopen("gcode_filt.txt", "w+");
   gcode_filt_r = fopen("gcode_filt.txt", "r");
   unit = filtra_unit(gcode_ori);
   filtra_xy(gcode_ori, gcode_filt_w);
   fclose(gcode_ori);
   fclose(gcode_filt_w);
   coord_xy_float = cria_arr_xy(line, gcode_filt_r, coord_xy_str);
   free(coord_xy_float);
   fclose(gcode_filt_r);
}

/*
void filtra_xy(FILE *f, FILE *newf){} //le o arquivo .drill,
   int c;                            //filtra as coordenadas do comando G82 e as escreve em um .txt
   while(!feof(f)){
      c = fgetc(f);
      if(c == 'G'){
         c = fgetc(f);
         if(c == '8'){
            c = fgetc(f);
            if(c == '2'){
               while(c != 'X'){
                  c = fgetc(f);
               }
               if(c == 'X'){
                  c = fgetc(f);
                  while(c != 'Y'){
                     fprintf(newf, "%c", c);
                     c = fgetc(f);
                  }
                  fprintf(newf, "\n");
               }
               if(c == 'Y'){
                  while(c != ' '){
                     c = fgetc(f);
                     fprintf(newf, "%c", c);
                  }
                  fprintf(newf, "\n");
               }
            }
         }    
      }
   }
}
*/

void filtra_xy(FILE *f, FILE *newf){ //le o arquivo .drill, 
   int c;                            //filtra as coordenadas do comando G82 e as escreve em um .txt
   while(!feof(f)){
      c = fgetc(f);
      if(c == 'G'){
         c = fgetc(f);
         if(c == '8'){
            c = fgetc(f);
            if(c == '2'){
               while(c != 'X'){
                  c = fgetc(f);
               }
               if(c == 'X'){
                  c = fgetc(f);
                  while(c != 'Y'){
                     fprintf(newf, "%c", c);
                     c = fgetc(f);
                     if(c == ' '){
                        fprintf(newf, "@");
                        c = fgetc(f);
                        if(c == ' '){
                           c = fgetc(f);
                        }      
                     }
                  }
               }
               if(c == 'Y'){
                  c = fgetc(f);
                  while(c != ' '){
                     fprintf(newf, "%c", c);
                     c = fgetc(f);
                  }
                  fprintf(newf, "@");
               }
            }
         }    
      }
   }
   fprintf(newf, "@");
}

int filtra_unit(FILE *f){ //filtra a unidade (mm ou pol) do arquivo .drill
   int c;
   while(!feof(f)){
      c = fgetc(f);
      if(c == 'G'){
         c = fgetc(f);
         if(c == '2'){
            c = fgetc(f);
            if(c == '0'){
               return 0; //polegadas
            }
            else if(c == '1'){
               return 1;  //milimetros
            }
         }  
      }   
   }
}

float *cria_arr_xy(int lines, FILE *f, char ch[]){ //armazena as coordenadas X's e Y's
   float *val;                                     //num float array e o escreve na flash do arm
   int i;
   val = (float *)malloc(lines * sizeof(int));
   for(i=0;lines--;i++){
      fscanf(f, "%s", ch);
      val[i] = atof(ch);
   }
   return val;
}
