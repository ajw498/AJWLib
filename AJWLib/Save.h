
typedef BOOL (*save_handler)(char *filename,BOOL safe,BOOL selection,void *reference);

void Save_ChangeDefaultFilename(char *newname);

void Save_ChangeFileType(int newfiletype);
/*Not yet implemented fully*/

void Save_ChangeEstimatedSize(int newsize);

extern window_handle Save_CreateWindow(int type,BOOL selectionbutton,int estsize,BOOL menuleaf,save_handler FileSave,save_handler RAMSave,void *reference);

