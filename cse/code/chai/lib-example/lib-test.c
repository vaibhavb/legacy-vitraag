#include "main.h"

main (int argc, char ** argv) 
{
  Mocha_Main();
  Mocha_Eval("read_module zeros.rm\n");
  printf("hello\n");
  
  Mocha_Eval("sym_search Zeros\n");
}


