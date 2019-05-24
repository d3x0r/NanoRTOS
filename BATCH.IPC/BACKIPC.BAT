pkzip -P -r -u -x*.zip -x*.com -x*.bak  m:\ipcfile m:\nipc\*.*
pkzip -P -r -u -x*.zip -x*.com -x*.bak  m:\ipcfile m:\common.ipc\*.*
pkzip -P -r -u -x*.zip -x*.com -x*.bak  m:\ipcfile m:\batch.ipc\*.*
copy m:\ipcfile.zip %1
