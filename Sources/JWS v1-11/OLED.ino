
// ******************************************
// Intégration d'un écran OLED SH1106 ou 1306
// ******************************************

// >>>>>>>> Sélectionner le modèle de l'écran OLED utilisé ICI <<<<<<<<<<
//#define USE_SSD1306 1 // Ecran OLED type SSD1306
#define USE_SH110X 1  // Ecran OLED type SH1106
// ----------------------------------------------------------------------

#include <Adafruit_GFX.h>

// Les caractères sont dans une matrice de 6x8 pixels averc 1 colonne de pixels libres à droite et 1 ligne de pixels en
// dessous. 
// Un paramètre optionnel (size pour taille) peut être passé à la fonction pour changer l'échelle d'affichage du caractère.
// La taille du caractère est multiplié par le facteur utilisé (ex: size=2 fera un rendu en 12x16 pixels par caractère). 

#define OLED_BLACK   0
#define OLED_WHITE   1
#define OLED_INVERSE 2

// Pour SSD1306
#if(USE_SSD1306)
  #include <Adafruit_SSD1306.h>
#endif

// Pour écran SH110X
#if(USE_SH110X)
  #include <Adafruit_SH110X.h>
#endif

#define SCREEN_WIDTH      128          // Largeur de l'écran OLED en pixels
#define SCREEN_HEIGHT      64          // Hauteur de l'écran OLED en pixels
#define OLED_RESET         -1          // GPIO utilisé pour le RESET (ou -1 si partagé)
#define SCREEN_ADDRESS   0x3c          // Voir le datasheet pour la définition de l'adresse : 0x3D pour 128x32, 0x3C pour
                                       // 128x64
bool OLED_on = false;                  // En cas de problème avec l'écran, les routines d'affichage sont ignorées

// Pour SSD1306
#if USE_SSD1306
  Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#endif

// Pour écran SH110X
#if USE_SH110X
  Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#endif

