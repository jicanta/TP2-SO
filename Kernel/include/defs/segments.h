#ifndef SEGMENTS_H
#define SEGMENTS_H

/* Flags para derechos de acceso de los segmentos */
#define ACS_PRESENT 0x80          /* segmento presente en memoria */
#define ACS_CSEG  0x18            /* segmento de codigo */ // Es de codigo pues S = 1 y E = 1
#define ACS_DSEG  0x10            /* segmento de datos */  // E = 0, no ejecutable
#define ACS_READ  0x02            /* segmento de lectura */
#define ACS_WRITE 0x02            /* segmento de escritura */

#define ACS_IDT         ACS_DSEG

#define ACS_INT_386 0X0E /* Interrupt GATE 32 bits */
#define ACS_INT (ACS_PRESENT | ACS_INT_386) 

#define ACS_CODE        (ACS_PRESENT | ACS_CSEG | ACS_READ)
#define ACS_DATA        (ACS_PRESENT | ACS_DSEG | ACS_WRITE)
#define ACS_STACK       (ACS_PRESENT | ACS_DSEG | ACS_WRITE)

#endif 