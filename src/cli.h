/*
 * cli.h -- Utilities for the command line interface.
 *
 * Copyright (c) 2012 Nick Reynolds
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

#ifndef CLI_H
#define CLI_H

#include "flathead.h"
#include "unistd.h"

#define ANSI_YELLOW  "\033[32m"
#define ANSI_ORANGE  "\033[33m"
#define ANSI_BLUE    "\033[34m"
#define ANSI_PINK    "\033[35m"
#define ANSI_GRAY    "\033[32;1m"
#define ANSI_WHITE   "\033[37;1m"
#define ANSI_RESET   "\033[0m"

void print_help(void);
void print_startup(void);
void print_version(void);
void cfprintf(FILE *, const char *, const char *, ...);

#endif
