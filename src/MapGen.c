#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//Constants and globals - future plans to minimize these as much as possible
#define FPS 60
#define FRAME_TARGET_TIME (1000/FPS)

#define BORDER_Y 10
#define BORDER_X 10
#define MAX_Y 600
#define MAX_X 1280

#define MIN_CONTINENTS 100
#define MAX_CONTINENTS 200

#define W -1 //Water
#define W2 -2 //Secondary Water layer; used to plump up rivers & non-detectable water layer
#define L1 -3 //Land Layer 1
#define L2 -4 //Land Layer 2

//anything that isn't modifiable land is stored as a negative int

//J is largest land val, P is smallest value
#define J 500 //Jungle
#define G 400 //Savanna / Grassland
#define D 300 //Desert
#define F 200 //Forest
#define T 100 //Tundra
#define P 0 //Polar: Snow and Ice


//_______________________
//Utility functions
int neg_pos_gen();
int skewed_neg_pos_gen(int skew_val);
//_______________________

//_______________________
//Map generation function
void gen_stretched_hexagon(int water_or_land, int distortion, int* array, int row, int col);
void gen_circle(int water_or_land, int max_size, int distortion, int hard_code_distortion, int render_direction, int* array, int row, int col);
void gen_ocean_split(int* array, int row, int col);
void gen_ocean_errosion(int* array, int row, int col);
void gen_rivers(int* array, int row, int col);

//  -Generating climate values mapped to colours for biomes
int gen_biomes(int* array, int row, int col);
//  -Colour filling biomes
void regular_colour_fill(int *renderer, int* array, int row, int col);
void blended_colour_fill(int *renderer, int* array, int row, int col);
//_______________________

//_______________________
//Main functions
void map_constructor(int *renderer, int* array, int row, int col);
int gameLoop();
int main();
//_______________________

//_______________________
//Unused functions; possible future use?
int major_climate_shift(int direction, int climate_val);
int oceanic_climate_shift(int direction, int climate_val, int* array, int row, int col);
int minor_climate_shift(int* array, int row, int col);
//_______________________

int neg_pos_gen(){// generates a -1 or 1
    int num = ((rand() % 2) - 1);
    if(num == 0){ num++;}
    return num;
}

int major_climate_shift(int direction, int climate_val){
//UNUSED IN V.1

    if(direction == -1){
        switch(climate_val){
            case J :
                climate_val = G;
                break;
            case G :
                climate_val = D;
                break;
            case D :
                climate_val = F;
                break;
            case F :
                climate_val = T;
                break;
            case T :
                climate_val = P;
                break;
            default: //polar cannot shift down or get colder
                break;
        }
    }

    else if(direction == 1){
        switch(climate_val){
            case P :
                climate_val = T;
                break;
            case T :
                climate_val = F;
                break;
            case F :
                climate_val = D;
                break;
            case D :
                climate_val = G;
                break;
            case G :
                climate_val = J;
                break;
            default: //jungle cannot shift up or get warmer
                break;
        }
    }

    return climate_val;
}

int oceanic_climate_shift(int direction, int climate_val, int* array, int row, int col){
//UNUSED IN V.1

    //These following blocks add in climate calculations based on theoretical ocean currents (ontop of latitude calculations)
    //_____________________________________________________________________________________________________________________________________

    //GEOGRAPHICAL/METEOROLOGICAL BACKGROUND: Northern currents flow clockwise and carry cold from the north and warm from the equator
    //                                        Southern currents flow counter-clockwise and carry cold from the south and warm from the equator
    //                                        There is one counter current on the equator that goes Left to Right

    //cool currents cause a shift in the zones towards cooler climates, and the formation of them near the coasts

    //e.g  jungle -> grassland or grassland -> desert, also the zone has jungle, the width in this area for jungle will decrease  
    


    
    //this iterator finds left lying coast
    int biome_shift;
    int depth = 1; 
    for(int x = BORDER_X; x < (MAX_X - BORDER_X); x++){
        for(int y = BORDER_Y; y < (MAX_Y - BORDER_Y); y++){
            if(array[(y*col) + x] != W && array[(y*col) + (x-1)] == W ){
                if (array[(y*col) + (x+1)] != W && array[((y+1)*col) + (x+1)] != W && array[((y+1)*col) + x] != W && array[((y-1)*col) + x] != W && array[((y-1)*col) + (x+1)] != W){
                    for(int i = 0; i <= depth; i++){
                        if (array[(y*col) + (x+1+i)] != W && array[((y+1)*col) + (x+1+i)] != W && array[((y+1)*col) + (x+i)] != W && array[((y-1)*col) + (x+i)] != W && array[((y-1)*col) + (x+1+i)] != W){
                            if(i == depth){
                                for(int j = 0; j < ((rand() % 3) + 1 ); j++){
                                    if(array[(y*col) + (x+i+j)] != W && array[(y*col) + (x+i+j)] != W2 && array[(y*col) + (x+i+j)] != L1){array[(y*col) + (x+i+j)] -= biome_shift;}    
                                }
                            }

                            if(array[(y*col) + (x+i+1)] != W2 && array[(y*col) + (x+i+1)] != L1){
                                biome_shift = 1;
                                array[(y*col) + (x+i+1)] -= biome_shift;
                            }

                            if( array[((y+1)*col) + (x+i)] != W2 && array[((y+1)*col) + (x+i)] != L1){array[((y+1)*col) + (x+i)] -= biome_shift;}
                            if( array[((y+1)*col) + (x+i+1)] != W2 && array[((y+1)*col) + (x+i+1)] != L1){array[((y+1)*col) + (x+i+1)] -= biome_shift;}
                            
                            if( array[((y-1)*col) + (x+i+1)] != W2 && array[((y-1)*col) + (x+i+1)] != L1){array[((y-1)*col) + (x+i+1)] -= biome_shift;}
                            if( array[((y-1)*col) + (x+i)] != W2 && array[((y-1)*col) + (x+i)] != L1){ array[((y-1)*col) + (x+i)] -= biome_shift;}
                        }   
                    }
                }
            }
        }
        depth += (rand() % 3) - 1;
        if(depth < 0){depth = 0;}
    }

    //warm currents cause a shift in the zones towards warmer climates, and the formation of them near the coasts
    //e.g opposite effect as what's stated above
    for(int x = (MAX_X - BORDER_X); x > BORDER_X; x--){
        for(int y = BORDER_Y; y < (MAX_Y - BORDER_Y); y++){
            if(array[(y*col) + x] != W && array[(y*col) + (x-1)] == W ){
                if (array[(y*col) + (x+1)] != W && array[((y+1)*col) + (x+1)] != W && array[((y+1)*col) + x] != W && array[((y-1)*col) + x] != W && array[((y-1)*col) + (x+1)] != W){
                    for(int i = 0; i <= depth; i++){
                        if (array[(y*col) + (x+1+i)] != W && array[((y+1)*col) + (x+1+i)] != W && array[((y+1)*col) + (x+i)] != W && array[((y-1)*col) + (x+i)] != W && array[((y-1)*col) + (x+1+i)] != W){
                            if(i == depth){
                                for(int j = 0; j < ((rand() % 3) + 1 ); j++){
                                    if(array[(y*col) + (x+i+j)] != W && array[(y*col) + (x+i+j)] != W2 && array[(y*col) + (x+i+j)] != L1){array[(y*col) + (x+i+j)] += biome_shift;}    
                                }
                            }

                            if(array[(y*col) + (x+i+1)] != W2 && array[(y*col) + (x+i+1)] != L1){
                                biome_shift = 1;
                                array[(y*col) + (x+i+1)] += biome_shift;
                            }

                            if( array[((y+1)*col) + (x+i)] != W2 && array[((y+1)*col) + (x+i)] != L1){array[((y+1)*col) + (x+i)] += biome_shift;}
                            if( array[((y+1)*col) + (x+i+1)] != W2 && array[((y+1)*col) + (x+i+1)] != L1){array[((y+1)*col) + (x+i+1)] += biome_shift;}
                            
                            if( array[((y-1)*col) + (x+i+1)] != W2 && array[((y-1)*col) + (x+i+1)] != L1){array[((y-1)*col) + (x+i+1)] += biome_shift;}
                            if( array[((y-1)*col) + (x+i)] != W2 && array[((y-1)*col) + (x+i)] != L1){ array[((y-1)*col) + (x+i)] += biome_shift;}
                        }   
                    }
                }
            }
        }
        depth += (rand() % 3) - 1;
        if(depth < 0){depth = 0;}
    }

}