// Logo affiché à l'initialisation de l'écran OLED 128x64 px
const unsigned char Logo_JWS [] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1E, 0x00, 
0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF1, 0xE0, 0x1E, 0x00, 0xF0, 0x7F, 0xC0, 
0x00, 0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF1, 0xE0, 0x1F, 0x00, 0xF0, 0xFF, 0xE0, 
0x00, 0x00, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF1, 0xE0, 0x1F, 0x00, 0xF1, 0xFF, 0xE0, 
0x00, 0x00, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF1, 0xE0, 0x1F, 0x00, 0xF3, 0xE0, 0xE0, 
0x00, 0x00, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xF0, 0x3F, 0x81, 0xE3, 0xC0, 0x00, 
0x00, 0x00, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xF0, 0x3F, 0x81, 0xE3, 0xC0, 0x00, 
0x00, 0x00, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xF0, 0x3F, 0x81, 0xE3, 0xC0, 0x00, 
0x00, 0x00, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xF8, 0x3B, 0x81, 0xC3, 0xE0, 0x00, 
0x00, 0x00, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x78, 0x7B, 0xC3, 0xC1, 0xF8, 0x00, 
0x00, 0x00, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x78, 0x7B, 0xC3, 0xC1, 0xFE, 0x00, 
0x00, 0x00, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x78, 0x73, 0xC3, 0xC0, 0xFF, 0x80, 
0x00, 0x00, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x3C, 0xF1, 0xC3, 0x80, 0x3F, 0xC0, 
0x00, 0x00, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x3C, 0xF1, 0xE7, 0x80, 0x0F, 0xE0, 
0x00, 0x00, 0x00, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x3C, 0xF1, 0xE7, 0x80, 0x03, 0xE0, 
0x00, 0x00, 0x00, 0x77, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x3C, 0xE1, 0xE7, 0x00, 0x01, 0xF0, 
0x00, 0x00, 0x00, 0x77, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x1F, 0xE0, 0xF7, 0x00, 0x00, 0xF0, 
0x00, 0x00, 0x00, 0x77, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x1F, 0xE0, 0xFF, 0x00, 0x00, 0xF0, 
0x00, 0x00, 0x00, 0x77, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x1F, 0xE0, 0xFF, 0x00, 0x00, 0xF0, 
0x00, 0x00, 0x00, 0x77, 0x00, 0x00, 0x00, 0x00, 0x01, 0xF0, 0x0F, 0xC0, 0x7E, 0x07, 0x01, 0xE0, 
0x00, 0x00, 0x00, 0xF7, 0x80, 0x00, 0x00, 0x00, 0x1F, 0xE0, 0x0F, 0xC0, 0x7E, 0x07, 0xFF, 0xE0, 
0x00, 0x00, 0x00, 0xE3, 0x80, 0x00, 0x00, 0x00, 0x1F, 0xE0, 0x0F, 0xC0, 0x7E, 0x03, 0xFF, 0xC0, 
0x00, 0x00, 0x00, 0xE3, 0x80, 0x00, 0x00, 0x00, 0x1F, 0xC0, 0x0F, 0xC0, 0x7C, 0x01, 0xFF, 0x80, 
0x00, 0x00, 0x00, 0xE3, 0x80, 0x00, 0x00, 0x00, 0x0F, 0x00, 0x03, 0x00, 0x18, 0x00, 0x7C, 0x00, 
0x00, 0x03, 0x80, 0xE3, 0x81, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x01, 0xF0, 0xE3, 0xC7, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x7F, 0xC1, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x13, 0xC1, 0xE4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x09, 0xC1, 0xC8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x07, 0xC1, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x07, 0xC1, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x0F, 0x80, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x1F, 0x80, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x3E, 0x00, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x78, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0xF0, 0x00, 0x07, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x01, 0xE0, 0x3E, 0x03, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x07, 0xC1, 0xFF, 0xC1, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x0F, 0x87, 0xFF, 0xF0, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x1F, 0x3F, 0xE7, 0xFE, 0x7C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x3F, 0xFF, 0x24, 0x7F, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x7F, 0xFC, 0x24, 0x1F, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0xFF, 0xE0, 0x18, 0x03, 0xFF, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x01, 0xFF, 0x00, 0x18, 0x00, 0x7F, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x03, 0xFC, 0x00, 0x18, 0x00, 0x1F, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x07, 0xE0, 0x00, 0x18, 0x00, 0x03, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x0F, 0x80, 0x00, 0x18, 0x00, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x1C, 0x00, 0x00, 0x18, 0x00, 0x00, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0xBE, 0xFB, 0xCF, 0xB9, 0xC0, 0xC4, 0x8E, 0xFB, 0xD1, 0x03, 0xE8, 0xBE, 0xEF, 0xB9, 0xC0,
0x00, 0xA0, 0x82, 0x68, 0xAD, 0x40, 0x4A, 0x8E, 0x26, 0x11, 0x02, 0x05, 0x20, 0x48, 0x2D, 0x40,
0x01, 0xBE, 0xFA, 0x68, 0xA5, 0x60, 0x4A, 0x9A, 0x26, 0x1F, 0x03, 0x87, 0x38, 0x4F, 0xA5, 0x60,
0x01, 0xB0, 0xC2, 0x68, 0xB5, 0x20, 0x6A, 0xB3, 0x26, 0x1F, 0x00, 0xE2, 0x0E, 0x4C, 0x35, 0x20,
0x01, 0xB0, 0xC2, 0x69, 0xB7, 0x20, 0x3B, 0xB3, 0x27, 0x19, 0x00, 0x62, 0x06, 0x4C, 0x37, 0x20,
0x0F, 0xBE, 0xFB, 0xCF, 0xB3, 0x20, 0x31, 0xB1, 0x23, 0xD9, 0x03, 0xE2, 0x3E, 0x4F, 0xB3, 0x20,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
  
