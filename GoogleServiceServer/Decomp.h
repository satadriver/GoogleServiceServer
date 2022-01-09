#pragma once



int DecompressFromFileBlock(char * filename);
int DecryptAndDecompFile(LPDATAPROCESS_PARAM lpparam, char * filename, char * lpdstfn, char * imei, char * username, int withparam, int append, DWORD type);
int NoneDecryptDataWiHeader(LPDATAPROCESS_PARAM lpparam, char * filename, char * lpdstfn, char * imei, char * username, int withparam, int append);
