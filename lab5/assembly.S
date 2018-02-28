#include "i8042.h"
.global _assembly_handler
.text

_assembly_handler:

           mov $0, %eax

 CICLE:
           inb $STAT_REG, %al //almost the same as sys_inb
           test $OBF, %al  //zf goes to 0 if OBF != 0
           jz CICLE
           jmp END

   END:
           inb $OUT_BUF, %al

   ret