int minor_climate_shift(int* array, int row, int col){
//UNUSED IN V.1
    for(int x = BORDER_X; x < (MAX_X - BORDER_X); x++){
        for(int y = BORDER_Y; y < (MAX_Y - BORDER_Y); y++){
            if(array[(y*col) + (x-1)] == W && array[(y*col) + x] != W && array[(y*col) + (x+1)] != W){
                for(int i = 0; i < 10; i++){
                    if ( !(array[(y*col) + (x+i)] < 10) ){array[(y*col) + (x+i)] += 11;}   
                }
            }
        }
    }    

    /*
    for(int x = (MAX_X - BORDER_X); x > BORDER_X; x--){
        for(int y = BORDER_Y; y < (MAX_Y - BORDER_Y); y++){
            if(array[(y*col) + (x-1)] != W && array[(y*col) + x] != W && array[(y*col) + (x+1)] == W){
                for(int i = 0; i < 10; i++){
                    if( !(array[(y*col) + (x-i)] < 10) ){array[(y*col) + (x-i)] = 400;} //cannot be less than zero otherwise the climate shift will replace land with water    
                }        
            }
        }
    }  
    */
}
    
int skewed_neg_pos_gen(int skew_val){
//Generates a -1 or 1 depending on skew val.
//Skew_val over 6 six has a negative inclination
//Skew_val under 6 six has a positive inclination

    /*
     skew_val    0  1  2  3  4  5  6  7  8  9 10 11 <- rand_val
           |
        0  V   | 1| 1| 1| 1| 1| 1| 1| 1| 1| 1| 1| 1|
        1      |-1| 1| 1| 1| 1| 1| 1| 1| 1| 1| 1| 1| 
        2      |-1|-1| 1| 1| 1| 1| 1| 1| 1| 1| 1| 1|
        3      |-1|-1|-1| 1| 1| 1| 1| 1| 1| 1| 1| 1|
        4      |-1|-1|-1|-1| 1| 1| 1| 1| 1| 1| 1| 1|
        5      |-1|-1|-1|-1|-1| 1| 1| 1| 1| 1| 1| 1|
        6      |-1|-1|-1|-1|-1|-1| 1| 1| 1| 1| 1| 1| //equal probability
        7      |-1|-1|-1|-1|-1|-1|-1| 1| 1| 1| 1| 1|
        8      |-1|-1|-1|-1|-1|-1|-1|-1| 1| 1| 1| 1|
        9      |-1|-1|-1|-1|-1|-1|-1|-1|-1| 1| 1| 1|
        10     |-1|-1|-1|-1|-1|-1|-1|-1|-1|-1| 1| 1| 
        11     |-1|-1|-1|-1|-1|-1|-1|-1|-1|-1|-1| 1|
        12     |-1|-1|-1|-1|-1|-1|-1|-1|-1|-1|-1|-1|

    */
    
    int rand_val = (rand() % 12);//num inclusively between 0 and 11

    if((rand_val) >= skew_val){ return 1;}
    else{ return -1;}
}