/*
                            #                                                                                      ####         
                            #                                           ####   ####        ####         ####     #########      
                            #                                           ####   ####        #####        ####    ###########     
                           ###                                          ####   ####        #####        ####   ############     
                           ###                                          ####   ####        #####        ####  #####     ###     
                           ###                                          ####    ####      #######      ####   ####              
                           ###                                          ####    ####      #######      ####   ####              
                           ###                                          ####    ####      #######      ####   ####              
                          #####                                         ####    #####     ### ###      ###    #####             
                          #####                                         ####     ####    #### ####    ####     ######           
                          #####                                         ####     ####    #### ####    ####     ########         
                          #####                                         ####     ####    ###  ####    ####      #########       
                          #####                                         ####      ####  ####   ###    ###         ########      
                          #####                                         ####      ####  ####   ####  ####           #######     
                         #######                                        ####      ####  ####   ####  ####             #####     
                         ### ###                                        ####      ####  ###    ####  ###               #####    
                         ### ###                                        ####       ########     #### ###                ####    
                         ### ###                                        ####       ########     ########                ####    
                         ### ###                                        ####       ########     ########                ####    
                         ### ###                                       #####        ######       ######      ###       ####     
                        #### ####                                  ########         ######       ######      ##############     
                        ###   ###                                  ########         ######       ######       ############      
                        ###   ###                                  #######          ######       #####         ##########       
                        ###   ###                                   ####              ##           ##            #####          
              ###       ###   ###      ###                                                                                      
               #####    ###   ####   ####                                                                                       
                 #########     #########                                                                                        
                   #  ####     ####  #                                                                                          
                    #  ###     ###  #                                                                                           
                     #####     #####                                                                                            
                     #####     #####                                                                                            
                    #####       #####                                                                                           
                   ######       ######                                                                                          
                  #####            ####                                                                                         
                 ####               ####                                                                                        
                ####                 ####                                                                                       
               ####       #####       ####                                                                                      
             #####     ###########     #####                                                                                    
            #####    ###############    #####                                                                                   
           #####  #########  ##########  #####                                                                                  
          ##############  #  #   ##############                                                                                 
         #############    #  #     #############                                                                                
        ###########        ##         ###########                                                                               
       #########           ##            #########                                                                              
      ########             ##              ########                                                                             
     ######                ##                 ######                                                                            
    #####                  ##                   #####                                                                           
   ###                     ##                      ###                                                                          
                                                                                                                                
                                                                                                                                
                                                                                                                                
                                                                                                                                
                                                                                                                                
        # ##### ##### ####  ##### ###  ###      ##   #  #   ### ##### #### #   #      ##### #   # ##### ### ##### ###  ###      
        # #     #     #  ## #   # # ## # #       #  # # #   ###   #  ##    #   #      #      # #  #      #  #     # ## # #      
       ## ##### ##### #  ## #   # #  # # ##      #  # # #  ## #   #  ##    #####      ###    ###  ###    #  ##### #  # # ##     
       ## ##    ##    #  ## #   # ## # #  #      ## # # # ##  ##  #  ##    #####        ###   #     ###  #  ##    ## # #  #     
       ## ##    ##    #  ## #  ## ## ###  #       ### ### ##  ##  #  ###   ##  #         ##   #      ##  #  ##    ## ###  #     
    ##### ##### ##### ####  ##### ##  ##  #       ##   ## ##   #  #   #### ##  #      #####   #   #####  #  ##### ##  ##  #     
*/

// Barres de puissance du signal Wifi 13x16 px
const unsigned char bitmap_Off[] PROGMEM = {
  0x00,0x00, // ................
  0x00,0x00, // ................
  0x00,0x00, // ................
  0x00,0x00, // ................
  0x00,0x00, // ................
  0x00,0x00, // ................
  0x00,0x00, // ................
  0x00,0x00, // ................
  0x00,0x00, // ................
  0x00,0x00, // ................
  0x00,0x00, // ................
  0x00,0x00, // ................
  0x00,0x00, // ................
};

const unsigned char bitmap_Signal1[] PROGMEM = {
  0x00,0x00, // ................
  0x00,0x00, // ................
  0x00,0x00, // ................
  0x00,0x00, // ................
  0x00,0x00, // ................
  0x00,0x00, // ................
  0x00,0x00, // ................
  0x00,0x00, // ................
  0x00,0x00, // ................
  0x00,0x00, // ................
  0x40,0x00, // .#..............
  0xC0,0x00, // ##..............
  0xC0,0x00  // ##..............
};

const unsigned char bitmap_Signal2[] PROGMEM = {
  0x00,0x00, // ................
  0x00,0x00, // ................
  0x00,0x00, // ................
  0x00,0x00, // ................
  0x00,0x00, // ................
  0x00,0x00, // ................
  0x00,0x00, // ................
  0x04,0x00, // .....#..........
  0x0C,0x00, // ....##..........
  0x1C,0x00, // ...###..........
  0x5C,0x00, // .#.###..........
  0xDC,0x00, // ##.###..........
  0xDC,0x00  // ##.###..........
};

