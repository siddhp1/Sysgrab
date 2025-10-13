#include <stdio.h>

#include "log.h"
#include "sysgrab/art.h"

int main(void) {
  FILE *log_fp = fopen("sysgrab.log", "w");
  if (log_fp != nullptr) {
    log_add_fp(log_fp, LOG_TRACE);
  }
  
  //log_set_quiet(true);
  //log_error("Test");

  FILE* fp = OpenArtFile("art.txt");
  struct Art* art = ReadArtFile(fp);
  CloseArtFile(fp);
  PrintArt(art);
  FreeArt(art);

  if (log_fp) {
    fclose(log_fp);
  }
}