void gen_stretched_hexagon(int water_or_land, int distortion, int* array, int row, int col){//generates a stretched out hexagon
    
    //a value of 80 is a good distortion value
    //variables for a stretched hexagon

    int dstrt = 100 - distortion;
    if(dstrt < 1){dstrt = 0;}
    

    // 1 = land     0 = water
    int pixel = W;
    int not_pixel = L1;
    if(water_or_land == 1){
        pixel = L1;
        not_pixel = W;
    }

    int x, y; 

    int x_min;
    int x_max;        
    int y_min;
    int y_max;

    int max_line_size;

    int rigidity = 2; //determines how squarish a map will be (0 = always slanted, huge values mean extreme squareness, e.g. 500 = square-like continents)
    
    //skew will determine how far left or right a continent border will slant (between 0 - 12 | 0 is left/down, 12 = right/up, 6 is equal slant)
    int skew1;
    int skew2;
    int skew3;
    int skew4;

    int u_len_line; //upper edges
    int u_border_y;
    int u_border_x;

    int l_len_line; //lower edges
    int l_border_y;
    int l_border_x;    

    int f_line_found;

    int left_u_tri_skew; //upper triangle skew
    int right_u_tri_skew;

    int left_l_tri_skew; //lower triangle skew
    int right_l_tri_skew;

    int y_len; //an alternative if the lower triangle overflows
        
    skew1 = (rand() % 13);
    skew2 = (rand() % 13);
    skew3 = (rand() % 13);
    skew4 = (rand() % 13);

    //These do whiles set up the bounds of the continents

    y_max = (BORDER_Y) + (rand() % (MAX_Y - (2 * BORDER_Y) - 30) ) + 1 ; //The 30 is a buffer

    do{
        y_min = (BORDER_Y) + (rand() % (MAX_Y - (2 * BORDER_Y)) ) ;
    }while(y_min > y_max);

    if ( (y_min - BORDER_Y) > ((MAX_Y - y_max) - BORDER_Y) ){max_line_size = (MAX_Y - y_max) - BORDER_Y; }
    else{ max_line_size =  (y_min - BORDER_Y); }

    do{
        x_max =  (BORDER_X) + (rand() %  (MAX_X - (2 * BORDER_X)) ) + 1 ; //random location between borders (plus 1, so smallest width is xmin=0 and xmax=1)
    }while( (x_max > MAX_X) );

    do{
        x_min = (( BORDER_X)) + (rand() % (MAX_X - (2 * BORDER_X)) );
    }while( x_min > x_max || ( (x_max - x_min)/*length*/  > max_line_size) );
    
    // These blocks of code remove a rectangular shape with triangles on top and bottom, out of the map
    u_border_y = y_min;
    u_border_x = x_min;
    u_len_line = x_max - x_min;

    f_line_found = 0; 

    for(int y = 0; y < MAX_Y; y++) { 
        for(int x = 0; x < MAX_X; x++) {       
            if(x >= x_min && x <= x_max && y >= y_min && y <= y_max){
                array[(y*col) + x] = pixel;//W creates Water, L1 creates Land
                f_line_found = 1;
                if(rand() % dstrt == 1){//1 in (distortion_val) chance of a line length change
                    x_min = x_min + (rand()%3 - 1);
                    x_max = x_max + (rand()%3 - 1);  
                }     
            }
            else{
                if(array[(y*col) + x] != not_pixel){
                    array[(y*col) + x] = pixel;    
                }
            }
                
        }

        if(f_line_found == 1 && (rand() % dstrt ) == 1){
            do{x_min = x_min + (rand()%3 - 1);
                if (x_min < BORDER_X){x_min = BORDER_X + 2;} 
                else if(x_min > (MAX_X - BORDER_X)){x_min = MAX_X - BORDER_X - 2;}   
            }while(x_min < BORDER_X || x_min > (MAX_X - BORDER_X));

            do{x_max = x_max + (rand()%3 - 1);
                if (x_max < BORDER_X){x_max = BORDER_X + 2;} 
                else if(x_max > (MAX_X - BORDER_X)){x_max = MAX_X - BORDER_X - 2;} 
            }while(x_max < BORDER_X || x_max > (MAX_X - BORDER_X));
        }
        if(y == y_max){
            l_border_y = y_max;
            l_border_x = x_min;
            l_len_line = x_max - x_min;
        }  

    }


//this block adds a triangular shape to the top border

    left_u_tri_skew = 1;
    right_u_tri_skew = 1;

    x_min = u_border_x ;
    x_max = u_border_x + u_len_line;
    y_min = u_border_y - u_len_line;
    y_max = u_border_y;

    for(int y = y_max; y > y_min; y--){
        if(y > y_min){
            x_min -= -1 * (left_u_tri_skew)  ;
            x_max -= (right_u_tri_skew)  ;  

            if( ((rand() % dstrt ) == 1) ){
                do{x_min = x_min + (rand()%3 - 1);
                    if (x_min < BORDER_X){x_min = BORDER_X + 2;} 
                    else if(x_min > (MAX_X - BORDER_X)){x_min = MAX_X - BORDER_X - 2;}   
                }while(x_min < BORDER_X || x_min > (MAX_X - BORDER_X));

                do{x_max = x_max + (rand()%3 - 1);
                    if (x_max < BORDER_X){x_max = BORDER_X + 2;} 
                    else if(x_max > (MAX_X - BORDER_X)){x_max = MAX_X - BORDER_X - 2;} 
                }while(x_max < BORDER_X || x_max > (MAX_X - BORDER_X));  
            }

        }
        for(int x = 0; x < MAX_X; x++){
            if(x >= x_min && x <= x_max && y >= y_min && y <= y_max){
                array[(y*col) + x] = pixel;//W creates Water, L1 creates Land
                if(rand() % dstrt == 1){//1 in (distortion_val) chance of a line length change
                    x_min = x_min + (rand()%3 - 1);
                    x_max = x_max + (rand()%3 - 1);  
                } 
            }
            else {
                if(array[(y*col) + x] != not_pixel){
                    array[(y*col) + x] = pixel;    
                }
            }
        }
    }

    //this block adds a triangular shape to the lower border

    left_l_tri_skew = 1;
    right_l_tri_skew = 1;

    x_min = l_border_x ;
    x_max = l_border_x + l_len_line;
    y_min = l_border_y;
    y_max = l_border_y + l_len_line;

    for(int y = y_min; y < y_max; y++){
        if(y < y_max){
            x_min -= - (left_l_tri_skew)   ;
            x_max -= (right_l_tri_skew)  ;

            if( ((rand() % (rigidity+1) ) == 0) ){
                do{x_min = x_min + (skewed_neg_pos_gen(skew3));
                    if (x_min < BORDER_X){x_min = BORDER_X + 2;} 
                    else if(x_min > (MAX_X - BORDER_X)){x_min = MAX_X - BORDER_X - 2;}   
                }while(x_min < BORDER_X || x_min > (MAX_X - BORDER_X));

                do{x_max = x_max + (skewed_neg_pos_gen(skew4));
                    if (x_max < BORDER_X){x_max = BORDER_X + 2;} 
                    else if(x_max > (MAX_X - BORDER_X)){x_max = MAX_X - BORDER_X - 2;} 
                }while(x_max < BORDER_X || x_max > (MAX_X - BORDER_X));  
            }
        }
        for(int x = 0; x < MAX_X; x++){
            if(x >= x_min && x <= x_max && y >= y_min && y <= y_max){
                array[(y*col) + x] = W; //W creates Water, L1 creates Land
                if(rand() % dstrt == 1){//1 in (distortion_val) chance of a line length change
                    x_min = x_min + (rand()%3 - 1);
                    x_max = x_max + (rand()%3 - 1);  
                } 
            }
            else {
                if(array[(y*col) + x] != not_pixel){
                    array[(y*col) + x] = pixel;    
                }
            }
        }

        //THIS BLOCK HERE prevents stack smashing (an overflow)
        if(x_max > (MAX_X - BORDER_X) ){ break; }
        if(x_min > (MAX_X - BORDER_X) ){ break; }
        if(x_max < (BORDER_X) ){ break; }
        if(x_min < (BORDER_X) ){ break; }

        //The bottoms of the continents have a propensity to overflow
        if(y_max > (MAX_Y - BORDER_Y)){ 
            //This block adds bumpiness to the bottom of the rectangle in the case of an overflow      
            for(int x = l_border_x; x <= l_border_x + l_len_line; x++){
                
                y_len += ( neg_pos_gen() );
                if(y_len < 1){ y_len = 2; }
                else if(y_len + l_border_y > (MAX_Y - BORDER_Y - 30)){ y_len = 1 + neg_pos_gen(); }

                for(int y = l_border_y; y < (l_border_y + y_len); y++){
                    array[(y*col) + x] = pixel;
                }
            }
            break; }

        if(y_min > (MAX_Y - BORDER_Y)){ break; }
        if(y_max < (BORDER_Y)){ break; }
        if(y_min < (BORDER_Y)){ break; }
    }

}

