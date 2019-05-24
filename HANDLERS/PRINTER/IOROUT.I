iorout.c 1: 
iorout.c 2: 
\common.ipc\mod.h 1: 
m:\common.ipc\os.h 1: 
m:\common.ipc\os.h 2: 
m:\common.ipc\os.h 3: 
m:\common.ipc\os.h 4: 
m:\common.ipc\os.h 5: 
m:\common.ipc\os.h 6: 
m:\common.ipc\os.h 7: 
m:\common.ipc\os.h 8: 
m:\common.ipc\os.h 9: 
m:\common.ipc\os.h 10: 
m:\common.ipc\os.h 11: 
m:\common.ipc\os.h 12: 
m:\common.ipc\os.h 13: 
m:\common.ipc\os.h 14: 
m:\common.ipc\os.h 15: 
m:\common.ipc\os.h 16: 
m:\common.ipc\os.h 17: 
m:\common.ipc\os.h 18: 
m:\common.ipc\os.h 19: 
m:\common.ipc\os.h 20: 
m:\common.ipc\os.h 21: 
m:\common.ipc\os.h 22: 
m:\common.ipc\os.h 23: 
m:\common.ipc\os.h 24: 
m:\common.ipc\os.h 25: 
m:\common.ipc\os.h 26: 
m:\common.ipc\os.h 27: 
m:\common.ipc\os.h 28: 
m:\common.ipc\os.h 29: 
m:\common.ipc\os.h 30: typedef (__far *proc)();
m:\common.ipc\os.h 31: 
m:\common.ipc\os.h 32: typedef long time_struc;
m:\common.ipc\os.h 33: typedef struct routine_ptr
m:\common.ipc\os.h 34: {
m:\common.ipc\os.h 35: char name[16];
m:\common.ipc\os.h 36: proc procedure;
m:\common.ipc\os.h 37: struct routine_ptr __far *next;
m:\common.ipc\os.h 38: }routine_ptr;
m:\common.ipc\os.h 39: 
m:\common.ipc\os.h 40: typedef struct environ_entry
m:\common.ipc\os.h 41: {
m:\common.ipc\os.h 42: char name[12];
m:\common.ipc\os.h 43: char parameter[30];
m:\common.ipc\os.h 44: struct environ_entry __far *next;
m:\common.ipc\os.h 45: }environ_entry;
m:\common.ipc\os.h 46: 
m:\common.ipc\os.h 47: typedef struct module
m:\common.ipc\os.h 48: {
m:\common.ipc\os.h 49: char name[8];                 
m:\common.ipc\os.h 50: struct module __far *child;           
m:\common.ipc\os.h 51: struct module __far *elder_sib;       
m:\common.ipc\os.h 52: struct module __far *parent;          
m:\common.ipc\os.h 53: struct module __far *next;            
m:\common.ipc\os.h 54: struct module __far *prior;           
m:\common.ipc\os.h 55: short __far *stack;                   
m:\common.ipc\os.h 56: unsigned char __far *code;            
m:\common.ipc\os.h 57: unsigned short size;                
m:\common.ipc\os.h 58: unsigned short stack_size;          
m:\common.ipc\os.h 59: unsigned short max_size;            
m:\common.ipc\os.h 60: unsigned long status;               
m:\common.ipc\os.h 61: unsigned short stackseg,stackofs;   
m:\common.ipc\os.h 62: unsigned long Acumulated_time;      
m:\common.ipc\os.h 63: unsigned char Priority;             
m:\common.ipc\os.h 64: unsigned char options;              
m:\common.ipc\os.h 65: environ_entry __far *environ;         
m:\common.ipc\os.h 66: char __far *device_name;              
m:\common.ipc\os.h 67: 
m:\common.ipc\os.h 68: } module;
m:\common.ipc\os.h 69: 
m:\common.ipc\os.h 70: typedef struct node_entry
m:\common.ipc\os.h 71: {
m:\common.ipc\os.h 72: short status;
m:\common.ipc\os.h 73: unsigned char __far *data;
m:\common.ipc\os.h 74: module __far *source;  
m:\common.ipc\os.h 75: 
m:\common.ipc\os.h 76: 
m:\common.ipc\os.h 77: 
m:\common.ipc\os.h 78: 
m:\common.ipc\os.h 79: 
m:\common.ipc\os.h 80: struct node_entry __far *next,__far *last;
m:\common.ipc\os.h 81: } node_entry;
m:\common.ipc\os.h 82: 
m:\common.ipc\os.h 83: typedef struct device_tree
m:\common.ipc\os.h 84: {
m:\common.ipc\os.h 85: struct device_tree __far *less,__far *more;
m:\common.ipc\os.h 86: module __far *Destination_Task;
m:\common.ipc\os.h 87: char dest[12];
m:\common.ipc\os.h 88: node_entry __far *node;
m:\common.ipc\os.h 89: } device_tree;
m:\common.ipc\os.h 90: 
m:\common.ipc\os.h 91: 
\common.ipc\mod.h 2: 
\common.ipc\mod.h 3: #pragma warn -rvl
\common.ipc\mod.h 4: extern long lmult(long num1,long num2);
\common.ipc\mod.h 5: extern long ldiv(long number,long divisor);
\common.ipc\mod.h 6: extern long lmod(long number,long divisor);
\common.ipc\mod.h 7: extern long Request;
\common.ipc\mod.h 8: 
\common.ipc\mod.h 9: 
\common.ipc\mod.h 10: 
\common.ipc\mod.h 11: 
\common.ipc\mod.h 12: 
\common.ipc\mod.h 13: 
\common.ipc\mod.h 14: 
\common.ipc\mod.h 15: 
\common.ipc\mod.h 16: 
\common.ipc\mod.h 17: 
\common.ipc\mod.h 18: 
\common.ipc\mod.h 19: 
\common.ipc\mod.h 20: 
\common.ipc\mod.h 21: 
\common.ipc\mod.h 22: 
\common.ipc\mod.h 23: extern module __far *__far *my_tcbptr;
\common.ipc\mod.h 24: 
\common.ipc\mod.h 25: 
\common.ipc\mod.h 26: extern void (__far *swap_to)(module __far *next_to_run);
\common.ipc\mod.h 27: extern void (__far *delay)(unsigned short delaytime);
\common.ipc\mod.h 28: extern void (__far *change_priority)(char new_priority);
\common.ipc\mod.h 29: extern void (__far *Exit)(short code);
\common.ipc\mod.h 30: extern void (__far *Relinquish)(long param);
\common.ipc\mod.h 31: extern short (__far *Export)(char __far*destination,
\common.ipc\mod.h 32: void __far *data);
\common.ipc\mod.h 33: extern void __far *(__far *Import)(char __far*WhoIAm);
\common.ipc\mod.h 34: extern void __far *(__far *Terminate)(char __far *whose,char pos,
\common.ipc\mod.h 35: char cnt,short __far *inprogress,...);
\common.ipc\mod.h 36: extern void (__far *Wake)(short mask,short spec);
\common.ipc\mod.h 37: extern short (__far *spawn_batch)(char __far *batch_name);
\common.ipc\mod.h 38: extern short (__far *spawn)(char __far *name,short stack,char __far *dev_name,
\common.ipc\mod.h 39: short threads,char priority);
\common.ipc\mod.h 40: extern char __far *(__far *Get_environ)(char __far *environ_name);
\common.ipc\mod.h 41: extern void __far *(__far *Allocate)(short length);
\common.ipc\mod.h 42: extern void (__far *Free)(void __far *pointer);
\common.ipc\mod.h 43: extern void (__far *Connect_Int)(void __far *routine,short Int_Num);
\common.ipc\mod.h 44: extern void (__far *Disconnect_Int)(void __far *routine,short Int_Num);
\common.ipc\mod.h 45: extern void (__far *gettime)(time_struc __far *time);
\common.ipc\mod.h 46: extern short __far *process_control;
\common.ipc\mod.h 47: extern short dataseg;
\common.ipc\mod.h 48: 
\common.ipc\mod.h 49: 
\common.ipc\mod.h 50: 
\common.ipc\mod.h 51: extern void perish(void);
\common.ipc\mod.h 52: extern short fork(char relation);
\common.ipc\mod.h 53: extern void destory(void);
\common.ipc\mod.h 54: extern void __far *(__far *Inquire_begin)(short what);
\common.ipc\mod.h 55: 
\common.ipc\mod.h 56: 
\common.ipc\mod.h 57: 
\common.ipc\mod.h 58: 
\common.ipc\mod.h 59: 
\common.ipc\mod.h 60: 
\common.ipc\mod.h 61: 
\common.ipc\mod.h 62: 
\common.ipc\mod.h 63: 
\common.ipc\mod.h 64: 
\common.ipc\mod.h 65: 
\common.ipc\mod.h 66: 
\common.ipc\mod.h 67: 
\common.ipc\mod.h 68: 
\common.ipc\mod.h 69: 
\common.ipc\mod.h 70: 
\common.ipc\mod.h 71: 
\common.ipc\mod.h 72: 
\common.ipc\mod.h 73: 
\common.ipc\mod.h 74: 
\common.ipc\mod.h 75: 
\common.ipc\mod.h 76: 
\common.ipc\mod.h 77: 
\common.ipc\mod.h 78: 
\common.ipc\mod.h 79: 
\common.ipc\mod.h 80: 
\common.ipc\mod.h 81: 
\common.ipc\mod.h 82: 
\common.ipc\mod.h 83: 
\common.ipc\mod.h 84: 
\common.ipc\mod.h 85: 
\common.ipc\mod.h 86: 
\common.ipc\mod.h 87: 
\common.ipc\mod.h 88: 
\common.ipc\mod.h 89: 
\common.ipc\mod.h 90: 
\common.ipc\mod.h 91: 
\common.ipc\mod.h 92: 
\common.ipc\mod.h 93: 
\common.ipc\mod.h 94: 
\common.ipc\mod.h 95: 
\common.ipc\mod.h 96: 
\common.ipc\mod.h 97: 
\common.ipc\mod.h 98: 
\common.ipc\mod.h 99: 
\common.ipc\mod.h 100: 
\common.ipc\mod.h 101: 
\common.ipc\mod.h 102: 
\common.ipc\mod.h 103: 
\common.ipc\mod.h 104: 
\common.ipc\mod.h 105: 
\common.ipc\mod.h 106: 
\common.ipc\mod.h 107: 
\common.ipc\mod.h 108: 
\common.ipc\mod.h 109: 
\common.ipc\mod.h 110: 
\common.ipc\mod.h 111: 
\common.ipc\mod.h 112: 
\common.ipc\mod.h 113: 
\common.ipc\mod.h 114: 
\common.ipc\mod.h 115: 
\common.ipc\mod.h 116: 
\common.ipc\mod.h 117: 
\common.ipc\mod.h 118: 
\common.ipc\mod.h 119: 
\common.ipc\mod.h 120: 
\common.ipc\mod.h 121: 
\common.ipc\mod.h 122: 
\common.ipc\mod.h 123: 
\common.ipc\mod.h 124: 
\common.ipc\mod.h 125: 
\common.ipc\mod.h 126: 
\common.ipc\mod.h 127: 
\common.ipc\mod.h 128: 
\common.ipc\mod.h 129: 
\common.ipc\mod.h 130: 
\common.ipc\mod.h 131: 
\common.ipc\mod.h 132: 
\common.ipc\mod.h 133: 
\common.ipc\mod.h 134: 
\common.ipc\mod.h 135: 
\common.ipc\mod.h 136: 
\common.ipc\mod.h 137: 
\common.ipc\mod.h 138: 
\common.ipc\mod.h 139: 
\common.ipc\mod.h 140: 
\common.ipc\mod.h 141: 
\common.ipc\mod.h 142: 
\common.ipc\mod.h 143: 
\common.ipc\mod.h 144: 
\common.ipc\mod.h 145: 
\common.ipc\mod.h 146: 
\common.ipc\mod.h 147: 
\common.ipc\mod.h 148: 
\common.ipc\mod.h 149: 
\common.ipc\mod.h 150: 
\common.ipc\mod.h 151: 
\common.ipc\mod.h 152: 
\common.ipc\mod.h 153: 
\common.ipc\mod.h 154: 
\common.ipc\mod.h 155: 
\common.ipc\mod.h 156: 
\common.ipc\mod.h 157: 
\common.ipc\mod.h 158: 
\common.ipc\mod.h 159: 
\common.ipc\mod.h 160: 
\common.ipc\mod.h 161: 
\common.ipc\mod.h 162: 
\common.ipc\mod.h 163: 
\common.ipc\mod.h 164: 
\common.ipc\mod.h 165: 
\common.ipc\mod.h 166: 
\common.ipc\mod.h 167: 
\common.ipc\mod.h 168: 
\common.ipc\mod.h 169: 
\common.ipc\mod.h 170: 
\common.ipc\mod.h 171: 
\common.ipc\mod.h 172: 
\common.ipc\mod.h 173: 
\common.ipc\mod.h 174: 
\common.ipc\mod.h 175: 
\common.ipc\mod.h 176: 
\common.ipc\mod.h 177: typedef short ether_type;
\common.ipc\mod.h 178: typedef ether_type __far *etherptr;
\common.ipc\mod.h 179: 
\common.ipc\mod.h 180: 
\common.ipc\mod.h 181: 
\common.ipc\mod.h 182: 
\common.ipc\mod.h 183: 
\common.ipc\mod.h 184: 
\common.ipc\mod.h 185: 
\common.ipc\mod.h 186: 
\common.ipc\mod.h 187: 
\common.ipc\mod.h 188: 
\common.ipc\mod.h 189: 
\common.ipc\mod.h 190: 
\common.ipc\mod.h 191: 
\common.ipc\mod.h 192: 
\common.ipc\mod.h 193: 
\common.ipc\mod.h 194: 
\common.ipc\mod.h 195: 
\common.ipc\mod.h 196: 
\common.ipc\mod.h 197: 
\common.ipc\mod.h 198: 
\common.ipc\mod.h 199: 
\common.ipc\mod.h 200: 
\common.ipc\mod.h 201: 
\common.ipc\mod.h 202: 
\common.ipc\mod.h 203: 
\common.ipc\mod.h 204: 
\common.ipc\mod.h 205: 
\common.ipc\mod.h 206: 
\common.ipc\mod.h 207: 
\common.ipc\mod.h 208: 
\common.ipc\mod.h 209: 
\common.ipc\mod.h 210: 
\common.ipc\mod.h 211: 
\common.ipc\mod.h 212: 
\common.ipc\mod.h 213: 
\common.ipc\mod.h 214: 
\common.ipc\mod.h 215: 
\common.ipc\mod.h 216: 
\common.ipc\mod.h 217: 
\common.ipc\mod.h 218: 
\common.ipc\mod.h 219: 
\common.ipc\mod.h 220: 
\common.ipc\mod.h 221: 
\common.ipc\mod.h 222: 
\common.ipc\mod.h 223: 
\common.ipc\mod.h 224: 
\common.ipc\mod.h 225: 
\common.ipc\mod.h 226: 
\common.ipc\mod.h 227: 
\common.ipc\mod.h 228: 
\common.ipc\mod.h 229: 
\common.ipc\mod.h 230: 
\common.ipc\mod.h 231: 
\common.ipc\mod.h 232: 
\common.ipc\mod.h 233: 
\common.ipc\mod.h 234: 
\common.ipc\mod.h 235: 
\common.ipc\mod.h 236: 
\common.ipc\mod.h 237: 
\common.ipc\mod.h 238: 
\common.ipc\mod.h 239: 
\common.ipc\mod.h 240: 
\common.ipc\mod.h 241: 
\common.ipc\mod.h 242: 
\common.ipc\mod.h 243: 
\common.ipc\mod.h 244: 
\common.ipc\mod.h 245: 
\common.ipc\mod.h 246: 
\common.ipc\mod.h 247: 
\common.ipc\mod.h 248: 
\common.ipc\mod.h 249: 
\common.ipc\mod.h 250: 
\common.ipc\mod.h 251: 
\common.ipc\mod.h 252: 
\common.ipc\mod.h 253: 
\common.ipc\mod.h 254: 
\common.ipc\mod.h 255: 
\common.ipc\mod.h 256: 
\common.ipc\mod.h 257: 
\common.ipc\mod.h 258: 
\common.ipc\mod.h 259: #pragma warn .rvl
\common.ipc\mod.h 260: 
\common.ipc\mod.h 261: 
\common.ipc\mod.h 262: 
iorout.c 3: 
\common.ipc\npca.h 1: 
m:\nipc\pca\npca.h 1: 
m:\nipc\pca\npca.h 2: 
m:\nipc\pca\npca.h 3: 
m:\nipc\pca\npca.h 4: 
m:\nipc\pca\npca.h 5: 
m:\nipc\pca\npca.h 6: 
m:\nipc\pca\npca.h 7: 
m:\nipc\pca\npca.h 8: 
m:\nipc\pca\npca.h 9: 
m:\nipc\pca\npca.h 10: 
m:\nipc\pca\npca.h 11: 
m:\nipc\pca\npca.h 12: 
m:\nipc\pca\npca.h 13: 
m:\nipc\pca\npca.h 14: 
m:\nipc\pca\npca.h 15: 
m:\nipc\pca\npca.h 16: 
m:\nipc\pca\npca.h 17: 
m:\nipc\pca\npca.h 18: 
m:\nipc\pca\npca.h 19: 
m:\nipc\pca\npca.h 20: 
m:\nipc\pca\npca.h 21: 
m:\nipc\pca\npca.h 22: 
m:\nipc\pca\npca.h 23: 
m:\nipc\pca\npca.h 24: 
m:\nipc\pca\npca.h 25: 
m:\nipc\pca\npca.h 26: 
m:\nipc\pca\npca.h 27: 
m:\nipc\pca\npca.h 28: 
m:\nipc\pca\npca.h 29: 
m:\nipc\pca\npca.h 30: 
m:\nipc\pca\npca.h 31: 
m:\nipc\pca\npca.h 32: 
m:\nipc\pca\npca.h 33: 
m:\nipc\pca\npca.h 34: 
m:\nipc\pca\npca.h 35: 
m:\nipc\pca\npca.h 36: 
m:\nipc\pca\npca.h 37: 
m:\nipc\pca\npca.h 38: 
m:\nipc\pca\npca.h 39: 
m:\nipc\pca\npca.h 40: 
m:\nipc\pca\npca.h 41: 
m:\nipc\pca\npca.h 42: 
m:\nipc\pca\npca.h 43: 
m:\nipc\pca\npca.h 44: 
m:\nipc\pca\npca.h 45: 
m:\nipc\pca\npca.h 46: 
m:\nipc\pca\npca.h 47: 
m:\nipc\pca\npca.h 48: 
m:\nipc\pca\npca.h 49: 
m:\nipc\pca\npca.h 50: 
m:\nipc\pca\npca.h 51: 
m:\nipc\pca\npca.h 52: 
m:\nipc\pca\npca.h 53: 
m:\nipc\pca\npca.h 54: 
m:\nipc\pca\npca.h 55: typedef struct Data_Track
m:\nipc\pca\npca.h 56: {
m:\nipc\pca\npca.h 57: char __far *Preamble;
m:\nipc\pca\npca.h 58: short index;
m:\nipc\pca\npca.h 59: }Data_Track;
m:\nipc\pca\npca.h 60: 
m:\nipc\pca\npca.h 61: typedef struct Etherdata
m:\nipc\pca\npca.h 62: {
m:\nipc\pca\npca.h 63: unsigned char SourceIP[4];
m:\nipc\pca\npca.h 64: unsigned char DestIP[4];
m:\nipc\pca\npca.h 65: unsigned short SourceTCP;
m:\nipc\pca\npca.h 66: unsigned short DestTCP;
m:\nipc\pca\npca.h 67: }Etherdata;
m:\nipc\pca\npca.h 68: 
m:\nipc\pca\npca.h 69: typedef struct Commdata
m:\nipc\pca\npca.h 70: {
m:\nipc\pca\npca.h 71: short filler1;
m:\nipc\pca\npca.h 72: }Commdata;
m:\nipc\pca\npca.h 73: 
m:\nipc\pca\npca.h 74: typedef struct NodeTrack
m:\nipc\pca\npca.h 75: {
m:\nipc\pca\npca.h 76: char Side;
m:\nipc\pca\npca.h 77: unsigned short Status;
m:\nipc\pca\npca.h 78: char __far *Source;
m:\nipc\pca\npca.h 79: struct Node __far *next,__far *prior;
m:\nipc\pca\npca.h 80: }NodeTrack;
m:\nipc\pca\npca.h 81: 
m:\nipc\pca\npca.h 82: typedef struct ReturnData
m:\nipc\pca\npca.h 83: {
m:\nipc\pca\npca.h 84: unsigned char  Opcode;
m:\nipc\pca\npca.h 85: unsigned short Status;
m:\nipc\pca\npca.h 86: unsigned long  Byte_count;
m:\nipc\pca\npca.h 87: unsigned short XStatus;
m:\nipc\pca\npca.h 88: unsigned short Extended_Size;
m:\nipc\pca\npca.h 89: unsigned short __far *Extended_mail;
m:\nipc\pca\npca.h 90: }ReturnData;
m:\nipc\pca\npca.h 91: 
m:\nipc\pca\npca.h 92: typedef struct NodeData
m:\nipc\pca\npca.h 93: {
m:\nipc\pca\npca.h 94: unsigned short Transport;
m:\nipc\pca\npca.h 95: unsigned short Channel;
m:\nipc\pca\npca.h 96: unsigned long  Node_Address;
m:\nipc\pca\npca.h 97: unsigned short Rex;
m:\nipc\pca\npca.h 98: unsigned short Options;
m:\nipc\pca\npca.h 99: unsigned long  Byte_count;
m:\nipc\pca\npca.h 100: unsigned short XOptions;
m:\nipc\pca\npca.h 101: unsigned long  FPI;
m:\nipc\pca\npca.h 102: union
m:\nipc\pca\npca.h 103: {
m:\nipc\pca\npca.h 104: Etherdata Ether;
m:\nipc\pca\npca.h 105: Commdata Comm;
m:\nipc\pca\npca.h 106: } Info;
m:\nipc\pca\npca.h 107: }NodeData;
m:\nipc\pca\npca.h 108: 
m:\nipc\pca\npca.h 109: typedef struct Node
m:\nipc\pca\npca.h 110: {
m:\nipc\pca\npca.h 111: NodeTrack Tracking;
m:\nipc\pca\npca.h 112: unsigned short __far *Data;
m:\nipc\pca\npca.h 113: ReturnData Return;
m:\nipc\pca\npca.h 114: NodeData Node;
m:\nipc\pca\npca.h 115: Data_Track Data_handle;
m:\nipc\pca\npca.h 116: }Node;
m:\nipc\pca\npca.h 117: 
m:\nipc\pca\npca.h 118: 
m:\nipc\pca\npca.h 119: 
m:\nipc\pca\npca.h 120: 
m:\nipc\pca\npca.h 121: 
m:\nipc\pca\npca.h 122: 
m:\nipc\pca\npca.h 123: 
m:\nipc\pca\npca.h 124: 
m:\nipc\pca\npca.h 125: 
m:\nipc\pca\npca.h 126: 
m:\nipc\pca\npca.h 127: 
m:\nipc\pca\npca.h 128: 
m:\nipc\pca\npca.h 129: 
m:\nipc\pca\npca.h 130: 
m:\nipc\pca\npca.h 131: 
m:\nipc\pca\npca.h 132: 
m:\nipc\pca\npca.h 133: 
m:\nipc\pca\npca.h 134: 
m:\nipc\pca\npca.h 135: 
m:\nipc\pca\npca.h 136: 
m:\nipc\pca\npca.h 137: 
m:\nipc\pca\npca.h 138: 
m:\nipc\pca\npca.h 139: 
m:\nipc\pca\npca.h 140: 
m:\nipc\pca\npca.h 141: 
m:\nipc\pca\npca.h 142: 
m:\nipc\pca\npca.h 143: 
m:\nipc\pca\npca.h 144: 
m:\nipc\pca\npca.h 145: 
m:\nipc\pca\npca.h 146: 
m:\nipc\pca\npca.h 147: 
m:\nipc\pca\npca.h 148: 
m:\nipc\pca\npca.h 149: 
m:\nipc\pca\npca.h 150: 
m:\nipc\pca\npca.h 151: 
m:\nipc\pca\npca.h 152: 
m:\nipc\pca\npca.h 153: 
m:\nipc\pca\npca.h 154: 
m:\nipc\pca\npca.h 155: 
m:\nipc\pca\npca.h 156: 
m:\nipc\pca\npca.h 157: 
m:\nipc\pca\npca.h 158: 
m:\nipc\pca\npca.h 159: 
m:\nipc\pca\npca.h 160: 
m:\nipc\pca\npca.h 161: 
m:\nipc\pca\npca.h 162: 
m:\nipc\pca\npca.h 163: 
m:\nipc\pca\npca.h 164: 
m:\nipc\pca\npca.h 165: 
m:\nipc\pca\npca.h 166: 
m:\nipc\pca\npca.h 167: 
m:\nipc\pca\npca.h 168: 
m:\nipc\pca\npca.h 169: 
m:\nipc\pca\npca.h 170: 
m:\nipc\pca\npca.h 171: 
m:\nipc\pca\npca.h 172: 
m:\nipc\pca\npca.h 173: 
m:\nipc\pca\npca.h 174: 
m:\nipc\pca\npca.h 175: 
m:\nipc\pca\npca.h 176: 
m:\nipc\pca\npca.h 177: 
m:\nipc\pca\npca.h 178: 
m:\nipc\pca\npca.h 179: 
m:\nipc\pca\npca.h 180: 
m:\nipc\pca\npca.h 181: 
m:\nipc\pca\npca.h 182: 
m:\nipc\pca\npca.h 183: 
m:\nipc\pca\npca.h 184: 
m:\nipc\pca\npca.h 185: 
m:\nipc\pca\npca.h 186: 
m:\nipc\pca\npca.h 187: 
m:\nipc\pca\npca.h 188: 
m:\nipc\pca\npca.h 189: 
m:\nipc\pca\npca.h 190: 
m:\nipc\pca\npca.h 191: 
m:\nipc\pca\npca.h 192: 
m:\nipc\pca\npca.h 193: 
m:\nipc\pca\npca.h 194: 
m:\nipc\pca\npca.h 195: 
m:\nipc\pca\npca.h 196: 
m:\nipc\pca\npca.h 197: 
m:\nipc\pca\npca.h 198: 
m:\nipc\pca\npca.h 199: 
m:\nipc\pca\npca.h 200: 
m:\nipc\pca\npca.h 201: 
m:\nipc\pca\npca.h 202: 
m:\nipc\pca\npca.h 203: 
m:\nipc\pca\npca.h 204: 
m:\nipc\pca\npca.h 205: 
m:\nipc\pca\npca.h 206: 
m:\nipc\pca\npca.h 207: 
m:\nipc\pca\npca.h 208: 
m:\nipc\pca\npca.h 209: 
m:\nipc\pca\npca.h 210: 
m:\nipc\pca\npca.h 211: 
m:\nipc\pca\npca.h 212: 
\common.ipc\npca.h 2: 
\common.ipc\npca.h 3: 
iorout.c 4: 
iorout.h 1: typedef char Data_process(Node __far *node,short __far *bytes_left);
iorout.h 2: 
iorout.h 3: 
iorout.h 4: 
iorout.h 5: 
iorout.h 6: 
iorout.h 7: 
iorout.h 8: 
iorout.h 9: ;
iorout.h 10: 
iorout.h 11: ;
iorout.h 12: ;
iorout.h 13: ;
iorout.h 14: ;
iorout.h 15: ;
iorout.h 16: ;
iorout.h 17: 
iorout.h 18: ;
iorout.h 19: ;
iorout.h 20: ;
iorout.h 21: ;
iorout.h 22: 
iorout.h 23: ;
iorout.h 24: ;
iorout.h 25: ;
iorout.h 26: ;
iorout.h 27: ;
iorout.h 28: ;
iorout.h 29: ;
iorout.h 30: ;
iorout.h 31: ;
iorout.h 32: ;
iorout.h 33: ;
iorout.h 34: ;
iorout.h 35: 
iorout.h 36: 
iorout.h 37: 
iorout.c 5: 
iorout.c 6: 
iorout.c 7: char Get_Rewindname[]="Get_Rewind"; char (__far Get_Rewind) (node __far *node,short __far *Bytes_left); void _Get_Rewind(void) { asm push ds; asm mov dx,offset Get_Rewind; asm mov si,offset Get_Rewindname; asm mov ax,cs; asm mov es,ax; asm mov ds,ax; asm mov ah,1; asm int 0x60; asm pop ds;}; char (__far Get_Rewind) (node __far *node,short __far *Bytes_left)
iorout.c 8: {
iorout.c 9: }
iorout.c 10: 
iorout.c 11: char Get_Back_Filename[]="Get_Back_File"; char (__far Get_Back_File) (node __far *node,short __far *Bytes_left); void _Get_Back_File(void) { asm push ds; asm mov dx,offset Get_Back_File; asm mov si,offset Get_Back_Filename; asm mov ax,cs; asm mov es,ax; asm mov ds,ax; asm mov ah,1; asm int 0x60; asm pop ds;}; char (__far Get_Back_File) (node __far *node,short __far *Bytes_left)
iorout.c 12: {
iorout.c 13: }
iorout.c 14: 
iorout.c 15: char Get_Back_Recordname[]="Get_Back_Record"; char (__far Get_Back_Record) (node __far *node,short __far *Bytes_left); void _Get_Back_Record(void) { asm push ds; asm mov dx,offset Get_Back_Record; asm mov si,offset Get_Back_Recordname; asm mov ax,cs; asm mov es,ax; asm mov ds,ax; asm mov ah,1; asm int 0x60; asm pop ds;}; char (__far Get_Back_Record) (node __far *node,short __far *Bytes_left)
iorout.c 16: {
iorout.c 17: }
iorout.c 18: 
iorout.c 19: char Get_Adv_Recordname[]="Get_Adv_Record"; char (__far Get_Adv_Record) (node __far *node,short __far *Bytes_left); void _Get_Adv_Record(void) { asm push ds; asm mov dx,offset Get_Adv_Record; asm mov si,offset Get_Adv_Recordname; asm mov ax,cs; asm mov es,ax; asm mov ds,ax; asm mov ah,1; asm int 0x60; asm pop ds;}; char (__far Get_Adv_Record) (node __far *node,short __far *Bytes_left)
iorout.c 20: {
iorout.c 21: }
iorout.c 22: 
iorout.c 23: char Get_Adv_Filename[]="Get_Adv_File"; char (__far Get_Adv_File) (node __far *node,short __far *Bytes_left); void _Get_Adv_File(void) { asm push ds; asm mov dx,offset Get_Adv_File; asm mov si,offset Get_Adv_Filename; asm mov ax,cs; asm mov es,ax; asm mov ds,ax; asm mov ah,1; asm int 0x60; asm pop ds;}; char (__far Get_Adv_File) (node __far *node,short __far *Bytes_left)
iorout.c 24: {
iorout.c 25: }
iorout.c 26: 
iorout.c 27: char Get_EOFname[]="Get_EOF"; char (__far Get_EOF) (node __far *node,short __far *Bytes_left); void _Get_EOF(void) { asm push ds; asm mov dx,offset Get_EOF; asm mov si,offset Get_EOFname; asm mov ax,cs; asm mov es,ax; asm mov ds,ax; asm mov ah,1; asm int 0x60; asm pop ds;}; char (__far Get_EOF) (node __far *node,short __far *Bytes_left)
iorout.c 28: {
iorout.c 29: }
iorout.c 30: 
iorout.c 31: char Get_Homename[]="Get_Home"; char (__far Get_Home) (node __far *node,short __far *Bytes_left); void _Get_Home(void) { asm push ds; asm mov dx,offset Get_Home; asm mov si,offset Get_Homename; asm mov ax,cs; asm mov es,ax; asm mov ds,ax; asm mov ah,1; asm int 0x60; asm pop ds;}; char (__far Get_Home) (node __far *node,short __far *Bytes_left)
iorout.c 32: {
iorout.c 33: }
iorout.c 34: 
iorout.c 35: char Get_Data_SAname[]="Get_Data_SA"; char (__far Get_Data_SA) (node __far *node,short __far *Bytes_left); void _Get_Data_SA(void) { asm push ds; asm mov dx,offset Get_Data_SA; asm mov si,offset Get_Data_SAname; asm mov ax,cs; asm mov es,ax; asm mov ds,ax; asm mov ah,1; asm int 0x60; asm pop ds;}; char (__far Get_Data_SA) (node __far *node,short __far *Bytes_left)
iorout.c 36: {
iorout.c 37: }
iorout.c 38: 
iorout.c 39: char Get_Data_SBname[]="Get_Data_SB"; char (__far Get_Data_SB) (node __far *node,short __far *Bytes_left); void _Get_Data_SB(void) { asm push ds; asm mov dx,offset Get_Data_SB; asm mov si,offset Get_Data_SBname; asm mov ax,cs; asm mov es,ax; asm mov ds,ax; asm mov ah,1; asm int 0x60; asm pop ds;}; char (__far Get_Data_SB) (node __far *node,short __far *Bytes_left)
iorout.c 40: {
iorout.c 41: }
iorout.c 42: 
iorout.c 43: char Get_Data_NSAname[]="Get_Data_NSA"; char (__far Get_Data_NSA) (node __far *node,short __far *Bytes_left); void _Get_Data_NSA(void) { asm push ds; asm mov dx,offset Get_Data_NSA; asm mov si,offset Get_Data_NSAname; asm mov ax,cs; asm mov es,ax; asm mov ds,ax; asm mov ah,1; asm int 0x60; asm pop ds;}; char (__far Get_Data_NSA) (node __far *node,short __far *Bytes_left)
iorout.c 44: {
iorout.c 45: }
iorout.c 46: 
iorout.c 47: char Get_Data_NSBname[]="Get_Data_NSB"; char (__far Get_Data_NSB) (node __far *node,short __far *Bytes_left); void _Get_Data_NSB(void) { asm push ds; asm mov dx,offset Get_Data_NSB; asm mov si,offset Get_Data_NSBname; asm mov ax,cs; asm mov es,ax; asm mov ds,ax; asm mov ah,1; asm int 0x60; asm pop ds;}; char (__far Get_Data_NSB) (node __far *node,short __far *Bytes_left)
iorout.c 48: {
iorout.c 49: }
iorout.c 50: 
iorout.c 51: void main()
iorout.c 52: {
iorout.c 53: _Get_Rewind();
iorout.c 54: _Get_Back_File();
iorout.c 55: _Get_Back_Record();
iorout.c 56: _Get_Adv_Record();
iorout.c 57: _Get_Adv_File();
iorout.c 58: _Get_EOF();
iorout.c 59: _Get_Home();
iorout.c 60: _Get_Data_SA();
iorout.c 61: _Get_Data_SB();
iorout.c 62: _Get_Data_NSA();
iorout.c 63: _Get_Data_NSB();
iorout.c 64: Relinquish(1L);
iorout.c 65: }
iorout.c 66: 
iorout.c 67: 
iorout.c 68: 
iorout.c 69: 