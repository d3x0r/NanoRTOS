typedef char (far *Data_process)(Node far *node,short far *bytes_left);

#ifndef IOROUT
#define declare(name) extern Data_process name;
#else
#define declare(name)
#endif

declare(Get_Rewind);

declare(Get_Back_File);
declare(Get_Back_Record);
declare(Get_Adv_Record);
declare(Get_Adv_File);
declare(Get_EOF);
declare(Get_Home);

declare(Get_Data_SA);
declare(Get_Data_NSA);
declare(Get_Data_SB);
declare(Get_Data_NSB);

declare(Put_Data_SA);
declare(Put_Data_NSA);
declare(Put_Data_SB);
declare(Put_Data_NSB);
declare(Put_Data_TSA);
declare(Put_Data_TSB);
declare(Put_Data_DSA);
declare(Put_Data_DNSA);
declare(Put_Data_DSB);
declare(Put_Data_DNSB);
declare(Put_Data_DTSA);
declare(Put_Data_DTSB);