void gen_circle(int water_or_land, int max_size, int distortion, int hard_code_distortion, int render_direction, int* array, int row, int col){
    //variables for circle
    int x, y; 

    int x_min;
    int x_max;        
    int y_min;
    int y_max;


    int c_center_y;
    int c_center_x;
    int c_sqrd;

    int c_size;

    int c_increase;
    int c_x_pos;
    int c_y_pos;

    int terrain_type;//sets the terrain 
    if(water_or_land == 1){terrain_type = L1;}
    else if(water_or_land == 0){terrain_type = W;}
        
    //this block ADDS a left to right circle to somewhere on the map
    c_size = (rand() % max_size);//300ish is good for continents
    // c_size/2 = radius


    x_min = ( (rand() % (MAX_X - (2 * BORDER_X) - c_size - 100 )) + BORDER_X) + 50 ; //there is a buffer of 50 on both sides
    x_max = x_min + c_size;
    y_min = ( (rand() % (MAX_Y - (2 * BORDER_Y) - c_size - 100)) + BORDER_Y) + 50;
    y_max = y_min + c_size;

    if(hard_code_distortion == 0){distortion = (rand() % ( ( (distortion/5) * 4) + 1 )) + (distortion/5);} // random value in distortion

    c_x_pos = x_min;
    c_y_pos = y_min;

    //calculates the point in the middle of circle
    c_center_y = y_min + (y_max - y_min)/2 ; 
    c_center_x = x_min + (x_max - x_min)/2 ;

    c_sqrd = (c_size/2)*(c_size/2); // radius squared

    for(int y = 0; y < MAX_Y; y++){
        for(int x = 0; x < MAX_X; x++){
            //a simple pythagorean formula is used to calculate bounds of circle
            if(x > x_min && x < x_max && y > y_min && y < y_max){
                if( (x - c_center_x )*(x - c_center_x) + (y - c_center_y) * (y - c_center_y) <= c_sqrd){
                    array[(y*col) + x] = terrain_type;
                    if(rand() % distortion == 1){//1 in (bumpiness_value) chance of a line length change
                        //if(x_min < (MAX_X - BORDER_X - c_size) && x_min > BORDER_X && x_max < (MAX_X - BORDER_X) && x_max > (BORDER_X + 1) ){
                            c_increase = (rand()% 3 - 1);
                            c_center_x = c_center_x + c_increase;
                            x_min += (rand()% 3 - 1);
                            x_max +=  c_increase;
                        //}
                        //else{
                            if (x_min < BORDER_X || x_max < (BORDER_X + 1) ){
                                c_center_x++;
                                x_min++;
                                x_max++;   
                            }
                            else if( x_min > (MAX_X - BORDER_X - c_size) || x_max > (MAX_X - BORDER_X) ){
                                c_center_x--;
                                x_min--;
                                x_max--;    
                            } 
                        //}
                    } 
                }
                else{
                    if(array[(y*col) + x] != L1){
                        array[(y*col) + x] = W;
                    }
                }

            }
        }
    }
    

    // this block ADDS an up to down circle ontop of the previous circle, which "bumpifies" the rounded edges 
    // c_size/2 is radius

    if (render_direction == 2){
        
        x_min = c_x_pos;
        x_max = x_min + c_size;
        y_min = c_y_pos;
        y_max = y_min + c_size;

        if(hard_code_distortion == 0){distortion = (rand() % ( ( (distortion/5) * 4) + 1 )) + (distortion/5); }

        //calculates the point in the middle of circle
        c_center_y = y_min + (y_max - y_min)/2 ; 
        c_center_x = x_min + (x_max - x_min)/2 ;

        c_sqrd = (c_size/2)*(c_size/2); // radius squared

        for(int x = 0; x < MAX_X; x++){
            for(int y = 0; y < MAX_Y; y++){
                //a simple pythagorean formula is used to calculate bounds of circle
                if(x > x_min && x < x_max && y > y_min && y < y_max){
                    if( (x - c_center_x )*(x - c_center_x) + (y - c_center_y) * (y - c_center_y) <= c_sqrd){
                        array[(y*col) + x] = terrain_type;
                        if(rand() % distortion == 1){//1 in (bumpiness_value) chance of a line length change
                            //if(y_min < (MAX_Y - BORDER_Y - c_size) && y_min > BORDER_Y && y_max < (MAX_Y - BORDER_Y) && y_max > (BORDER_Y + 1)){
                                c_increase = (rand()% 3 - 1);
                                c_center_y = c_center_y + c_increase;
                                y_min += (rand()% 3 - 1);
                                y_max +=  c_increase;
                            //}

                            //else{
                                if (y_min < BORDER_Y || y_max < (BORDER_Y + 1) ){ 
                                    c_center_y++;
                                    y_min++;
                                    y_max++;
                                }
                                else if( y_min > (MAX_Y - BORDER_Y - c_size) || y_max > (MAX_Y - BORDER_Y) ){
                                    c_center_y--;
                                    y_min--;
                                    y_max--;
                                }
                            //}
                        } 
                    }
                    else{
                        if(array[(y*col) + x] != L1){
                            array[(y*col) + x] = W;
                        }
                    }

                }
            }
        }
    }

}

void gen_ocean_split(int* array, int row, int col){
    //This function adds a max 150 pixel wide ocean that spits the pieces of land, it runs from BORDER_Y to MAX_Y
    //These do whiles set up the bounds of the ocean split

    int ocean_angle = (rand() % 7) - 3; //angle between -3 and 3

    int x, y; 

    int x_min;
    int x_max;        
    int y_min;
    int y_max;

    y_max = MAX_Y - BORDER_Y;
    y_min = BORDER_Y;

    x_min = (rand() % (MAX_X - (2 * BORDER_X) - 300)) + (BORDER_X + 100) ; // random location with a buffer of 100 on left, and 200 on right

    x_max = x_min + (rand() % 100) + 50; 
    //random location between borders with a min of 50 and max of 150

    for(int y = 0; y < MAX_Y; y++){ 
        for(int x = 0; x < MAX_X; x++){   
            if(x >= x_min && x <= x_max && y >= y_min && y <= y_max){
                array[(y*col) + x] = W;//W creates Water, L1 creates Land

                if(rand() % 5 == 1){//1 in 5 chance of a line length change
                    x_min = x_min + (rand()%3 - 1);
                    x_max = x_max + (rand()%3 - 1);  
                }  
                if (x_max > (MAX_X - BORDER_X)){
                    x_max--;
                    x_min--;
                    ocean_angle * -1;
                }
                if (x_min < (BORDER_X)){
                    x_max++;
                    x_min++;
                    ocean_angle * -1;
                }       
            } 
        }
        x_min = x_min + ocean_angle;
        x_max = x_max + ocean_angle;  
        if(rand() % 10 == 1){ocean_angle = (rand() % 7) - 3; }//1 in 20 chance of a whole line angle change //angle between -3 and 3      
    }

}

