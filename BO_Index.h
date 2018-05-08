#ifndef __BO_INDEX_H__
#define __BO_INDEX_H__
char * Detect_BO(FILE * fptr,char * rdbuf);

int Get_BO_Index(const char * bo,BO_Unit_t *boptr);

int Get_SG_Index(FILE * fptr,char * rdbuf,BO_Unit_t *boptr);

int Free_BO_(int BO_Number);

int Dbc_load(FILE * dbc_cfg_fd,char *dbc_cfg_rdbuf,BO_Unit_t **BO_List);
#endif // BO_INDEX_H_INCLUDED