const unsigned char bitmap_Signal3[] PROGMEM = {
  0x00,0x00, // ................
  0x00,0x00, // ................
  0x00,0x00, // ................
  0x00,0x00, // ................
  0x00,0x40, // .........#......
  0x00,0xC0, // ........##......
  0x01,0xC0, // .......###......
  0x05,0xC0, // .....#.###......
  0x0D,0xC0, // ....##.###......
  0x1D,0xC0, // ...###.###......
  0x5D,0xC0, // .#.###.###......
  0xDD,0xC0, // ##.###.###......
  0xDD,0xC0  // ##.###.###......
};

const unsigned char bitmap_Signal4[] PROGMEM = {
  0x00,0x04, // .............#..
  0x00,0x0C, // ............##..
  0x00,0x1C, // ...........###..
  0x00,0x1C, // ...........###..
  0x00,0x5C, // .........#.###..
  0x00,0xDC, // ........##.###..
  0x01,0xDC, // .......###.###..
  0x05,0xDC, // .....#.###.###..
  0x0D,0xDC, // ....##.###.###..
  0x1D,0xDC, // ...###.###.###..
  0x5D,0xDC, // .#.###.###.###..
  0xDD,0xDC, // ##.###.###.###..
  0xDD,0xDC  // ##.###.###.###..
};

// ----------------------------------------------------------------------
// Intinialisation de l'écran OLED
// ----------------------------------------------------------------------

void InitOLED(void *pvParameters){
  
  Wire.begin(SDA_PIN, SCL_PIN);
  //Force le RESET de l'écran si définition d'un GPIO pour le RESET
  if(OLED_RESET != -1) {
    pinMode (OLED_RESET, OUTPUT);
    digitalWrite(OLED_RESET, HIGH);
    digitalWrite(OLED_RESET, LOW);
    vTaskDelay(250 / portTICK_PERIOD_MS);// Attente du réveil de l'écran
  }

  #if USE_SSD1306                      // Pour écran SSD1306
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    }
    else {
        display.ssd1306_command(SSD1306_DISPLAYON);
        OLED_on=true;
    }
  #endif
  
  #if USE_SH110X                       // Pour écran SH110X   
    if (!display.begin(SCREEN_ADDRESS, true)) { 
    }
    else {
      OLED_on=true;
    }
  #endif

  if (OLED_on){
    display.setRotation(2);            // Orientation de l'écran : 0 = 0° (défaut), 1 = 90°, 2 = 180°, 3 = 270°
    display.clearDisplay();
    display.setCursor(0,0);
    display.drawBitmap(0, 0, Logo_JWS, 128, 68, OLED_WHITE); // Affichage du logo à l'initialisation de l'écran OLED
    display.display();

    while (init_progress != 9) {       // Affiche la progression de l'initialisation en cours jusqu'à la dernière étape
         drawProgressBar(init_progress);
    }
  }
  vTaskDelete(NULL);                   // Supprime la tâche une fois exécutée
}

// ----------------------------------------------------------------------
// Affichage de la barre de progression
// ----------------------------------------------------------------------

void drawProgressBar(int i) {
  int barWidth = 58;                   // Longueur de la barre de progression
  int barHeight = 5;                   // Hauteur
  int x = 66;                          // Position X
  int y = 35;                          // Position Y
  #define x_lib 66                     // Position X des libéllés des phases d'init
  #define y_lib 42                     // Position Y des libéllés des phases d'init
  int pourcentage = 0;                 // Progression en %
  
  if (i == 8){
    pourcentage = 100;                 // Dernière étape, on force l'échelle à 100%
    } 
   else {
    pourcentage = i*12;                // Progression en % (8 étapes, soit 12 % par étape)
    }

  display.drawRect(x, y, barWidth, barHeight, OLED_WHITE); // Dessine le contour de la barre de progression
  int filledWidth = (barWidth * pourcentage) / 100; // Calcul de la longueur de la barre de progression en fonction du
                                                    // pourcentage
  display.fillRect(x, y, filledWidth, barHeight, OLED_WHITE); // Affiche la barre de progression
  display.fillRect(x, y+(y_lib-y), barWidth, 8, OLED_BLACK);  // Efface la ligne des libéllés pour le libéllé suivant
  display.display();

  // Affiche le libéllé au fur et à mesure des phases d'init terminées sous la barre de progression
  display.setTextSize(1);
  display.setCursor(x_lib, y_lib);
  if (i == 8) {
      display.fillRect(x, y, barWidth, 8, OLED_WHITE);  // En fin de phase d'init, affiche "GO" sur une barre blanche
      display.setCursor((x+((barWidth/2)-6)), y+1);     // Centre le dernier libéllé ("GO") 
      #if(USE_SSD1306)
          display.setTextColor(SSD1306_BLACK);
      #endif
      #if(USE_SH110X)
          display.setTextColor(SH110X_BLACK);
      #endif
  }
    else 
  {
      display.setTextColor(OLED_WHITE);
  }
  display.print(steps[i]);
  display.display();
}