void gen_ocean_errosion(int* array, int row, int col){
    //This function adds chunks of ocean flowing into the mainlands from left to right
    //it uses a modified version of the gen_circle algo, slightly optimized for faster runtime
    int x, y; 

    int x_min;
    int x_max;        
    int y_min;
    int y_max;

    int distortion;

    int c_center_y;
    int c_center_x;
    int c_sqrd;

    int c_size;

    int c_increase;
    int c_x_pos;
    int c_y_pos;

    for(int y = 1; y < (MAX_Y - BORDER_Y - 10) ; y++){ 
        for(int x = 1; x < (MAX_X - BORDER_X - 10) ; x++){   
            if(array[(y*col) + x] == L1 && array[(y*col) + (x-1)] == W){
                if(rand() % 30 == 1){//1 in 20 chance of a circle of water spawned into the border
                            
                    //this inner block REMOVES a left to right circle to the border on the map
                    c_size = (rand() % 15) + 5; //size between 5 and 10

                    // c_size/2 = radius
                    c_x_pos = x - c_size/2;
                    c_y_pos = y - c_size/2;

                    x_min = c_x_pos;
                    x_max = x_min + c_size;
                    y_min = c_y_pos;
                    y_max = y_min + c_size;

                    distortion = 3;  

                    //calculates the point in the middle of circle
                    c_center_y = y_min + (y_max - y_min)/2 ; 
                    c_center_x = x_min + (x_max - x_min)/2 ;

                    c_sqrd = (c_size/2)*(c_size/2); // radius squared

                    for(int y = y_min; y < y_max; y++){
                        for(int x = 0; x < (MAX_X - BORDER_X - 10); x++){
                            //a simple pythagorean formula is used to calculate bounds of circle
                            if(x > x_min && x < x_max && y > y_min && y < y_max){
                                if( (x - c_center_x )*(x - c_center_x) + (y - c_center_y) * (y - c_center_y) <= c_sqrd){
                                    array[(y*col) + x] = W;
                                    if(rand() % distortion == 1){//1 in (bumpiness_value) chance of a line length change
                                        c_increase = (rand()% 3 - 1);
                                        c_center_x = c_center_x + c_increase;
                                        x_min += (rand()% 3 - 1);
                                        x_max +=  c_increase;

                                        if (x_min < BORDER_X || x_max < (BORDER_X + 1) ){
                                            c_center_x++;
                                            x_min++;
                                            x_max++;   
                                        }
                                        else if( x_min > (MAX_X - BORDER_X - c_size) || x_max > (MAX_X - BORDER_X) ){
                                            c_center_x--;
                                            x_min--;
                                            x_max--;    
                                        } 
                                    } 
                                }

                            }
                        }
                    }               

                    // this block ADDS an up to down circle ontop of the previous circle, which "bumpifies" the rounded edges 
                    // c_size/2 is radius
                    x_min = c_x_pos;
                    x_max = x_min + c_size;
                    y_min = c_y_pos;
                    y_max = y_min + c_size;

                    distortion = 3;

                    //calculates the point in the middle of circle
                    c_center_y = y_min + (y_max - y_min)/2 ; 
                    c_center_x = x_min + (x_max - x_min)/2 ;

                    c_sqrd = (c_size/2)*(c_size/2); // radius squared

                    for(int x = x_min; x < x_max; x++){
                        for(int y = 0; y < (MAX_Y - BORDER_Y - 10); y++){
                            //a simple pythagorean formula is used to calculate bounds of circle
                            if(x > x_min && x < x_max && y > y_min && y < y_max){
                                if( (x - c_center_x )*(x - c_center_x) + (y - c_center_y) * (y - c_center_y) <= c_sqrd){
                                    array[(y*col) + x];
                                    if(rand() % distortion == 1){//1 in (bumpiness_value) chance of a line length change
                                        
                                        c_increase = (rand()% 3 - 1);
                                        c_center_y = c_center_y + c_increase;
                                        y_min += (rand()% 3 - 1);
                                        y_max +=  c_increase;
                                    
                                        if (y_min < BORDER_Y || y_max < (BORDER_Y + 1) ){ 
                                            c_center_y++;
                                            y_min++;
                                            y_max++;
                                        }
                                        else if( y_min > (MAX_Y - BORDER_Y - c_size) || y_max > (MAX_Y - BORDER_Y) ){
                                            c_center_y--;
                                            y_min--;
                                            y_max--;
                                        }             
                                    } 
                                }


                            }
                        }
                    }


                }      
            } 
        }
    }




    //This block adds chunks of ocean flowing into the mainlands from RIGHT to Left
    for(int y = 1; y < (MAX_Y - BORDER_Y - 10) ; y++){ 
        for(int x = (MAX_X - BORDER_X - 11); x > 0 ; x--){   
            if(array[(y*col) + x] == L1 && array[(y*col) + (x-1)] == W){
                if(rand() % 30 == 1){//1 in 20 chance of a circle of water spawned into the border
                            
                    //this inner block REMOVES a left to right circle to the border on the map

                    c_size = (rand() % 15) + 5; //size between 5 and 10

                    // c_size/2 = radius
                    c_x_pos = x - c_size/2;
                    c_y_pos = y - c_size/2;

                    x_min = c_x_pos;
                    x_max = x_min + c_size;
                    y_min = c_y_pos;
                    y_max = y_min + c_size;

                    distortion = 3;  // 50% chance of a line change

                    //calculates the point in the middle of circle
                    c_center_y = y_min + (y_max - y_min)/2 ; 
                    c_center_x = x_min + (x_max - x_min)/2 ;

                    c_sqrd = (c_size/2)*(c_size/2); // radius squared

                    for(int y = y_min; y < y_max; y++){
                        for(int x = 0; x < (MAX_X - BORDER_X - 10); x++){
                            //a simple pythagorean formula is used to calculate bounds of circle
                            if(x > x_min && x < x_max && y > y_min && y < y_max){
                                if( (x - c_center_x )*(x - c_center_x) + (y - c_center_y) * (y - c_center_y) <= c_sqrd){
                                    array[(y*col) + x] = W;
                                    if(rand() % distortion == 1){//1 in (bumpiness_value) chance of a line length change
                                        c_increase = (rand()% 3 - 1);
                                        c_center_x = c_center_x + c_increase;
                                        x_min += (rand()% 3 - 1);
                                        x_max +=  c_increase;

                                        if (x_min < BORDER_X || x_max < (BORDER_X + 1) ){
                                            c_center_x++;
                                            x_min++;
                                            x_max++;   
                                        }
                                        else if( x_min > (MAX_X - BORDER_X - c_size) || x_max > (MAX_X - BORDER_X) ){
                                            c_center_x--;
                                            x_min--;
                                            x_max--;    
                                        } 
                                    } 
                                }

                            }
                        }
                    }               

                    // this block ADDS an up to down circle ontop of the previous circle, which "bumpifies" the rounded edges 
                    // c_size/2 is radius
                    x_min = c_x_pos;
                    x_max = x_min + c_size;
                    y_min = c_y_pos;
                    y_max = y_min + c_size;

                    distortion = 3;

                    //calculates the point in the middle of circle
                    c_center_y = y_min + (y_max - y_min)/2 ; 
                    c_center_x = x_min + (x_max - x_min)/2 ;

                    c_sqrd = (c_size/2)*(c_size/2); // radius squared

                    for(int x = x_min; x < x_max; x++){
                        for(int y = 0; y < (MAX_Y - BORDER_Y - 10); y++){
                            //a simple pythagorean formula is used to calculate bounds of circle
                            if(x > x_min && x < x_max && y > y_min && y < y_max){
                                if( (x - c_center_x )*(x - c_center_x) + (y - c_center_y) * (y - c_center_y) <= c_sqrd){
                                    array[(y*col) + x] = W;
                                    if(rand() % distortion == 1){//1 in (bumpiness_value) chance of a line length change
                                        
                                        c_increase = (rand()% 3 - 1);
                                        c_center_y = c_center_y + c_increase;
                                        y_min += (rand()% 3 - 1);
                                        y_max +=  c_increase;
                                    
                                        if (y_min < BORDER_Y || y_max < (BORDER_Y + 1) ){ 
                                            c_center_y++;
                                            y_min++;
                                            y_max++;
                                        }
                                        else if( y_min > (MAX_Y - BORDER_Y - c_size) || y_max > (MAX_Y - BORDER_Y) ){
                                            c_center_y--;
                                            y_min--;
                                            y_max--;
                                        }             
                                    } 
                                }


                            }
                        }
                    }


                }      
            } 
        }
    }     
}

