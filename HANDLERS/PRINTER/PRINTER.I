printer.c 1: 
printer.c 2: 
m:\common.ipc\mod.h 1: 
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
m:\common.ipc\mod.h 2: 
m:\common.ipc\mod.h 3: #pragma warn -rvl
m:\common.ipc\mod.h 4: extern long lmult(long num1,long num2);
m:\common.ipc\mod.h 5: extern long ldiv(long number,long divisor);
m:\common.ipc\mod.h 6: extern long lmod(long number,long divisor);
m:\common.ipc\mod.h 7: extern long Request;
m:\common.ipc\mod.h 8: 
m:\common.ipc\mod.h 9: 
m:\common.ipc\mod.h 10: 
m:\common.ipc\mod.h 11: 
m:\common.ipc\mod.h 12: 
m:\common.ipc\mod.h 13: 
m:\common.ipc\mod.h 14: 
m:\common.ipc\mod.h 15: 
m:\common.ipc\mod.h 16: 
m:\common.ipc\mod.h 17: 
m:\common.ipc\mod.h 18: 
m:\common.ipc\mod.h 19: 
m:\common.ipc\mod.h 20: 
m:\common.ipc\mod.h 21: 
m:\common.ipc\mod.h 22: 
m:\common.ipc\mod.h 23: extern module __far *__far *my_tcbptr;
m:\common.ipc\mod.h 24: 
m:\common.ipc\mod.h 25: 
m:\common.ipc\mod.h 26: extern void (__far *swap_to)(module __far *next_to_run);
m:\common.ipc\mod.h 27: extern void (__far *delay)(unsigned short delaytime);
m:\common.ipc\mod.h 28: extern void (__far *change_priority)(char new_priority);
m:\common.ipc\mod.h 29: extern void (__far *Exit)(short code);
m:\common.ipc\mod.h 30: extern void (__far *Relinquish)(long param);
m:\common.ipc\mod.h 31: extern short (__far *Export)(char __far*destination,
m:\common.ipc\mod.h 32: void __far *data);
m:\common.ipc\mod.h 33: extern void __far *(__far *Import)(char __far*WhoIAm);
m:\common.ipc\mod.h 34: extern void __far *(__far *Terminate)(char __far *whose,char pos,
m:\common.ipc\mod.h 35: char cnt,short __far *inprogress,...);
m:\common.ipc\mod.h 36: extern void (__far *Wake)(short mask,short spec);
m:\common.ipc\mod.h 37: extern short (__far *spawn_batch)(char __far *batch_name);
m:\common.ipc\mod.h 38: extern short (__far *spawn)(char __far *name,short stack,char __far *dev_name,
m:\common.ipc\mod.h 39: short threads,char priority);
m:\common.ipc\mod.h 40: extern char __far *(__far *Get_environ)(char __far *environ_name);
m:\common.ipc\mod.h 41: extern void __far *(__far *Allocate)(short length);
m:\common.ipc\mod.h 42: extern void (__far *Free)(void __far *pointer);
m:\common.ipc\mod.h 43: extern void (__far *Connect_Int)(void __far *routine,short Int_Num);
m:\common.ipc\mod.h 44: extern void (__far *Disconnect_Int)(void __far *routine,short Int_Num);
m:\common.ipc\mod.h 45: extern void (__far *gettime)(time_struc __far *time);
m:\common.ipc\mod.h 46: extern short __far *process_control;
m:\common.ipc\mod.h 47: extern short dataseg;
m:\common.ipc\mod.h 48: 
m:\common.ipc\mod.h 49: 
m:\common.ipc\mod.h 50: 
m:\common.ipc\mod.h 51: extern void perish(void);
m:\common.ipc\mod.h 52: extern short fork(char relation);
m:\common.ipc\mod.h 53: extern void destory(void);
m:\common.ipc\mod.h 54: extern void __far *(__far *Inquire_begin)(short what);
m:\common.ipc\mod.h 55: 
m:\common.ipc\mod.h 56: 
m:\common.ipc\mod.h 57: 
m:\common.ipc\mod.h 58: 
m:\common.ipc\mod.h 59: 
m:\common.ipc\mod.h 60: 
m:\common.ipc\mod.h 61: 
m:\common.ipc\mod.h 62: 
m:\common.ipc\mod.h 63: 
m:\common.ipc\mod.h 64: 
m:\common.ipc\mod.h 65: 
m:\common.ipc\mod.h 66: 
m:\common.ipc\mod.h 67: 
m:\common.ipc\mod.h 68: 
m:\common.ipc\mod.h 69: 
m:\common.ipc\mod.h 70: 
m:\common.ipc\mod.h 71: 
m:\common.ipc\mod.h 72: 
m:\common.ipc\mod.h 73: 
m:\common.ipc\mod.h 74: 
m:\common.ipc\mod.h 75: 
m:\common.ipc\mod.h 76: 
m:\common.ipc\mod.h 77: 
m:\common.ipc\mod.h 78: 
m:\common.ipc\mod.h 79: 
m:\common.ipc\mod.h 80: 
m:\common.ipc\mod.h 81: 
m:\common.ipc\mod.h 82: 
m:\common.ipc\mod.h 83: 
m:\common.ipc\mod.h 84: 
m:\common.ipc\mod.h 85: 
m:\common.ipc\mod.h 86: 
m:\common.ipc\mod.h 87: 
m:\common.ipc\mod.h 88: 
m:\common.ipc\mod.h 89: 
m:\common.ipc\mod.h 90: 
m:\common.ipc\mod.h 91: 
m:\common.ipc\mod.h 92: 
m:\common.ipc\mod.h 93: 
m:\common.ipc\mod.h 94: 
m:\common.ipc\mod.h 95: 
m:\common.ipc\mod.h 96: 
m:\common.ipc\mod.h 97: 
m:\common.ipc\mod.h 98: 
m:\common.ipc\mod.h 99: 
m:\common.ipc\mod.h 100: 
m:\common.ipc\mod.h 101: 
m:\common.ipc\mod.h 102: 
m:\common.ipc\mod.h 103: 
m:\common.ipc\mod.h 104: 
m:\common.ipc\mod.h 105: 
m:\common.ipc\mod.h 106: 
m:\common.ipc\mod.h 107: 
m:\common.ipc\mod.h 108: 
m:\common.ipc\mod.h 109: 
m:\common.ipc\mod.h 110: 
m:\common.ipc\mod.h 111: 
m:\common.ipc\mod.h 112: 
m:\common.ipc\mod.h 113: 
m:\common.ipc\mod.h 114: 
m:\common.ipc\mod.h 115: 
m:\common.ipc\mod.h 116: 
m:\common.ipc\mod.h 117: 
m:\common.ipc\mod.h 118: 
m:\common.ipc\mod.h 119: 
m:\common.ipc\mod.h 120: 
m:\common.ipc\mod.h 121: 
m:\common.ipc\mod.h 122: 
m:\common.ipc\mod.h 123: 
m:\common.ipc\mod.h 124: 
m:\common.ipc\mod.h 125: 
m:\common.ipc\mod.h 126: 
m:\common.ipc\mod.h 127: 
m:\common.ipc\mod.h 128: 
m:\common.ipc\mod.h 129: 
m:\common.ipc\mod.h 130: 
m:\common.ipc\mod.h 131: 
m:\common.ipc\mod.h 132: 
m:\common.ipc\mod.h 133: 
m:\common.ipc\mod.h 134: 
m:\common.ipc\mod.h 135: 
m:\common.ipc\mod.h 136: 
m:\common.ipc\mod.h 137: 
m:\common.ipc\mod.h 138: 
m:\common.ipc\mod.h 139: 
m:\common.ipc\mod.h 140: 
m:\common.ipc\mod.h 141: 
m:\common.ipc\mod.h 142: 
m:\common.ipc\mod.h 143: 
m:\common.ipc\mod.h 144: 
m:\common.ipc\mod.h 145: 
m:\common.ipc\mod.h 146: 
m:\common.ipc\mod.h 147: 
m:\common.ipc\mod.h 148: 
m:\common.ipc\mod.h 149: 
m:\common.ipc\mod.h 150: 
m:\common.ipc\mod.h 151: 
m:\common.ipc\mod.h 152: 
m:\common.ipc\mod.h 153: 
m:\common.ipc\mod.h 154: 
m:\common.ipc\mod.h 155: 
m:\common.ipc\mod.h 156: 
m:\common.ipc\mod.h 157: 
m:\common.ipc\mod.h 158: 
m:\common.ipc\mod.h 159: 
m:\common.ipc\mod.h 160: 
m:\common.ipc\mod.h 161: 
m:\common.ipc\mod.h 162: 
m:\common.ipc\mod.h 163: 
m:\common.ipc\mod.h 164: 
m:\common.ipc\mod.h 165: 
m:\common.ipc\mod.h 166: 
m:\common.ipc\mod.h 167: 
m:\common.ipc\mod.h 168: 
m:\common.ipc\mod.h 169: 
m:\common.ipc\mod.h 170: 
m:\common.ipc\mod.h 171: 
m:\common.ipc\mod.h 172: 
m:\common.ipc\mod.h 173: 
m:\common.ipc\mod.h 174: 
m:\common.ipc\mod.h 175: 
m:\common.ipc\mod.h 176: 
m:\common.ipc\mod.h 177: typedef short ether_type;
m:\common.ipc\mod.h 178: typedef ether_type __far *etherptr;
m:\common.ipc\mod.h 179: 
m:\common.ipc\mod.h 180: 
m:\common.ipc\mod.h 181: 
m:\common.ipc\mod.h 182: 
m:\common.ipc\mod.h 183: 
m:\common.ipc\mod.h 184: 
m:\common.ipc\mod.h 185: 
m:\common.ipc\mod.h 186: 
m:\common.ipc\mod.h 187: 
m:\common.ipc\mod.h 188: 
m:\common.ipc\mod.h 189: 
m:\common.ipc\mod.h 190: 
m:\common.ipc\mod.h 191: 
m:\common.ipc\mod.h 192: 
m:\common.ipc\mod.h 193: 
m:\common.ipc\mod.h 194: 
m:\common.ipc\mod.h 195: 
m:\common.ipc\mod.h 196: 
m:\common.ipc\mod.h 197: 
m:\common.ipc\mod.h 198: 
m:\common.ipc\mod.h 199: 
m:\common.ipc\mod.h 200: 
m:\common.ipc\mod.h 201: 
m:\common.ipc\mod.h 202: 
m:\common.ipc\mod.h 203: 
m:\common.ipc\mod.h 204: 
m:\common.ipc\mod.h 205: 
m:\common.ipc\mod.h 206: 
m:\common.ipc\mod.h 207: 
m:\common.ipc\mod.h 208: 
m:\common.ipc\mod.h 209: 
m:\common.ipc\mod.h 210: 
m:\common.ipc\mod.h 211: 
m:\common.ipc\mod.h 212: 
m:\common.ipc\mod.h 213: 
m:\common.ipc\mod.h 214: 
m:\common.ipc\mod.h 215: 
m:\common.ipc\mod.h 216: 
m:\common.ipc\mod.h 217: 
m:\common.ipc\mod.h 218: 
m:\common.ipc\mod.h 219: 
m:\common.ipc\mod.h 220: 
m:\common.ipc\mod.h 221: 
m:\common.ipc\mod.h 222: 
m:\common.ipc\mod.h 223: 
m:\common.ipc\mod.h 224: 
m:\common.ipc\mod.h 225: 
m:\common.ipc\mod.h 226: 
m:\common.ipc\mod.h 227: 
m:\common.ipc\mod.h 228: 
m:\common.ipc\mod.h 229: 
m:\common.ipc\mod.h 230: 
m:\common.ipc\mod.h 231: 
m:\common.ipc\mod.h 232: 
m:\common.ipc\mod.h 233: 
m:\common.ipc\mod.h 234: 
m:\common.ipc\mod.h 235: 
m:\common.ipc\mod.h 236: 
m:\common.ipc\mod.h 237: 
m:\common.ipc\mod.h 238: 
m:\common.ipc\mod.h 239: 
m:\common.ipc\mod.h 240: 
m:\common.ipc\mod.h 241: 
m:\common.ipc\mod.h 242: 
m:\common.ipc\mod.h 243: 
m:\common.ipc\mod.h 244: 
m:\common.ipc\mod.h 245: 
m:\common.ipc\mod.h 246: 
m:\common.ipc\mod.h 247: 
m:\common.ipc\mod.h 248: 
m:\common.ipc\mod.h 249: 
m:\common.ipc\mod.h 250: 
m:\common.ipc\mod.h 251: 
m:\common.ipc\mod.h 252: 
m:\common.ipc\mod.h 253: 
m:\common.ipc\mod.h 254: 
m:\common.ipc\mod.h 255: 
m:\common.ipc\mod.h 256: 
m:\common.ipc\mod.h 257: 
m:\common.ipc\mod.h 258: 
m:\common.ipc\mod.h 259: #pragma warn .rvl
m:\common.ipc\mod.h 260: 
m:\common.ipc\mod.h 261: 
m:\common.ipc\mod.h 262: 
printer.c 3: 
m:\common.ipc\npca.h 1: 
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
m:\common.ipc\npca.h 2: 
m:\common.ipc\npca.h 3: 
printer.c 4: 
m:\common.ipc\multthrd.h 1: 
m:\nipc\handlers\multthrd.h 1: 
m:\nipc\handlers\multthrd.h 2: typedef struct Node_holder
m:\nipc\handlers\multthrd.h 3: {
m:\nipc\handlers\multthrd.h 4: Node __far *Node;
m:\nipc\handlers\multthrd.h 5: struct Node_holder __far *next;
m:\nipc\handlers\multthrd.h 6: } Node_holder;
m:\nipc\handlers\multthrd.h 7: 
m:\nipc\handlers\multthrd.h 8: typedef struct threads_active
m:\nipc\handlers\multthrd.h 9: {
m:\nipc\handlers\multthrd.h 10: short id;  
m:\nipc\handlers\multthrd.h 11: struct threads_active __far *next;
m:\nipc\handlers\multthrd.h 12: Node_holder __far *Nodeq;
m:\nipc\handlers\multthrd.h 13: } threads_active;
m:\nipc\handlers\multthrd.h 14: 
m:\nipc\handlers\multthrd.h 15: 
m:\nipc\handlers\multthrd.h 16: extern threads_active __far *first_thread;
m:\nipc\handlers\multthrd.h 17: extern void store_node(Node __far *what,threads_active __far *where);
m:\nipc\handlers\multthrd.h 18: extern Node __far *get_old_node(short id);
m:\nipc\handlers\multthrd.h 19: extern threads_active __far *thread_active(short which);
m:\nipc\handlers\multthrd.h 20: extern void deactivate(short what);
m:\nipc\handlers\multthrd.h 21: extern void check_outstanding(threads_active __far *where);
m:\nipc\handlers\multthrd.h 22: 
m:\nipc\handlers\multthrd.h 23: 
m:\nipc\handlers\multthrd.h 24: 
m:\nipc\handlers\multthrd.h 25: 
m:\common.ipc\multthrd.h 2: 
m:\common.ipc\multthrd.h 3: 
printer.c 5: 
m:\common.ipc\video.h 1: 
M:\nipc\video\video.h 1: 
M:\nipc\video\video.h 2: typedef struct key_buffer
M:\nipc\video\video.h 3: {
M:\nipc\video\video.h 4: short head,tail,size;
M:\nipc\video\video.h 5: char buffer[];
M:\nipc\video\video.h 6: }key_buffer;
M:\nipc\video\video.h 7: 
M:\nipc\video\video.h 8: 
M:\nipc\video\video.h 9: typedef struct window_type
M:\nipc\video\video.h 10: {
M:\nipc\video\video.h 11: short ulx,uly;           
M:\nipc\video\video.h 12: short width,height;      
M:\nipc\video\video.h 13: short dwidth,dheight;    
M:\nipc\video\video.h 14: short owidth,oheight;    
M:\nipc\video\video.h 15: short xofs,yofs;         
M:\nipc\video\video.h 16: 
M:\nipc\video\video.h 17: short cursx,cursy;       
M:\nipc\video\video.h 18: short _cursx,_cursy;     
M:\nipc\video\video.h 19: short status;            
M:\nipc\video\video.h 20: 
M:\nipc\video\video.h 21: 
M:\nipc\video\video.h 22: 
M:\nipc\video\video.h 23: 
M:\nipc\video\video.h 24: 
M:\nipc\video\video.h 25: 
M:\nipc\video\video.h 26: short opts;              
M:\nipc\video\video.h 27: key_buffer __far *keys;    
M:\nipc\video\video.h 28: char cursattr,attr,borattr,backattr;
M:\nipc\video\video.h 29: short __far *data;
M:\nipc\video\video.h 30: char __far *border;
M:\nipc\video\video.h 31: short __far *shadow;
M:\nipc\video\video.h 32: char title[16];
M:\nipc\video\video.h 33: struct window_type __far *next_window,__far *previous_window;
M:\nipc\video\video.h 34: } window_type;
M:\nipc\video\video.h 35: 
M:\nipc\video\video.h 36: typedef window_type __far *windowptr;
M:\nipc\video\video.h 37: 
M:\nipc\video\video.h 38: 
M:\nipc\video\video.h 39: 
M:\nipc\video\video.h 40: 
M:\nipc\video\video.h 41: 
M:\nipc\video\video.h 42: 
M:\nipc\video\video.h 43: 
M:\nipc\video\video.h 44: 
M:\nipc\video\video.h 45: 
M:\nipc\video\video.h 46: 
M:\nipc\video\video.h 47: 
M:\nipc\video\video.h 48: 
M:\nipc\video\video.h 49: 
M:\nipc\video\video.h 50: 
M:\nipc\video\video.h 51: 
M:\nipc\video\video.h 52: 
M:\nipc\video\video.h 53: 
M:\nipc\video\video.h 54: 
M:\nipc\video\video.h 55: 
M:\nipc\video\video.h 56: 
M:\nipc\video\video.h 57: 
M:\nipc\video\video.h 58: 
M:\nipc\video\video.h 59: 
M:\nipc\video\video.h 60: 
M:\nipc\video\video.h 61: 
M:\nipc\video\video.h 62: 
M:\nipc\video\video.h 63: 
M:\nipc\video\video.h 64: 
M:\nipc\video\video.h 65: 
M:\nipc\video\video.h 66: 
M:\nipc\video\video.h 67: 
M:\nipc\video\video.h 68: 
M:\nipc\video\video.h 69: 
M:\nipc\video\video.h 70: 
M:\nipc\video\video.h 71: 
M:\nipc\video\video.h 72: 
M:\nipc\video\video.h 73: 
M:\nipc\video\video.h 74: 
M:\nipc\video\video.h 75: 
M:\nipc\video\video.h 76: 
M:\nipc\video\video.h 77: 
M:\nipc\video\video.h 78: 
M:\nipc\video\video.h 79: 
M:\nipc\video\video.h 80: 
M:\nipc\video\video.h 81: 
M:\nipc\video\video.h 82: 
M:\nipc\video\video.h 83: 
M:\nipc\video\video.h 84: 
M:\nipc\video\video.h 85: 
M:\nipc\video\video.h 86: 
M:\nipc\video\video.h 87: 
M:\nipc\video\video.h 88: 
M:\nipc\video\video.h 89: 
M:\nipc\video\video.h 90: 
M:\nipc\video\video.h 91: enum windowmods
M:\nipc\video\video.h 92: {
M:\nipc\video\video.h 93: WEND_MOD,
M:\nipc\video\video.h 94: WMOVE_WINDOW,
M:\nipc\video\video.h 95: WMOVE_MOUSE,
M:\nipc\video\video.h 96: WSELECT_WINDOW,
M:\nipc\video\video.h 97: WREMOVE_DATA,
M:\nipc\video\video.h 98: WRESTORE_DATA,
M:\nipc\video\video.h 99: WICON_WINDOW,
M:\nipc\video\video.h 100: WDELTA_MOVE_W,
M:\nipc\video\video.h 101: WDELTA_MOVE_M,
M:\nipc\video\video.h 102: WCLOSE_WINDOW,
M:\nipc\video\video.h 103: WRESIZEX,
M:\nipc\video\video.h 104: WSCROLL_UD,
M:\nipc\video\video.h 105: WRESIZEY,
M:\nipc\video\video.h 106: WSCROLL_LR,
M:\nipc\video\video.h 107: WSCROLL_PUD,
M:\nipc\video\video.h 108: WSCROLL_PLR,
M:\nipc\video\video.h 109: WRESTORE_WINDOW,
M:\nipc\video\video.h 110: WHIDE_WINDOW
M:\nipc\video\video.h 111: };
M:\nipc\video\video.h 112: 
M:\nipc\video\video.h 113: 
M:\nipc\video\video.h 114: 
M:\nipc\video\video.h 115: 
M:\nipc\video\video.h 116: 
M:\nipc\video\video.h 117: 
M:\nipc\video\video.h 118: 
M:\nipc\video\video.h 119: 
M:\nipc\video\video.h 120: 
M:\nipc\video\video.h 121: 
M:\nipc\video\video.h 122: 
M:\nipc\video\video.h 123: 
M:\nipc\video\video.h 124: 
M:\nipc\video\video.h 125: 
M:\nipc\video\video.h 126: 
M:\nipc\video\video.h 127: 
M:\nipc\video\video.h 128: 
M:\nipc\video\video.h 129: 
M:\nipc\video\video.h 130: 
M:\nipc\video\video.h 131: 
M:\nipc\video\video.h 132: 
M:\nipc\video\video.h 133: 
M:\nipc\video\video.h 134: 
M:\nipc\video\video.h 135: 
M:\nipc\video\video.h 136: 
M:\nipc\video\video.h 137: 
M:\nipc\video\video.h 138: 
M:\nipc\video\video.h 139: 
M:\nipc\video\video.h 140: 
M:\nipc\video\video.h 141: 
M:\nipc\video\video.h 142: 
M:\nipc\video\video.h 143: #pragma warn -rvl
M:\nipc\video\video.h 144: 
M:\nipc\video\video.h 145: #pragma argsused
M:\nipc\video\video.h 146: short (__far keypressed)(windowptr window);;
M:\nipc\video\video.h 147: #pragma argsused
M:\nipc\video\video.h 148: char (__far readch)(windowptr window);;
M:\nipc\video\video.h 149: #pragma argsused
M:\nipc\video\video.h 150: void (__far Clr_window)(windowptr window);;
M:\nipc\video\video.h 151: #pragma argsused
M:\nipc\video\video.h 152: 
M:\nipc\video\video.h 153: 
M:\nipc\video\video.h 154: 
M:\nipc\video\video.h 155: windowptr (__far opendisplay)(short ulx,short uly,short width,short height,short opts, char data_attr,char border_attr,char cursor_attr,char __far *title);;
M:\nipc\video\video.h 156: #pragma argsused
M:\nipc\video\video.h 157: 
M:\nipc\video\video.h 158: void (__far get_xy)(windowptr window,short __far *x,short __far *y);;
M:\nipc\video\video.h 159: #pragma argsused
M:\nipc\video\video.h 160: void (__far display)(windowptr window,char charac);;
M:\nipc\video\video.h 161: #pragma argsused
M:\nipc\video\video.h 162: void (__far displayln)(windowptr window,char __far *line,...);;
M:\nipc\video\video.h 163: #pragma argsused
M:\nipc\video\video.h 164: void (__far position)(windowptr window,int x,int y);;
M:\nipc\video\video.h 165: #pragma argsused
M:\nipc\video\video.h 166: void (__far setattr)(windowptr window,char attr);;
M:\nipc\video\video.h 167: #pragma argsused
M:\nipc\video\video.h 168: char (__far getattr)(windowptr window);;
M:\nipc\video\video.h 169: #pragma argsused
M:\nipc\video\video.h 170: void (__far clr_display)(windowptr window,char notes);;
M:\nipc\video\video.h 171: #pragma argsused
M:\nipc\video\video.h 172: short (__far getdisplay)(short __far *x,short __far *y,windowptr __far *window);;
M:\nipc\video\video.h 173: #pragma argsused
M:\nipc\video\video.h 174: short (__far moddisplay)(windowptr window,...);;
M:\nipc\video\video.h 175: #pragma argsused
M:\nipc\video\video.h 176: short (__far closedisplay)(windowptr window);;
M:\nipc\video\video.h 177: #pragma argsused
M:\nipc\video\video.h 178: windowptr (__far dupdisplay)(windowptr window,short opts);;
M:\nipc\video\video.h 179: 
M:\nipc\video\video.h 180: 
M:\nipc\video\video.h 181: 
M:\nipc\video\video.h 182: 
M:\nipc\video\video.h 183: 
M:\nipc\video\video.h 184: 
M:\nipc\video\video.h 185: #pragma warn .rvl
M:\nipc\video\video.h 186: 
M:\nipc\video\video.h 187: 
m:\common.ipc\video.h 2: 
m:\common.ipc\video.h 3: 
printer.c 6: 
iorout.h 1: typedef char Data_process(Node __far *node,short __far *bytes_left);
iorout.h 2: 
iorout.h 3: 
iorout.h 4: #pragma argsused
iorout.h 5: 
iorout.h 6: 
iorout.h 7: 
iorout.h 8: 
iorout.h 9: 
iorout.h 10: 
iorout.h 11: 
iorout.h 12: 
iorout.h 13: 
iorout.h 14: 
iorout.h 15: 
iorout.h 16: 
iorout.h 17: 
iorout.h 18: 
iorout.h 19: 
iorout.h 20: 
iorout.h 21: 
iorout.h 22: 
iorout.h 23: 
iorout.h 24: 
iorout.h 25: 
iorout.h 26: 
iorout.h 27: 
iorout.h 28: 
iorout.h 29: 
iorout.h 30: 
iorout.h 31: 
iorout.h 32: 
iorout.h 33: 
iorout.h 34: 
iorout.h 35: 
iorout.h 36: 
iorout.h 37: 
iorout.h 38: 
iorout.h 39: 
iorout.h 40: 
iorout.h 41: 
iorout.h 42: 
iorout.h 43: 
iorout.h 44: 
iorout.h 45: 
iorout.h 46: 
iorout.h 47: 
iorout.h 48: 
iorout.h 49: 
iorout.h 50: 
iorout.h 51: 
iorout.h 52: 
iorout.h 53: 
iorout.h 54: 
iorout.h 55: 
iorout.h 56: 
iorout.h 57: 
iorout.h 58: 
iorout.h 59: 
iorout.h 60: 
iorout.h 61: 
iorout.h 62: 
iorout.h 63: 
iorout.h 64: char (__far IOGet_Rewind)(Node __far *node,short __far *bytes_left); Data_process __far *Get_Rewind=(Data_process)IOGet_Rewind;; char (__far IOGet_Back_File)(Node __far *node,short __far *bytes_left); Data_process __far *Get_Back_File=(Data_process)IOGet_Back_File;; char (__far IOGet_Back_Record)(Node __far *node,short __far *bytes_left); Data_process __far *Get_Back_Record=(Data_process)IOGet_Back_Record;; char (__far IOGet_Adv_Record)(Node __far *node,short __far *bytes_left); Data_process __far *Get_Adv_Record=(Data_process)IOGet_Adv_Record;; char (__far IOGet_Adv_File)(Node __far *node,short __far *bytes_left); Data_process __far *Get_Adv_File=(Data_process)IOGet_Adv_File;; char (__far IOGet_EOF)(Node __far *node,short __far *bytes_left); Data_process __far *Get_EOF=(Data_process)IOGet_EOF;; char (__far IOGet_Home)(Node __far *node,short __far *bytes_left); Data_process __far *Get_Home=(Data_process)IOGet_Home;; char (__far IOGet_Data_SA)(Node __far *node,short __far *bytes_left); Data_process __far *Get_Data_SA=(Data_process)IOGet_Data_SA;; char (__far IOGet_Data_NSA)(Node __far *node,short __far *bytes_left); Data_process __far *Get_Data_NSA=(Data_process)IOGet_Data_NSA;; char (__far IOGet_Data_SB)(Node __far *node,short __far *bytes_left); Data_process __far *Get_Data_SB=(Data_process)IOGet_Data_SB;; char (__far IOGet_Data_NSB)(Node __far *node,short __far *bytes_left); Data_process __far *Get_Data_NSB=(Data_process)IOGet_Data_NSB;; char (__far IOPut_Data_SA)(Node __far *node,short __far *bytes_left); Data_process __far *Put_Data_SA=(Data_process)IOPut_Data_SA;; char (__far IOPut_Data_NSA)(Node __far *node,short __far *bytes_left); Data_process __far *Put_Data_NSA=(Data_process)IOPut_Data_NSA;; char (__far IOPut_Data_SB)(Node __far *node,short __far *bytes_left); Data_process __far *Put_Data_SB=(Data_process)IOPut_Data_SB;; char (__far IOPut_Data_NSB)(Node __far *node,short __far *bytes_left); Data_process __far *Put_Data_NSB=(Data_process)IOPut_Data_NSB;; char (__far IOPut_Data_TSA)(Node __far *node,short __far *bytes_left); Data_process __far *Put_Data_TSA=(Data_process)IOPut_Data_TSA;; char (__far IOPut_Data_TSB)(Node __far *node,short __far *bytes_left); Data_process __far *Put_Data_TSB=(Data_process)IOPut_Data_TSB;; char (__far IOPut_Data_DSA)(Node __far *node,short __far *bytes_left); Data_process __far *Put_Data_DSA=(Data_process)IOPut_Data_DSA;; char (__far IOPut_Data_DNSA)(Node __far *node,short __far *bytes_left); Data_process __far *Put_Data_DNSA=(Data_process)IOPut_Data_DNSA;; char (__far IOPut_Data_DSB)(Node __far *node,short __far *bytes_left); Data_process __far *Put_Data_DSB=(Data_process)IOPut_Data_DSB;; char (__far IOPut_Data_DNSB)(Node __far *node,short __far *bytes_left); Data_process __far *Put_Data_DNSB=(Data_process)IOPut_Data_DNSB;; char (__far IOPut_Data_DTSA)(Node __far *node,short __far *bytes_left); Data_process __far *Put_Data_DTSA=(Data_process)IOPut_Data_DTSA;; char (__far IOPut_Data_DTSB)(Node __far *node,short __far *bytes_left); Data_process __far *Put_Data_DTSB=(Data_process)IOPut_Data_DTSB;;;;;;;;;;;;;;;;;;;;;;;;;
iorout.h 65: 
printer.c 7: 
printer.c 8: windowptr output;
printer.c 9: 
printer.c 10: 
printer.c 11: 
printer.c 12: 
printer.c 13: 
printer.c 14: 
printer.c 15: 
printer.c 16: 
printer.c 17: 
printer.c 18: typedef struct message_holder
printer.c 19: {
printer.c 20: unsigned short __far *message;
printer.c 21: short length;
printer.c 22: struct message_holder __far *next;
printer.c 23: } message_holder;
printer.c 24: 
printer.c 25: 
printer.c 26: void terminate_watch(Node __far *what
printer.c 27: 
printer.c 28: )
printer.c 29: {
printer.c 30: threads_active __far *temp=first_thread;
printer.c 31: char cnt=0;
printer.c 32: 
printer.c 33: while (temp)
printer.c 34: {
printer.c 35: if (temp->id==what->Node.Channel)
printer.c 36: break;
printer.c 37: temp=temp->next;
printer.c 38: }
printer.c 39: if (!temp)
printer.c 40: {
printer.c 41: asm int 3;
printer.c 42: Exit(50);     
printer.c 43: }
printer.c 44: while (1)
printer.c 45: {
printer.c 46: if (!what->Node.Node_Address)
printer.c 47: {
printer.c 48: 
printer.c 49: Relinquish(16L);
printer.c 50: }
printer.c 51: if (!(cnt&0xf))
printer.c 52: check_outstanding(temp);
printer.c 53: if (cnt==1)              
printer.c 54: {                        
printer.c 55: if (!(what->Tracking.Status&(0x01|0x04|0x08)))
printer.c 56: {
printer.c 57: what->Return.Opcode=0;
printer.c 58: what->Tracking.Status|=0x01|0x08;
printer.c 59: Export(what->Tracking.Source,what);
printer.c 60: }
printer.c 61: }
printer.c 62: Relinquish(0L);
printer.c 63: cnt++;
printer.c 64: }
printer.c 65: }
printer.c 66: 
printer.c 67: void Record_Print(Data_process __far *Routine,Node __far *what)
printer.c 68: {
printer.c 69: char ch;
printer.c 70: short chars_to_go;
printer.c 71: do
printer.c 72: {
printer.c 73: ch=Routine(what,&chars_to_go);
printer.c 74: displayln(output,"*c",ch);
printer.c 75: }
printer.c 76: while (chars_to_go);
printer.c 77: }
printer.c 78: 
printer.c 79: void process_node(Node __far *what,short me)
printer.c 80: {
printer.c 81: short temp;
printer.c 82: message_holder __far *tmessage;
printer.c 83: proc_again:
printer.c 84: if (what->Node.Node_Address)
printer.c 85: {
printer.c 86: displayln(output,"Processing: *n on *n Node:*H\n",
printer.c 87: what->Node.Rex,
printer.c 88: what->Tracking.Side,
printer.c 89: what->Node.Node_Address);
printer.c 90: switch(what->Node.Rex&0xf)
printer.c 91: {
printer.c 92: case 0:
printer.c 93: if (what->Tracking.Side==0&&
printer.c 94: !(what->Tracking.Status&(0x01|0x08)))
printer.c 95: {
printer.c 96: what->Return.Opcode=0;
printer.c 97: what->Tracking.Status|=0x01|0x08;
printer.c 98: Export(what->Tracking.Source,what);
printer.c 99: what->Tracking.Side=1;
printer.c 100: }
printer.c 101: if (fork(0))
printer.c 102: terminate_watch(what);
printer.c 103: else
printer.c 104: {
printer.c 105: if (what->Node.XOptions&0x10)
printer.c 106: {
printer.c 107: displayln(output,"Flushing\n");
printer.c 108: }
printer.c 109: }
printer.c 110: destory();
printer.c 111: free(what->Data);
printer.c 112: what->Data=0L;
printer.c 113: what->Return.Byte_count=0;
printer.c 114: what->Node.FPI++;
printer.c 115: what->Return.Opcode=1;
printer.c 116: break;
printer.c 117: case 1:
printer.c 118: {
printer.c 119: 
printer.c 120: 
printer.c 121: Data_process __far *Routine;
printer.c 122: switch(what->Node.Options&(0x2000|0x1000))
printer.c 123: {
printer.c 124: case 0x2000:
printer.c 125: Routine=Get_Data_SA;
printer.c 126: break;
printer.c 127: case 0x1000:
printer.c 128: Routine=Get_Data_NSB;
printer.c 129: break;
printer.c 130: case 0x1000|0x2000:
printer.c 131: Routine=Get_Data_SB;
printer.c 132: break;
printer.c 133: default:
printer.c 134: Routine=Get_Data_NSA;
printer.c 135: break;
printer.c 136: }
printer.c 137: 
printer.c 138: Record_Print(Routine,what);
printer.c 139: what->Data=0L;  
printer.c 140: 
printer.c 141: what->Node.FPI++;
printer.c 142: what->Return.Opcode=1;
printer.c 143: }
printer.c 144: break;
printer.c 145: case 2:
printer.c 146: Record_Print(Get_Rewind,what);
printer.c 147: what->Node.FPI++;
printer.c 148: what->Return.Opcode=1;
printer.c 149: break;
printer.c 150: case 4:
printer.c 151: Record_Print(Get_Back_Record,what);
printer.c 152: what->Node.FPI++;
printer.c 153: what->Return.Opcode=1;
printer.c 154: break;
printer.c 155: case 3:
printer.c 156: Record_Print(Get_Back_File,what);
printer.c 157: what->Node.FPI++;
printer.c 158: what->Return.Opcode=1;
printer.c 159: break;
printer.c 160: case 6:
printer.c 161: Record_Print(Get_Adv_File,what);
printer.c 162: what->Node.FPI++;
printer.c 163: what->Return.Opcode=1;
printer.c 164: break;
printer.c 165: case 5:
printer.c 166: Record_Print(Get_Adv_Record,what);
printer.c 167: what->Node.FPI++;
printer.c 168: what->Return.Opcode=1;
printer.c 169: break;
printer.c 170: case 7:
printer.c 171: Record_Print(Get_EOF,what);
printer.c 172: what->Node.FPI++;
printer.c 173: what->Return.Opcode=1;
printer.c 174: break;
printer.c 175: case 8:
printer.c 176: if (what->Node.XOptions&0x0008)
printer.c 177: displayln(output,"Disconnect Line\n");
printer.c 178: Record_Print(Get_Home,what);
printer.c 179: break;
printer.c 180: 
printer.c 181: }
printer.c 182: if (!(what->Tracking.Status&0x04))
printer.c 183: {
printer.c 184: what->Tracking.Status|=0x04;
printer.c 185: if (!(what->Tracking.Status&0x08))  
printer.c 186: {
printer.c 187: what->Tracking.Status|=0x08;
printer.c 188: Export(what->Tracking.Source,what);
printer.c 189: }
printer.c 190: }
printer.c 191: }
printer.c 192: else
printer.c 193: {
printer.c 194: what->Return.Opcode=1;
printer.c 195: Export(what->Tracking.Source,what);
printer.c 196: }
printer.c 197: 
printer.c 198: what=get_old_node(me);
printer.c 199: if (!what)
printer.c 200: deactivate(me);
printer.c 201: goto proc_again;
printer.c 202: }
printer.c 203: 
printer.c 204: void init_local_structs(void)
printer.c 205: {
printer.c 206: short i;
printer.c 207: output=opendisplay(4,5,50,10,0x0020|0x0004|0x0400,
printer.c 208: 0x1f,0x1f,0x2e,"Printer");
printer.c 209: 
printer.c 210: }
printer.c 211: 
printer.c 212: 
printer.c 213: 
printer.c 214: 