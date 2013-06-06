//exemplo:
//printErrorAndExit("teste erro!", __FILE__, __LINE__);

#pragma once


void printErrorAndExit(const char* erro, const char* file, int line);
void printWarning(const char* erro, const char* file, int line);