void gen_rivers(int* array, int row, int col){
    
    //Here we spawn in rivers

    //GEOGRAPHICAL BACKGROUND: Rivers should always flow from higher elevation to lower elevation

    //"river units" are about 2 to 3 pixels in length, with the size of 2 to 3 varying for aesthetic realism
    //use of W2 below to plump up the river while avoiding collision detection - i.e. river route is 1 pixel in length
    //I tried having the a "river unit" at 1 pixel length and it looked choppy and rigid

    int r_len;//river length in river units
    int r_x, r_y;//river x and river y
    int r_x0, r_y0;//river x naught and y naught (the river starting postions)

    int x_dir, y_dir; //x and y river direction -- i.e. skew values
    //skew between 3 and 9 inclusive, any greater or smaller will create a visually awkward intense skew (the word awkward is spelt so awkwardly)

    //NOTE: our land search iterator still operates pixel by pixel as opposed to through river units
    for(int y = 1; y < (MAX_Y - BORDER_Y - 10)/* -10 is a buffer */; y++){ 
        for(int x = 1; x < (MAX_X - BORDER_X - 10); x++){
            if(array[(y*col) + x] == L1 && array[(y*col) + (x-1)] == W){
                if((rand() % 8) == 1){// 1 in 10 chance of spawning in river
                    //here is the river line algorithm
                    r_x0 = r_x = x;
                    r_y0 = r_y = y;
                    r_len = ((rand() % 64) + 65); //length between 1 and 128

                    x_dir = (rand() % 10) + 1;
                    y_dir = (rand() % 10) + 1;

                    for(int p = 0; p < r_len; p++){ //iterates and draws a line as series of points
                        if(array[(r_y * col) + r_x] == W){break;}
                        array[(r_y * col) + r_x] = W;
                        array[( (r_y + ((rand() % 3) - 1)) * col) + ( r_x + ((rand() % 3) - 1 ) ) ] = W2;

                        //array[r_x + ((rand() % 5) - 2 )][r_y + ((rand() % 5) - 2)] = W2; 

                        do{
                            if((rand() % 3) != 1){r_x += skewed_neg_pos_gen(x_dir);} //there is 33% of a 0 instead of a skewed -1 or 1
                            if((rand() % 3) != 1){r_y += skewed_neg_pos_gen(y_dir);}
                        }while(r_x == r_x0 && r_y == r_y0);
                        r_x0 = r_x;
                        r_y0 = r_y;
                        
                    }
                }
            }
        }
    }

    //same algo as before -- but the spawning of the river is from right to left

    for(int y = 1; y < (MAX_Y - BORDER_Y - 10)/* -10 is a buffer */; y++){ 
        for(int x =  (MAX_X - BORDER_X - 10); x > 0; x--){
            if(array[(y*col) + x] == L1 && array[(y*col) + (x+1)] == W){
                if((rand() % 8) == 1){// 1 in 10 chance of spawning in river
                    //here is the river line algorithm
                    r_x0 = r_x = x;
                    r_y0 = r_y = y;
                    r_len = ((rand() % 64) + 65); //length between 64 and 128

                    x_dir = (rand() % 10) + 1; //should be a value between 1 and 11; it's a parameter in the skewed generator
                    y_dir = (rand() % 10) + 1;

                    for(int p = 0; p < r_len; p++){ //iterates and draws a line as series of points
                        if(array[(r_y * col) + r_x] == W){break;}
                        array[(r_y * col) + r_x] = W;
                        array[( (r_y + ((rand() % 3) - 1)) * col) + ( r_x + ((rand() % 3) - 1 ) ) ] = W2; //see the river units note at top
                        //array[r_x + ((rand() % 5) - 2 )][r_y + ((rand() % 5) - 2)] = W2; 

                        do{
                            if((rand() % 3) != 1){r_x += skewed_neg_pos_gen(x_dir);} //there is 33% of a 0 instead of a skewed -1 or 1
                            if((rand() % 3) != 1){r_y += skewed_neg_pos_gen(y_dir);}
                        }while(r_x == r_x0 && r_y == r_y0);
                        r_x0 = r_x;
                        r_y0 = r_y;
                        
                    }
                }
            }
        }
    }


}

int test_gen_biomes(int* array, int row, int col){
    //this function adds in a rough estimate of climated base on latitude 

    //Here we add base layer of land -- in this case P = Polar/Snow or Ice, ontop of the light blue render acting as an outline
    for(int y = 0; y < MAX_Y; y++){
        for(int x = 0; x < MAX_X; x++){
            if(array[(y*col) + x] == L1){
                array[(y*col) + x] = L1;
                if(array[((y-1)*col) + x] == L1 || array[((y-1)*col) + x] == P){ //check above           
                    if(array[(y*col) + (x+1)] == L1 || array[(y*col) + (x+1)] == P){ //check right  
                        if(array[((y-1)*col) + x] == L1 || array[((y-1)*col) + x] == P){ //check below             
                            if(array[(y*col) + (x-1)] == L1 || array[(y*col) + (x-1)] == P){ //check left
                                array[(y*col) + x] = P;
                            }
                        }
                    }
                }
                
            }
        }
    }

    int biome_val = 0;
    
     for(int x = 0; x < MAX_X; x++){
        biome_val = 0;
        for(int y = 0; y < MAX_Y; y++){
            if(biome_val < 501){biome_val += 1;} 
            
            if(array[(y*col) + x] == P){
                array[(y*col) + x] = biome_val;
            }
        }
     }


}