// ----------------------------------------------------------------------
// Affichage lors de RESET de la box JEEDOM
// ----------------------------------------------------------------------

void Affichage_RESET(){

if(!OLED_on) {return;}

  display.clearDisplay();
  display.setTextSize(2);
  display.drawLine(0,21,128,21,OLED_WHITE);
  display.drawLine(0,41,128,41,OLED_WHITE);
  display.fillRect(0,23,128,17,OLED_WHITE);
  #if(USE_SSD1306)
   display.setTextColor(SSD1306_BLACK);
  #endif
  #if(USE_SH110X)
   display.setTextColor(SH110X_BLACK);
  #endif
  display.setCursor(4,24);
  display.print(F("RESET SYST"));
  display.display();
  display.setTextColor(OLED_WHITE);
}

// ----------------------------------------------------------------------
// Affichage lors d'une mise à jour par OTA
// ----------------------------------------------------------------------

void Affichage_OTA(){

if(!OLED_on) {return;}

  display.clearDisplay();
  display.setTextSize(2);
  display.drawLine(0,17,128,17,OLED_WHITE);
  display.drawLine(0,37,128,37,OLED_WHITE);
  display.fillRect(0,19,128,17,OLED_WHITE);
  #if(USE_SSD1306)
   display.setTextColor(SSD1306_BLACK);
  #endif
  #if(USE_SH110X)
   display.setTextColor(SH110X_BLACK);
  #endif
  display.setCursor(4,20);
  display.print(F("OTA UPDATE"));
  display.setTextColor(OLED_WHITE);
  display.setTextSize(1);
  display.setCursor(3,39);
  display.print(F("Mise a jour en cours"));
  if(erreur_OTA){
   display.setCursor(21,47);
   display.print(F("=== Erreur ==="));   
  }
  display.display();
}

// ----------------------------------------------------------------------
// Affichage des informations
// ----------------------------------------------------------------------

void OledAffiche(){

if(!OLED_on) {return;}  
display.clearDisplay();
display.display();
if (Avarie!=0) {
  Ecran_generique();
  Ecran_resultats();
  }
}

// ----------------------------------------------------------------------
// Affichage des informations génériques
// ----------------------------------------------------------------------

void Ecran_generique(){
  
  // Défini l'affichage des informations génériques présentées sur la première et dernière ligne.
  if(!OLED_on) {return;}   

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(OLED_WHITE);

  // Lignes de séparation haute et basse
  display.drawLine(0,15,128,15,OLED_WHITE);
  display.drawLine(0,52,128,52,OLED_WHITE);
  
  // Wifi - Affichage de l'@IP de la box surveillée (1ère ligne)
  display.setCursor(0,4);
  display.print(jeedom_server);
  //display.print(WiFi.localIP().toString());
  
  // Wifi - Niveau de connection (1ère ligne)
  if (WiFi.status() == WL_CONNECTED) {
    int NivSignal = int(WiFi.RSSI()) ;
         if(NivSignal<-80) {display.drawBitmap(112, 0, bitmap_Signal1 , 16, 13, OLED_WHITE); }
    else if(NivSignal<-60) {display.drawBitmap(112, 0, bitmap_Signal2 , 16, 13, OLED_WHITE); }
    else if(NivSignal<-45) {display.drawBitmap(112, 0, bitmap_Signal3 , 16, 13, OLED_WHITE); }
    else if(NivSignal<-30) {display.drawBitmap(112, 0, bitmap_Signal4 , 16, 13, OLED_WHITE); }
  }
  else {
    (WiFi.getMode() == WIFI_STA)==false ? display.print(WiFi.softAPIP().toString()) :  display.print(F("Wifi HS")); 
    display.drawBitmap(112, 0, bitmap_Off, 16, 13, OLED_WHITE);
  }

  // Affichage du numéro de version (dernière ligne)
  display.setCursor(0,54);
  display.print(F("v"));
  String Version_routeur = Version;
  display.setCursor(6,54);
  display.print(Version_routeur);

  //Affichage de la durée de fonctionnement de l'ESP32 (dernière ligne)
  int T = int(millis() / 1000);
  float DureeOn = float(T) / 3600;
  String durOn=String(DureeOn)+" h";
  display.setCursor(128-(durOn.length()*6),54);
  display.print(durOn);
}

