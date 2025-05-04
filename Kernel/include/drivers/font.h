
#ifndef FONT_H
#define FONT_H

#include <stdint.h>
#include <stddef.h>

/*
*
*   LIBRERÍA FONT
*
*   Contenido: 
*   
*   - Funciones para obtener información sobre la fuente.
*
*
*/


/**
 * @brief getter del ancho de la fuente en pixeles.
 */
uint8_t getFontWidth();

/**
 * @brief getter del alto de la fuente en pixeles.
 */
uint8_t getFontHeight();

/**
 * @brief obtención del bitmap correspondiente a un determinado caracter. 
 * 
 * @param c caracter a representar.
 * 
 * @return bitmap correspondiente o NULL si no existe.
 */
uint8_t * getFontChar(char c);

#endif