int gen_biomes(int* array, int row, int col){
//this function adds in a rough estimate of climated base on latitude 

    //Here we add base layer of land -- in this case P = Polar/Snow or Ice, ontop of the light blue render acting as an outline
    for(int y = 0; y < MAX_Y; y++){
        for(int x = 0; x < MAX_X; x++){
            if(array[(y*col) + x] == L1){
                array[(y*col) + x] = L1;
                if(array[((y-1)*col) + x] == L1 || array[((y-1)*col) + x] == P){ //check above           
                    if(array[(y*col) + (x+1)] == L1 || array[(y*col) + (x+1)] == P){ //check right  
                        if(array[((y-1)*col) + x] == L1 || array[((y-1)*col) + x] == P){ //check below             
                            if(array[(y*col) + (x-1)] == L1 || array[(y*col) + (x-1)] == P){ //check left
                                array[(y*col) + x] = P;
                            }
                        }
                    }
                }
                
            }
        }
    }

    int biome_val = 0;
    int add_val = (MAX_Y/2) / 500;
    int equator = (MAX_Y/2);

    int south_ice_cap = 0; //default is 0
    int north_ice_cap = 0; //default is 0

    for(int x = 1; x < MAX_X; x++){
        if((rand() % 7) == 0){south_ice_cap += (rand() % 3) - 1;}
        if((rand() % 7) == 0){north_ice_cap += (rand() % 3) - 1;}
        if((rand() % 7) == 0){equator += (rand() % 3) - 1;}
        biome_val = 0;

        for(int y = (75 + north_ice_cap); y < MAX_Y - (75 + south_ice_cap); y++){
            if(y < equator){
                if( (y % 3) == 0 || (y % 3) == 1 ){
                    if(biome_val < 498){
                        biome_val += 3;  
                    }
                }
            }
            else if(y > equator){
                if( (y % 3) == 0 || (y % 3) == 1 ){
                    if(biome_val > 0){
                        biome_val -= 3;
                    }
                }
            }
            if(array[(y*col) + x] == P){
                
                array[(y*col) + x] = biome_val;
            }
        }
    }

    return 0;
}

void regular_colour_fill(int *renderer, int* array, int row, int col){
    //all the colours of the biome are shown unblended -- good test function

    SDL_SetRenderDrawColor(*&renderer, 0, 0, 130, 255); //sets the background to dark blue
    SDL_RenderClear(*&renderer);

    SDL_SetRenderDrawColor(*&renderer, 40, 200, 220, 255); //sets render colour to light blue


    //sets up border
    int left_x = 10;
    int right_x = MAX_X - 10;
    int bottom_y = MAX_Y - 10;
    int top_y = 10;
    SDL_RenderDrawLine(*&renderer, left_x, top_y, right_x, top_y);
    SDL_RenderDrawLine(*&renderer, right_x, top_y, right_x, bottom_y);
    SDL_RenderDrawLine(*&renderer, right_x, bottom_y, left_x, bottom_y);
    SDL_RenderDrawLine(*&renderer, left_x, bottom_y, left_x, top_y);

    //colours in the light blue outline!
    for(int y = 0; y < MAX_Y; y++){
        for(int x = 0; x < MAX_X; x++ ){
            if(array[(y*col) + x] == L1){
                SDL_RenderDrawPoint(*&renderer, x, y); 
            }
        }

    }

    SDL_SetRenderDrawColor(*&renderer, 255, 255, 255, 255); //sets render colour to white
    for(int y = 0; y < MAX_Y; y++){
        for(int x = 0; x < MAX_X; x++ ){
            if(array[(y*col) + x] == P){
                SDL_RenderDrawPoint(*&renderer, x, y); 
            }
        }
    }

    SDL_SetRenderDrawColor(*&renderer, 130, 190, 130, 255); //sets render colour to tundra green
    for(int y = 0; y < MAX_Y; y++){
        for(int x = 0; x < MAX_X; x++ ){
            if(array[(y*col) + x] > P && array[(y*col) + x] <= T){
                SDL_RenderDrawPoint(*&renderer, x, y); 
            }
        }
    }

    SDL_SetRenderDrawColor(*&renderer, 80, 160, 80, 255); //sets render colour to green
    for(int y = 0; y < MAX_Y; y++){
        for(int x = 0; x < MAX_X; x++ ){
            if(array[(y*col) + x] > T && array[(y*col) + x] <= F){
                SDL_RenderDrawPoint(*&renderer, x, y); 
            }
        }
    }

    SDL_SetRenderDrawColor(*&renderer, 222, 206, 126, 255); //sets render colour to sand colour
    for(int y = 0; y < MAX_Y; y++){
        for(int x = 0; x < MAX_X; x++ ){
            if(array[(y*col) + x] > F && array[(y*col) + x] <= D){
                SDL_RenderDrawPoint(*&renderer, x, y); 
            }
        }
    }

    SDL_SetRenderDrawColor(*&renderer, 120, 140, 40, 255); //sets render colour to light olive green
    for(int y = 0; y < MAX_Y; y++){
        for(int x = 0; x < MAX_X; x++ ){
            if(array[(y*col) + x] > D && array[(y*col) + x] <= G){
                SDL_RenderDrawPoint(*&renderer, x, y); 
            }
        }
    }

    SDL_SetRenderDrawColor(*&renderer, 0, 100, 0, 255); //sets render colour to dark green
    for(int y = 0; y < MAX_Y; y++){
        for(int x = 0; x < MAX_X; x++ ){
            if(array[(y*col) + x] > G && array[(y*col) + x] <= J){
                SDL_RenderDrawPoint(*&renderer, x, y); 
            }
        }
    }

    SDL_RenderPresent(*&renderer);
}    