// ----------------------------------------------------------------------
// Affichage des résultats
// ----------------------------------------------------------------------

void Ecran_resultats(){

  if(!OLED_on) {return;}  

  // Affichage des avaries

  display.setCursor(0,21); 
  display.print(F("Cnx IP")); 
  display.setCursor(60,21); 
  display.setTextColor(OLED_WHITE);

  if(Avarie!=1 && Avarie!=3 && Avarie!=5 && Avarie!=7){
    display.print(F("OK"));            // Connexion IP - OK
  }
  else {
    display.fillRect(59,20,13,9,OLED_WHITE); 
    #if(USE_SSD1306)
      display.setTextColor(SSD1306_BLACK);
    #endif
    #if(USE_SH110X)
      display.setTextColor(SH110X_BLACK);
    #endif
    display.print(F("KO"));
    display.setTextColor(OLED_WHITE);
  }
  if (compteur_echecIP > 9){
     display.setCursor(78,21);}
  else {
     display.setCursor(84,21);}  
  display.print(compteur_echecIP); 

  display.setCursor(0,31);  
  display.print(F("Cnx MQTT")); 
  display.setCursor(60,31); 
  display.setTextColor(OLED_WHITE);
  if(Avarie!=2 && Avarie!=3 && Avarie!=6 && Avarie!=7){
    display.print(F("OK"));            // Connexion MQTT - OK
  }
  else {
    display.fillRect(59,30,13,9,OLED_WHITE);
    #if(USE_SSD1306)
      display.setTextColor(SSD1306_BLACK);
    #endif
    #if(USE_SH110X)
      display.setTextColor(SH110X_BLACK);
    #endif
    display.print(F("KO"));
    display.setTextColor(OLED_WHITE);
  }
    if (compteur_echecMQTT > 9){
     display.setCursor(78,31);}
  else {
     display.setCursor(84,31);}  
  display.print(compteur_echecMQTT); 
  
  display.setCursor(0,41);
  display.print(F("Msg MQTT")); 
  display.setCursor(60,41);
  display.setTextColor(OLED_WHITE);
  if(Avarie!=4 && Avarie!=5 && Avarie!=6 && Avarie!=7){
    display.print(F("OK"));            // Echange de messages MQTT - OK
  }
  else {
    display.fillRect(59,40,13,9,OLED_WHITE);
    #if(USE_SSD1306)
       display.setTextColor(SSD1306_BLACK);
    #endif
    #if(USE_SH110X)
       display.setTextColor(SH110X_BLACK);
    #endif
    display.print(F("KO"));
    display.setTextColor(OLED_WHITE);
  }
  if (compteur_echecExRx > 9){
     display.setCursor(78,41);}
  else {
     display.setCursor(84,41);}  
  display.print(compteur_echecExRx); 

  display.drawLine(96,15,96,52,OLED_WHITE);
  
  display.setTextSize(2);
  display.setCursor(108,26);
  if (compteur_relais == 0) {
      #if(USE_SSD1306)
        display.setTextColor(SSD1306_BLACK);
      #endif
      #if(USE_SH110X)
        display.setTextColor(SH110X_BLACK);
      #endif
    display.fillRect(96,15,32,37,OLED_WHITE);
  }
  display.print(compteur_relais);      // Nombre restants possibles d'activation du relais
  display.display();
}
