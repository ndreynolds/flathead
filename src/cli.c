/*
 * cli.c -- Utilities for the command line interface.
 *
 * Copyright (c) 2012-2013 Nick Reynolds
 *  
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *  
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <unistd.h>

#include "flathead.h"
#include "cli.h"


void
fh_print_help()
{
  printf("Usage: flat [options] [script.js] \n"
         "\n"
         "Options:\n"
         "  -v, --version       print version info\n"
         "  -h, --help          print this help text\n"
         "  -i, --interactive   force REPL\n"
         "  -n, --nodes         print the AST\n"
         "  -t, --tokens        print tokens\n");
}

void
fh_print_startup()
{
  printf(ANSI_BLUE);
  printf("   ______     __  __               __    \n"
         "  / __/ /__ _/ /_/ /  ___ ___ ____/ /    \n"
         " / _// / _ `/ __/ _ \\/ -_) _ `/ _  /    \n"
         "/_/ /_/\\_,_/\\__/_//_/\\__/\\_,_/\\_,_/ \n");
  printf(ANSI_RESET);
  printf("\nVersion %s (<ctrl-d> to quit)          \n", FH_VERSION);
}

void
fh_print_version()
{
  printf("%s\n", FH_VERSION);
}

void
cfprintf(FILE *stream, const char *color, const char *tpl, ...)
{
  if (fh->opt_interactive) fputs(color, stream);

  va_list ap;
  va_start(ap, tpl);
  vfprintf(stream, tpl, ap);
  va_end(ap);

  if (fh->opt_interactive) fputs(ANSI_RESET, stream);
}