void blended_colour_fill(int *renderer, int* array, int row, int col){
    //blends all the colours of the biomes together

    typedef struct{
        float  r;
        float  g;
        float  b;
        float biome_val;
    }Colour;
    
    Colour dark_blue;
    dark_blue.r = 0.0; 
    dark_blue.g = 0.0; 
    dark_blue.b = 130.0;
    dark_blue.biome_val = (float)W; //also w2 but this is the bottom/default layer

    Colour light_blue;
    light_blue.r = 40.0; 
    light_blue.g = 200.0; 
    light_blue.b = 220.0;
    light_blue.biome_val = (float)L1;

    Colour dark_green;
    dark_green.r = 0.0; 
    dark_green.g = 100.0 - 40.0; 
    dark_green.b = 0.0; 
    dark_green.biome_val = (float)J;

    Colour light_olive_green;
    light_olive_green.r = 80.0 - 10.0; 
    light_olive_green.g = 140.0 - 40.0; 
    light_olive_green.b = 80.0 - 10.0;
    light_olive_green.biome_val = (float)G;

    Colour sand;
    sand.r = 222.0 - 30.0; 
    sand.g = 206.0 - 40.0; 
    sand.b = 126.0 - 40.0;
    sand.biome_val = (float)D;

    Colour grass_green;
    grass_green.r = 80.0 - 40.0; 
    grass_green.g = 160.0 - 40.0; 
    grass_green.b = 80.0 - 40.0;
    grass_green.biome_val = (float)F;

    Colour tundra_green;
    tundra_green.r = 130.0 - 40.0;
    tundra_green.g = 190.0 - 40.0; 
    tundra_green.b = 130.0 - 40.0;
    tundra_green.biome_val = (float)T;

    Colour white;
    white.r = 255.0; 
    white.g = 255.0; 
    white.b = 255.0;
    white.biome_val = (float)P;

    Colour latitude_colours[6] = {white, tundra_green, grass_green, sand, light_olive_green, dark_green,};            

    SDL_SetRenderDrawColor(*&renderer, (int)light_blue.r, (int)light_blue.g, (int)light_blue.b, 255); //sets render colour to light blue
    for(int y = 0; y < row; y++){
        for(int x = 0; x < col; x++ ){
            if(array[(y*col) + x] == L1){
                    SDL_RenderDrawPoint(*&renderer, x, y); 
            }
        }
    }  

    float red;
    float green;
    float blue;

    float d_r; //d for delta as in: change in
    float d_g;
    float d_b;

    float current_biome_val;
    float remainder_biome_val;

    for(int x = 0; x < col; x++){
        for(int y = 0; y < row; y++ ){
                current_biome_val = (float)(array[(y*col) + x]); //the biome value is stored in the current pixel mapped to an int
                remainder_biome_val = (float)current_biome_val;

                if (current_biome_val == latitude_colours[0].biome_val){ 
                    red = latitude_colours[0].r;
                    green = latitude_colours[0].g;
                    blue = latitude_colours[0].b;
                    
                }

                for(int i = 0; i < 5; i++){
                    if (current_biome_val > latitude_colours[i].biome_val){
                        if(current_biome_val < latitude_colours[i+1].biome_val){

                            remainder_biome_val =  current_biome_val - latitude_colours[i].biome_val;
                            
                            //red
                            d_r = ( (latitude_colours[i+1].biome_val - latitude_colours[i].biome_val) / (remainder_biome_val) ); 
                            red = latitude_colours[i].r - ( (latitude_colours[i].r - latitude_colours[i+1].r) / d_r ); 
                        
                            //green
                            d_g = ( (latitude_colours[i+1].biome_val - latitude_colours[i].biome_val) / (remainder_biome_val) );  
                            green = latitude_colours[i].g - ( (latitude_colours[i].g - latitude_colours[i+1].g) / d_g );

                            //blue
                            d_b = ( (latitude_colours[i+1].biome_val - latitude_colours[i].biome_val)  / (remainder_biome_val) );  
                            blue = latitude_colours[i].b - ( (latitude_colours[i].b - latitude_colours[i+1].b) / d_b );
                        }
                        else{
                            red = latitude_colours[i+1].r;
                            green = latitude_colours[i+1].g;
                            blue = latitude_colours[i+1].b;
                        }
                    }
                }

                if(array[(y*col) + x] != L1 && array[(y*col) + x] != W && array[(y*col) + x] != W2){ 
                SDL_SetRenderDrawColor(*&renderer, (int)red, (int)green, (int)blue, 255); //sets render colour according to the calculation
                SDL_RenderDrawPoint(*&renderer, x, y);  
                }
            
        }
    }

}

void map_constructor(int *renderer, int* array, int row, int col){

    //sets the background of map to default water
    for(int y = 0; y < row; y++){
        for(int x = 0; x < col; x++ ){
            array[(y*col) + x] = W;
        }

    }

    SDL_SetRenderDrawColor(*&renderer, 0, 0, 130, 255); //sets the background to dark blue
    SDL_RenderClear(*&renderer);

    SDL_SetRenderDrawColor(*&renderer, 40, 200, 220, 255); //sets render colour to light blue


    //sets up border
    int left_x = 10;
    int right_x = MAX_X - 10;
    int bottom_y = MAX_Y - 10;
    int top_y = 10;
    SDL_RenderDrawLine(*&renderer, left_x, top_y, right_x, top_y);
    SDL_RenderDrawLine(*&renderer, right_x, top_y, right_x, bottom_y);
    SDL_RenderDrawLine(*&renderer, right_x, bottom_y, left_x, bottom_y);
    SDL_RenderDrawLine(*&renderer, left_x, bottom_y, left_x, top_y);

    int continents = MIN_CONTINENTS + (rand() % (MAX_CONTINENTS - MIN_CONTINENTS));

    //first block creates the main land masses
    for(int i = 0; i < continents; i++){
        gen_stretched_hexagon(0, 80, array, row, col);
        gen_circle(1, 300, 100, 0, 2, array, row, col);
        gen_circle(0, 150, 90, 1, 2, array, row, col);
    }

    //this second block adds geographic realism to the land masses
    gen_ocean_split(array, row, col);
    gen_ocean_errosion(array, row, col);
    gen_ocean_errosion(array, row, col);
    gen_rivers(array, row, col);
    gen_rivers(array, row, col);

    gen_biomes(array, row, col);
    blended_colour_fill(renderer, array, row, col);

    //Test functions below
    //______________________________________________
    //test_gen_biomes(array, row, col);
    //regular_colour_fill(renderer, array, row, col);

    SDL_RenderPresent(*&renderer);
}

int gameLoop(){ 
    //Initializes SD
    SDL_Window     *window; //SDL_Window pointer type
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("SDL2 Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, MAX_X, MAX_Y, 0); 

    if(window==NULL)
    {   
        printf("Could not create window: %s\n", SDL_GetError());
        return 0;
    }
   
    SDL_Renderer *renderer;
    renderer = SDL_CreateRenderer(window, -1, 0);
    if(renderer==NULL)
    {   
        printf("Could not render: %s\n", SDL_GetError());
        return 0;
    }

    int col = MAX_X;
    int row = MAX_Y;

    //Initializes a 2D array that will store every pixel on the map
    int* gen_map;
    gen_map = malloc(col*row*sizeof(int)); //a heap allocated version of gen_map[row][col]
    //NOTE: for some reason using gen_map[row][col] and then memset (for dynamic mem allocation), was crashing this function

    //Main Loop
    int endGame = 0;
    while (endGame == 0){
        int last_frame_time = SDL_GetTicks();
        while(!SDL_TICKS_PASSED(SDL_GetTicks(), last_frame_time + FRAME_TARGET_TIME)); //locks the fps

        map_constructor(renderer, (int*)gen_map, row, col);
        endGame = 1;
    } 

    SDL_Delay(100000); //100 seconds
    SDL_DestroyWindow(window);    
    SDL_Quit(); 

    //I'm leaking
    free(gen_map); 
    //jk

    return 0;
}

int main( int argc, char* args[] ) {
    srand(time(NULL));
    gameLoop();
    return 0;
}
