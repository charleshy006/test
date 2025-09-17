#!/bin/bash
ctags -R -h ".c.cpp.h" --c++-kinds=+p --fields=+ialS --extra=+q -o tags
