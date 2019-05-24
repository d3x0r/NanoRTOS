multhand.c 1: 
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
m:\common.ipc\os.h 30: 
m:\common.ipc\os.h 31: 
m:\common.ipc\os.h 32: 
m:\common.ipc\os.h 33: 
m:\common.ipc\os.h 34: typedef (__far *proc)();
m:\common.ipc\os.h 35: 
m:\common.ipc\os.h 36: typedef long time_struc;
m:\common.ipc\os.h 37: typedef struct routine_ptr
m:\common.ipc\os.h 38: {
m:\common.ipc\os.h 39: char name[16];
m:\common.ipc\os.h 40: proc procedure;
m:\common.ipc\os.h 41: struct routine_ptr __far *next;
m:\common.ipc\os.h 42: }routine_ptr;
m:\common.ipc\os.h 43: 
m:\common.ipc\os.h 44: typedef struct environ_entry
m:\common.ipc\os.h 45: {
m:\common.ipc\os.h 46: char name[12];
m:\common.ipc\os.h 47: char parameter[30];
m:\common.ipc\os.h 48: struct environ_entry __far *next;
m:\common.ipc\os.h 49: }environ_entry;
m:\common.ipc\os.h 50: 
m:\common.ipc\os.h 51: typedef struct module
m:\common.ipc\os.h 52: {
m:\common.ipc\os.h 53: char name[8];                 
m:\common.ipc\os.h 54: struct module __far *child;           
m:\common.ipc\os.h 55: struct module __far *elder_sib;       
m:\common.ipc\os.h 56: struct module __far *parent;          
m:\common.ipc\os.h 57: struct module __far *next;            
m:\common.ipc\os.h 58: struct module __far *prior;           
m:\common.ipc\os.h 59: short __far *stack;                   
m:\common.ipc\os.h 60: unsigned char __far *code;            
m:\common.ipc\os.h 61: unsigned short size;                
m:\common.ipc\os.h 62: unsigned short stack_size;          
m:\common.ipc\os.h 63: unsigned short max_size;            
m:\common.ipc\os.h 64: unsigned long status;               
m:\common.ipc\os.h 65: unsigned short stackseg,stackofs;   
m:\common.ipc\os.h 66: unsigned long Acumulated_time;      
m:\common.ipc\os.h 67: unsigned char Priority;             
m:\common.ipc\os.h 68: unsigned char options;              
m:\common.ipc\os.h 69: environ_entry __far *environ;         
m:\common.ipc\os.h 70: char __far *device_name;              
m:\common.ipc\os.h 71: char __far *load_path;                
m:\common.ipc\os.h 72: 
m:\common.ipc\os.h 73: } module;
m:\common.ipc\os.h 74: 
m:\common.ipc\os.h 75: typedef struct node_entry
m:\common.ipc\os.h 76: {
m:\common.ipc\os.h 77: short status;
m:\common.ipc\os.h 78: unsigned char __far *data;
m:\common.ipc\os.h 79: module __far *source;  
m:\common.ipc\os.h 80: 
m:\common.ipc\os.h 81: 
m:\common.ipc\os.h 82: 
m:\common.ipc\os.h 83: 
m:\common.ipc\os.h 84: 
m:\common.ipc\os.h 85: struct node_entry __far *next,__far *last;
m:\common.ipc\os.h 86: } node_entry;
m:\common.ipc\os.h 87: 
m:\common.ipc\os.h 88: typedef struct device_tree
m:\common.ipc\os.h 89: {
m:\common.ipc\os.h 90: struct device_tree __far *less,__far *more;
m:\common.ipc\os.h 91: module __far *Destination_Task;
m:\common.ipc\os.h 92: char dest[12];
m:\common.ipc\os.h 93: node_entry __far *node;
m:\common.ipc\os.h 94: } device_tree;
m:\common.ipc\os.h 95: 
m:\common.ipc\os.h 96: 
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
\common.ipc\mod.h 37: extern module __far *(__far *spawn_batch)(char __far *batch_name);
\common.ipc\mod.h 38: extern module __far *(__far *spawn)(char __far *name,short stack,char __far *dev_name,
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
\common.ipc\mod.h 177: 
\common.ipc\mod.h 178: 
\common.ipc\mod.h 179: 
\common.ipc\mod.h 180: 
\common.ipc\mod.h 181: 
\common.ipc\mod.h 182: 
\common.ipc\mod.h 183: 
\common.ipc\mod.h 184: 
\common.ipc\mod.h 185: 
\common.ipc\mod.h 186: 
\common.ipc\mod.h 187: typedef short ether_type;
\common.ipc\mod.h 188: typedef ether_type __far *etherptr;
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
\common.ipc\mod.h 259: 
\common.ipc\mod.h 260: 
\common.ipc\mod.h 261: 
\common.ipc\mod.h 262: 
\common.ipc\mod.h 263: 
\common.ipc\mod.h 264: 
\common.ipc\mod.h 265: 
\common.ipc\mod.h 266: 
\common.ipc\mod.h 267: 
\common.ipc\mod.h 268: 
\common.ipc\mod.h 269: 
\common.ipc\mod.h 270: 
\common.ipc\mod.h 271: 
\common.ipc\mod.h 272: 
\common.ipc\mod.h 273: 
\common.ipc\mod.h 274: #pragma warn .rvl
\common.ipc\mod.h 275: 
\common.ipc\mod.h 276: 
\common.ipc\mod.h 277: 
multhand.c 2: 
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
m:\nipc\pca\npca.h 55: 
m:\nipc\pca\npca.h 56: 
m:\nipc\pca\npca.h 57: 
m:\nipc\pca\npca.h 58: typedef struct Data_Track
m:\nipc\pca\npca.h 59: {
m:\nipc\pca\npca.h 60: short offset;  
m:\nipc\pca\npca.h 61: short filler;
m:\nipc\pca\npca.h 62: short length;  
m:\nipc\pca\npca.h 63: }Data_Track;
m:\nipc\pca\npca.h 64: 
m:\nipc\pca\npca.h 65: typedef struct Etherdata
m:\nipc\pca\npca.h 66: {
m:\nipc\pca\npca.h 67: unsigned char Control;     
m:\nipc\pca\npca.h 68: unsigned char Break_char;
m:\nipc\pca\npca.h 69: unsigned char SourceIP[4];
m:\nipc\pca\npca.h 70: unsigned char DestIP[4];
m:\nipc\pca\npca.h 71: unsigned short SourceTCP;
m:\nipc\pca\npca.h 72: unsigned short DestTCP;
m:\nipc\pca\npca.h 73: unsigned short Top_of_form;
m:\nipc\pca\npca.h 74: unsigned char TCL_data;
m:\nipc\pca\npca.h 75: unsigned char TCL_index;
m:\nipc\pca\npca.h 76: 
m:\nipc\pca\npca.h 77: }Etherdata;
m:\nipc\pca\npca.h 78: 
m:\nipc\pca\npca.h 79: typedef struct Commdata
m:\nipc\pca\npca.h 80: {
m:\nipc\pca\npca.h 81: 
m:\nipc\pca\npca.h 82: 
m:\nipc\pca\npca.h 83: 
m:\nipc\pca\npca.h 84: 
m:\nipc\pca\npca.h 85: 
m:\nipc\pca\npca.h 86: 
m:\nipc\pca\npca.h 87: 
m:\nipc\pca\npca.h 88: unsigned char Control;
m:\nipc\pca\npca.h 89: unsigned char Baud;
m:\nipc\pca\npca.h 90: unsigned char Frame_Stop_Par;   
m:\nipc\pca\npca.h 91: unsigned char fill1;
m:\nipc\pca\npca.h 92: unsigned char Mode;             
m:\nipc\pca\npca.h 93: unsigned char Break_char;
m:\nipc\pca\npca.h 94: unsigned short fill2;
m:\nipc\pca\npca.h 95: unsigned short Top_of_form;     
m:\nipc\pca\npca.h 96: 
m:\nipc\pca\npca.h 97: 
m:\nipc\pca\npca.h 98: 
m:\nipc\pca\npca.h 99: 
m:\nipc\pca\npca.h 100: 
m:\nipc\pca\npca.h 101: 
m:\nipc\pca\npca.h 102: 
m:\nipc\pca\npca.h 103: 
m:\nipc\pca\npca.h 104: unsigned char TCL_data;
m:\nipc\pca\npca.h 105: unsigned char TCL_index;
m:\nipc\pca\npca.h 106: }Commdata;
m:\nipc\pca\npca.h 107: 
m:\nipc\pca\npca.h 108: typedef struct NodeTrack
m:\nipc\pca\npca.h 109: {
m:\nipc\pca\npca.h 110: char Side;
m:\nipc\pca\npca.h 111: long start_time;
m:\nipc\pca\npca.h 112: unsigned short Status;
m:\nipc\pca\npca.h 113: char __far *Source;
m:\nipc\pca\npca.h 114: struct Node __far *next,__far *prior;
m:\nipc\pca\npca.h 115: }NodeTrack;
m:\nipc\pca\npca.h 116: 
m:\nipc\pca\npca.h 117: typedef struct BreakReturn
m:\nipc\pca\npca.h 118: {
m:\nipc\pca\npca.h 119: char Opcode;
m:\nipc\pca\npca.h 120: unsigned char TCL_data;
m:\nipc\pca\npca.h 121: unsigned char TCL_idx;
m:\nipc\pca\npca.h 122: unsigned short Transport;
m:\nipc\pca\npca.h 123: unsigned short SubChannel;
m:\nipc\pca\npca.h 124: } BreakReturn;
m:\nipc\pca\npca.h 125: 
m:\nipc\pca\npca.h 126: typedef struct ReturnData
m:\nipc\pca\npca.h 127: {
m:\nipc\pca\npca.h 128: char  Opcode;
m:\nipc\pca\npca.h 129: unsigned short Status;
m:\nipc\pca\npca.h 130: unsigned long  Byte_count;
m:\nipc\pca\npca.h 131: unsigned short XStatus;
m:\nipc\pca\npca.h 132: unsigned short Extended_Size;
m:\nipc\pca\npca.h 133: unsigned short __far *Extended_mail;
m:\nipc\pca\npca.h 134: }ReturnData;
m:\nipc\pca\npca.h 135: 
m:\nipc\pca\npca.h 136: typedef struct NodeData
m:\nipc\pca\npca.h 137: {
m:\nipc\pca\npca.h 138: unsigned short Transport;
m:\nipc\pca\npca.h 139: unsigned short Channel;
m:\nipc\pca\npca.h 140: unsigned long  Node_Address;
m:\nipc\pca\npca.h 141: unsigned short Rex;
m:\nipc\pca\npca.h 142: unsigned short Options;
m:\nipc\pca\npca.h 143: unsigned long  Byte_count;
m:\nipc\pca\npca.h 144: unsigned short XOptions;
m:\nipc\pca\npca.h 145: unsigned long  FPI;
m:\nipc\pca\npca.h 146: union
m:\nipc\pca\npca.h 147: {
m:\nipc\pca\npca.h 148: Etherdata Ether;
m:\nipc\pca\npca.h 149: Commdata Comm;
m:\nipc\pca\npca.h 150: } Info;
m:\nipc\pca\npca.h 151: }NodeData;
m:\nipc\pca\npca.h 152: 
m:\nipc\pca\npca.h 153: typedef struct Node
m:\nipc\pca\npca.h 154: {
m:\nipc\pca\npca.h 155: ReturnData Return;
m:\nipc\pca\npca.h 156: NodeTrack Tracking;
m:\nipc\pca\npca.h 157: unsigned short __far *Data;
m:\nipc\pca\npca.h 158: NodeData Node;
m:\nipc\pca\npca.h 159: Data_Track Data_handle;
m:\nipc\pca\npca.h 160: }Node;
m:\nipc\pca\npca.h 161: 
m:\nipc\pca\npca.h 162: typedef struct Node_holder
m:\nipc\pca\npca.h 163: {
m:\nipc\pca\npca.h 164: Node __far *Node;
m:\nipc\pca\npca.h 165: struct Node_holder __far *next;
m:\nipc\pca\npca.h 166: } Node_holder;
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
m:\nipc\pca\npca.h 213: 
m:\nipc\pca\npca.h 214: 
m:\nipc\pca\npca.h 215: 
m:\nipc\pca\npca.h 216: 
m:\nipc\pca\npca.h 217: 
m:\nipc\pca\npca.h 218: 
m:\nipc\pca\npca.h 219: 
m:\nipc\pca\npca.h 220: 
m:\nipc\pca\npca.h 221: 
m:\nipc\pca\npca.h 222: 
m:\nipc\pca\npca.h 223: 
m:\nipc\pca\npca.h 224: 
m:\nipc\pca\npca.h 225: 
m:\nipc\pca\npca.h 226: 
m:\nipc\pca\npca.h 227: 
m:\nipc\pca\npca.h 228: 
m:\nipc\pca\npca.h 229: 
m:\nipc\pca\npca.h 230: 
m:\nipc\pca\npca.h 231: 
m:\nipc\pca\npca.h 232: 
m:\nipc\pca\npca.h 233: 
m:\nipc\pca\npca.h 234: 
m:\nipc\pca\npca.h 235: 
m:\nipc\pca\npca.h 236: 
m:\nipc\pca\npca.h 237: 
m:\nipc\pca\npca.h 238: 
m:\nipc\pca\npca.h 239: 
m:\nipc\pca\npca.h 240: 
m:\nipc\pca\npca.h 241: 
m:\nipc\pca\npca.h 242: 
m:\nipc\pca\npca.h 243: 
m:\nipc\pca\npca.h 244: 
m:\nipc\pca\npca.h 245: 
m:\nipc\pca\npca.h 246: 
m:\nipc\pca\npca.h 247: 
m:\nipc\pca\npca.h 248: 
m:\nipc\pca\npca.h 249: 
m:\nipc\pca\npca.h 250: 
m:\nipc\pca\npca.h 251: 
m:\nipc\pca\npca.h 252: 
m:\nipc\pca\npca.h 253: 
m:\nipc\pca\npca.h 254: 
m:\nipc\pca\npca.h 255: 
m:\nipc\pca\npca.h 256: 
m:\nipc\pca\npca.h 257: 
m:\nipc\pca\npca.h 258: 
m:\nipc\pca\npca.h 259: 
m:\nipc\pca\npca.h 260: 
m:\nipc\pca\npca.h 261: 
m:\nipc\pca\npca.h 262: 
m:\nipc\pca\npca.h 263: 
m:\nipc\pca\npca.h 264: 
m:\nipc\pca\npca.h 265: 
m:\nipc\pca\npca.h 266: 
m:\nipc\pca\npca.h 267: 
m:\nipc\pca\npca.h 268: 
m:\nipc\pca\npca.h 269: 
m:\nipc\pca\npca.h 270: 
m:\nipc\pca\npca.h 271: 
m:\nipc\pca\npca.h 272: #pragma argsused
m:\nipc\pca\npca.h 273: Node __far * (__far Create_node)(char side);;
m:\nipc\pca\npca.h 274: 
m:\nipc\pca\npca.h 275: 
m:\nipc\pca\npca.h 276: 
m:\nipc\pca\npca.h 277: 
\common.ipc\npca.h 2: 
\common.ipc\npca.h 3: 
multhand.c 3: 
hostess.h 1: 
hostess.h 2: typedef struct line_entry
hostess.h 3: {
hostess.h 4: char rx_size;
hostess.h 5: char stop_par;
hostess.h 6: char tx_size;
hostess.h 7: short baud;
hostess.h 8: char flow;
hostess.h 9: } line_entry;
hostess.h 10: 
hostess.h 11: typedef struct card_entry
hostess.h 12: {
hostess.h 13: short io_base,line_status;
hostess.h 14: unsigned char WR2,WR3,WR4,WR5,WR12,WR13,WR15,RR0;
hostess.h 15: short Txq_head,Txq_tail,Txq_offset;
hostess.h 16: short Rxq_head,Rxq_tail,Rxq_offset;
hostess.h 17: char Flow_control,status,Break_char;
hostess.h 18: } card_entry;
hostess.h 19: 
hostess.h 20: 
hostess.h 21: 
hostess.h 22: 
hostess.h 23: 
hostess.h 24: 
hostess.h 25: 
hostess.h 26: 
hostess.h 27: 
hostess.h 28: 
hostess.h 29: 
hostess.h 30: 
hostess.h 31: 
hostess.h 32: 
hostess.h 33: 
hostess.h 34: 
hostess.h 35: 
hostess.h 36: 
hostess.h 37: 
hostess.h 38: 
hostess.h 39: #pragma argsused
hostess.h 40: short (__far hiread)(char __far *buffer,short length,short line);;
hostess.h 41: 
hostess.h 42: #pragma argsused
hostess.h 43: short (__far hiwrite)(char __far *buffer,short length,short line);;
hostess.h 44: 
hostess.h 45: #pragma argsused
hostess.h 46: short (__far hiopen)(short line,line_entry __far *message);;
hostess.h 47: 
hostess.h 48: #pragma argsused
hostess.h 49: short (__far hiclose)(short line);;
hostess.h 50: #pragma argsused
hostess.h 51: short (__far higetc)(short line,char __far *charac);;
hostess.h 52: #pragma argsused
hostess.h 53: short (__far higetblk)(char __far *buffer,short length,short line);;
hostess.h 54: #pragma argsused
hostess.h 55: void __far * (__far higetline)(short line);;
hostess.h 56: #pragma argsused
hostess.h 57: short (__far hiterm)(short line,char op);;
hostess.h 58: 
hostess.h 59: 
hostess.h 60: #pragma argsused
hostess.h 61: void (__far hold_IO)(void);;
hostess.h 62: 
hostess.h 63: 
multhand.c 4: 
ipchost.h 1: short bauds[]=
ipchost.h 2: {
ipchost.h 3: -1,3070,2046,1394,1140,1022,510,254,126,83,75,62,41,30,19,14,
ipchost.h 4: -1,-1,6,-1,2,-1,-1,1,0,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
ipchost.h 5: 
ipchost.h 6: 
ipchost.h 7: 
ipchost.h 8: 
ipchost.h 9: 
ipchost.h 10: 
ipchost.h 11: 
ipchost.h 12: 
ipchost.h 13: 
ipchost.h 14: 
ipchost.h 15: 
ipchost.h 16: 
ipchost.h 17: 
ipchost.h 18: 
ipchost.h 19: 
ipchost.h 20: 
ipchost.h 21: 
ipchost.h 22: 
ipchost.h 23: 
ipchost.h 24: 
ipchost.h 25: 
ipchost.h 26: 
ipchost.h 27: 
ipchost.h 28: 
ipchost.h 29: typedef struct line_table
ipchost.h 30: {
ipchost.h 31: line_entry config;
ipchost.h 32: char control,FrStPa,Mode;
ipchost.h 33: short baud;
ipchost.h 34: char open;
ipchost.h 35: unsigned char LPP;  
ipchost.h 36: unsigned char CLP;  
ipchost.h 37: }line_table;
ipchost.h 38: 
ipchost.h 39: typedef struct break_info
ipchost.h 40: {
ipchost.h 41: char TCL_avail;
ipchost.h 42: char TCL_idx;
ipchost.h 43: short Transport;
ipchost.h 44: short SubChannel;
ipchost.h 45: char last_sigs;
ipchost.h 46: char last_break;
ipchost.h 47: }break_info;
ipchost.h 48: 
ipchost.h 49: 
ipchost.h 50: 
ipchost.h 51: 
ipchost.h 52: 
ipchost.h 53: 
ipchost.h 54: 
multhand.c 5: 
\common.ipc\ether.h 1: 
m:\nipc\ether\wdether\ether.h 1: 
m:\nipc\ether\wdether\ether.h 2: 
m:\nipc\ether\wdether\ether.h 3: 
m:\nipc\ether\wdether\ether.h 4: 
m:\nipc\ether\wdether\ether.h 5: 
m:\nipc\ether\wdether\ether.h 6: 
m:\nipc\ether\wdether\ether.h 7: 
m:\nipc\ether\wdether\ether.h 8: 
m:\nipc\ether\wdether\ether.h 9: 
m:\nipc\ether\wdether\ether.h 10: 
m:\nipc\ether\wdether\ether.h 11: 
m:\nipc\ether\wdether\ether.h 12: 
m:\nipc\ether\wdether\ether.h 13: 
m:\nipc\ether\wdether\ether.h 14: 
m:\nipc\ether\wdether\ether.h 15: typedef unsigned char IP_addr[4];
m:\nipc\ether\wdether\ether.h 16: typedef unsigned char Ether_addr[6];
m:\nipc\ether\wdether\ether.h 17: 
m:\nipc\ether\wdether\ether.h 18: 
m:\nipc\ether\wdether\ether.h 19: 
m:\nipc\ether\wdether\ether.h 20: 
m:\nipc\ether\wdether\ether.h 21: 
m:\nipc\ether\wdether\ether.h 22: 
m:\nipc\ether\wdether\ether.h 23: 
m:\nipc\ether\wdether\ether.h 24: 
m:\nipc\ether\wdether\ether.h 25: 
m:\nipc\ether\wdether\ether.h 26: 
m:\nipc\ether\wdether\ether.h 27: 
m:\nipc\ether\wdether\ether.h 28: 
m:\nipc\ether\wdether\ether.h 29: 
m:\nipc\ether\wdether\ether.h 30: 
m:\nipc\ether\wdether\ether.h 31: typedef struct Ether_header    
m:\nipc\ether\wdether\ether.h 32: 
m:\nipc\ether\wdether\ether.h 33: {
m:\nipc\ether\wdether\ether.h 34: unsigned char status;
m:\nipc\ether\wdether\ether.h 35: unsigned char next;
m:\nipc\ether\wdether\ether.h 36: short count;
m:\nipc\ether\wdether\ether.h 37: }Ether_header;
m:\nipc\ether\wdether\ether.h 38: 
m:\nipc\ether\wdether\ether.h 39: typedef struct Ether_layer
m:\nipc\ether\wdether\ether.h 40: {
m:\nipc\ether\wdether\ether.h 41: Ether_addr dest;
m:\nipc\ether\wdether\ether.h 42: Ether_addr source;
m:\nipc\ether\wdether\ether.h 43: short type;
m:\nipc\ether\wdether\ether.h 44: } Ether_layer;
m:\nipc\ether\wdether\ether.h 45: 
m:\nipc\ether\wdether\ether.h 46: typedef struct IP_layer
m:\nipc\ether\wdether\ether.h 47: {
m:\nipc\ether\wdether\ether.h 48: unsigned char version_len; 
m:\nipc\ether\wdether\ether.h 49: 
m:\nipc\ether\wdether\ether.h 50: 
m:\nipc\ether\wdether\ether.h 51: 
m:\nipc\ether\wdether\ether.h 52: unsigned char service;     
m:\nipc\ether\wdether\ether.h 53: unsigned short length;     
m:\nipc\ether\wdether\ether.h 54: unsigned short ident;      
m:\nipc\ether\wdether\ether.h 55: 
m:\nipc\ether\wdether\ether.h 56: 
m:\nipc\ether\wdether\ether.h 57: 
m:\nipc\ether\wdether\ether.h 58: 
m:\nipc\ether\wdether\ether.h 59: 
m:\nipc\ether\wdether\ether.h 60: 
m:\nipc\ether\wdether\ether.h 61: unsigned short frags;      
m:\nipc\ether\wdether\ether.h 62: 
m:\nipc\ether\wdether\ether.h 63: 
m:\nipc\ether\wdether\ether.h 64: 
m:\nipc\ether\wdether\ether.h 65: 
m:\nipc\ether\wdether\ether.h 66: unsigned char time_to_live;
m:\nipc\ether\wdether\ether.h 67: unsigned char protocol;    
m:\nipc\ether\wdether\ether.h 68: short checksum;            
m:\nipc\ether\wdether\ether.h 69: IP_addr source;            
m:\nipc\ether\wdether\ether.h 70: 
m:\nipc\ether\wdether\ether.h 71: IP_addr dest;              
m:\nipc\ether\wdether\ether.h 72: 
m:\nipc\ether\wdether\ether.h 73: } IP_layer;
m:\nipc\ether\wdether\ether.h 74: 
m:\nipc\ether\wdether\ether.h 75: 
m:\nipc\ether\wdether\ether.h 76: 
m:\nipc\ether\wdether\ether.h 77: 
m:\nipc\ether\wdether\ether.h 78: 
m:\nipc\ether\wdether\ether.h 79: typedef struct Pseudo_IP
m:\nipc\ether\wdether\ether.h 80: {
m:\nipc\ether\wdether\ether.h 81: IP_addr source;
m:\nipc\ether\wdether\ether.h 82: IP_addr dest;
m:\nipc\ether\wdether\ether.h 83: unsigned char zero;
m:\nipc\ether\wdether\ether.h 84: unsigned char prot;
m:\nipc\ether\wdether\ether.h 85: unsigned short length;
m:\nipc\ether\wdether\ether.h 86: long zeros;
m:\nipc\ether\wdether\ether.h 87: } Pseudo_IP;
m:\nipc\ether\wdether\ether.h 88: 
m:\nipc\ether\wdether\ether.h 89: 
m:\nipc\ether\wdether\ether.h 90: typedef struct TCP_layer
m:\nipc\ether\wdether\ether.h 91: {
m:\nipc\ether\wdether\ether.h 92: short source;              
m:\nipc\ether\wdether\ether.h 93: 
m:\nipc\ether\wdether\ether.h 94: 
m:\nipc\ether\wdether\ether.h 95: short dest;                
m:\nipc\ether\wdether\ether.h 96: 
m:\nipc\ether\wdether\ether.h 97: 
m:\nipc\ether\wdether\ether.h 98: unsigned long seq;         
m:\nipc\ether\wdether\ether.h 99: unsigned long ack;         
m:\nipc\ether\wdether\ether.h 100: char hlen;                 
m:\nipc\ether\wdether\ether.h 101: 
m:\nipc\ether\wdether\ether.h 102: 
m:\nipc\ether\wdether\ether.h 103: 
m:\nipc\ether\wdether\ether.h 104: char control;              
m:\nipc\ether\wdether\ether.h 105: short window;              
m:\nipc\ether\wdether\ether.h 106: short checksum;            
m:\nipc\ether\wdether\ether.h 107: short urgent;
m:\nipc\ether\wdether\ether.h 108: }TCP_layer;
m:\nipc\ether\wdether\ether.h 109: 
m:\nipc\ether\wdether\ether.h 110: typedef struct ARP_layer
m:\nipc\ether\wdether\ether.h 111: {
m:\nipc\ether\wdether\ether.h 112: short hardware,protocol;
m:\nipc\ether\wdether\ether.h 113: char hardlen;
m:\nipc\ether\wdether\ether.h 114: char protlen;
m:\nipc\ether\wdether\ether.h 115: short opcode;
m:\nipc\ether\wdether\ether.h 116: Ether_addr source_hard_addr;
m:\nipc\ether\wdether\ether.h 117: IP_addr    source_prot_addr;
m:\nipc\ether\wdether\ether.h 118: Ether_addr target_hard_addr;
m:\nipc\ether\wdether\ether.h 119: IP_addr    target_prot_addr;
m:\nipc\ether\wdether\ether.h 120: } ARP_layer;
m:\nipc\ether\wdether\ether.h 121: 
m:\nipc\ether\wdether\ether.h 122: typedef struct ARP_prot
m:\nipc\ether\wdether\ether.h 123: {
m:\nipc\ether\wdether\ether.h 124: Ether_layer M;
m:\nipc\ether\wdether\ether.h 125: ARP_layer A;
m:\nipc\ether\wdether\ether.h 126: } ARP_prot;
m:\nipc\ether\wdether\ether.h 127: 
m:\nipc\ether\wdether\ether.h 128: typedef struct TCP_prot
m:\nipc\ether\wdether\ether.h 129: {
m:\nipc\ether\wdether\ether.h 130: Ether_layer M;
m:\nipc\ether\wdether\ether.h 131: IP_layer I;
m:\nipc\ether\wdether\ether.h 132: TCP_layer T;
m:\nipc\ether\wdether\ether.h 133: unsigned char data[];
m:\nipc\ether\wdether\ether.h 134: } TCP_prot;
m:\nipc\ether\wdether\ether.h 135: 
m:\nipc\ether\wdether\ether.h 136: typedef struct IP_prot
m:\nipc\ether\wdether\ether.h 137: {
m:\nipc\ether\wdether\ether.h 138: Ether_layer M;
m:\nipc\ether\wdether\ether.h 139: IP_layer I;
m:\nipc\ether\wdether\ether.h 140: unsigned char data[];
m:\nipc\ether\wdether\ether.h 141: } IP_prot;
m:\nipc\ether\wdether\ether.h 142: 
m:\nipc\ether\wdether\ether.h 143: typedef struct record
m:\nipc\ether\wdether\ether.h 144: {
m:\nipc\ether\wdether\ether.h 145: short base;      
m:\nipc\ether\wdether\ether.h 146: short length;    
m:\nipc\ether\wdether\ether.h 147: char flags;      
m:\nipc\ether\wdether\ether.h 148: }record;
m:\nipc\ether\wdether\ether.h 149: 
m:\nipc\ether\wdether\ether.h 150: typedef struct seq_data
m:\nipc\ether\wdether\ether.h 151: {
m:\nipc\ether\wdether\ether.h 152: short ident;
m:\nipc\ether\wdether\ether.h 153: unsigned long seq;
m:\nipc\ether\wdether\ether.h 154: short length;
m:\nipc\ether\wdether\ether.h 155: char flags;
m:\nipc\ether\wdether\ether.h 156: char sent;
m:\nipc\ether\wdether\ether.h 157: unsigned char times_sent;     
m:\nipc\ether\wdether\ether.h 158: 
m:\nipc\ether\wdether\ether.h 159: short time_sent;
m:\nipc\ether\wdether\ether.h 160: char __far *data;
m:\nipc\ether\wdether\ether.h 161: struct seq_data __far *next;
m:\nipc\ether\wdether\ether.h 162: } seq_data;
m:\nipc\ether\wdether\ether.h 163: 
m:\nipc\ether\wdether\ether.h 164: 
m:\nipc\ether\wdether\ether.h 165: 
m:\nipc\ether\wdether\ether.h 166: 
m:\nipc\ether\wdether\ether.h 167: 
m:\nipc\ether\wdether\ether.h 168: 
m:\nipc\ether\wdether\ether.h 169: typedef struct connection
m:\nipc\ether\wdether\ether.h 170: {
m:\nipc\ether\wdether\ether.h 171: Ether_addr Edest;      
m:\nipc\ether\wdether\ether.h 172: 
m:\nipc\ether\wdether\ether.h 173: char state;            
m:\nipc\ether\wdether\ether.h 174: 
m:\nipc\ether\wdether\ether.h 175: 
m:\nipc\ether\wdether\ether.h 176: 
m:\nipc\ether\wdether\ether.h 177: 
m:\nipc\ether\wdether\ether.h 178: 
m:\nipc\ether\wdether\ether.h 179: 
m:\nipc\ether\wdether\ether.h 180: IP_layer I;            
m:\nipc\ether\wdether\ether.h 181: 
m:\nipc\ether\wdether\ether.h 182: TCP_layer T;           
m:\nipc\ether\wdether\ether.h 183: Pseudo_IP PI;          
m:\nipc\ether\wdether\ether.h 184: 
m:\nipc\ether\wdether\ether.h 185: char __far *inbuf;       
m:\nipc\ether\wdether\ether.h 186: short ihead,itail;     
m:\nipc\ether\wdether\ether.h 187: short base,hend;       
m:\nipc\ether\wdether\ether.h 188: 
m:\nipc\ether\wdether\ether.h 189: 
m:\nipc\ether\wdether\ether.h 190: 
m:\nipc\ether\wdether\ether.h 191: char status;           
m:\nipc\ether\wdether\ether.h 192: 
m:\nipc\ether\wdether\ether.h 193: 
m:\nipc\ether\wdether\ether.h 194: 
m:\nipc\ether\wdether\ether.h 195: 
m:\nipc\ether\wdether\ether.h 196: 
m:\nipc\ether\wdether\ether.h 197: 
m:\nipc\ether\wdether\ether.h 198: 
m:\nipc\ether\wdether\ether.h 199: 
m:\nipc\ether\wdether\ether.h 200: char flags;             
m:\nipc\ether\wdether\ether.h 201: 
m:\nipc\ether\wdether\ether.h 202: char retransmitting;
m:\nipc\ether\wdether\ether.h 203: 
m:\nipc\ether\wdether\ether.h 204: short last_packet_sent; 
m:\nipc\ether\wdether\ether.h 205: short Dwindow;          
m:\nipc\ether\wdether\ether.h 206: short last_ad;          
m:\nipc\ether\wdether\ether.h 207: 
m:\nipc\ether\wdether\ether.h 208: 
m:\nipc\ether\wdether\ether.h 209: 
m:\nipc\ether\wdether\ether.h 210: 
m:\nipc\ether\wdether\ether.h 211: unsigned short smth_dwind;       
m:\nipc\ether\wdether\ether.h 212: unsigned short dwind_var;         
m:\nipc\ether\wdether\ether.h 213: 
m:\nipc\ether\wdether\ether.h 214: 
m:\nipc\ether\wdether\ether.h 215: 
m:\nipc\ether\wdether\ether.h 216: 
m:\nipc\ether\wdether\ether.h 217: unsigned short srtt,sdev,rto;  
m:\nipc\ether\wdether\ether.h 218: unsigned short a_size;         
m:\nipc\ether\wdether\ether.h 219: 
m:\nipc\ether\wdether\ether.h 220: unsigned short p_received      
m:\nipc\ether\wdether\ether.h 221: ,rec_duplicats
m:\nipc\ether\wdether\ether.h 222: ,over_flows 
m:\nipc\ether\wdether\ether.h 223: ,transmits       
m:\nipc\ether\wdether\ether.h 224: ,retransmits;
m:\nipc\ether\wdether\ether.h 225: 
m:\nipc\ether\wdether\ether.h 226: short cur_rec,first_rec,last_rec;  
m:\nipc\ether\wdether\ether.h 227: 
m:\nipc\ether\wdether\ether.h 228: 
m:\nipc\ether\wdether\ether.h 229: 
m:\nipc\ether\wdether\ether.h 230: 
m:\nipc\ether\wdether\ether.h 231: 
m:\nipc\ether\wdether\ether.h 232: record __far *rec_list;
m:\nipc\ether\wdether\ether.h 233: struct connection __far *next;
m:\nipc\ether\wdether\ether.h 234: unsigned long connect_timer;
m:\nipc\ether\wdether\ether.h 235: seq_data __far *outstanding_recs;  
m:\nipc\ether\wdether\ether.h 236: 
m:\nipc\ether\wdether\ether.h 237: unsigned long  highest_seq;      
m:\nipc\ether\wdether\ether.h 238: 
m:\nipc\ether\wdether\ether.h 239: 
m:\nipc\ether\wdether\ether.h 240: unsigned long lowest_not_sent_seq;
m:\nipc\ether\wdether\ether.h 241: 
m:\nipc\ether\wdether\ether.h 242: short current_ident;             
m:\nipc\ether\wdether\ether.h 243: 
m:\nipc\ether\wdether\ether.h 244: 
m:\nipc\ether\wdether\ether.h 245: short packets;                   
m:\nipc\ether\wdether\ether.h 246: 
m:\nipc\ether\wdether\ether.h 247: short mult_packets;              
m:\nipc\ether\wdether\ether.h 248: 
m:\nipc\ether\wdether\ether.h 249: 
m:\nipc\ether\wdether\ether.h 250: unsigned char users;             
m:\nipc\ether\wdether\ether.h 251: 
m:\nipc\ether\wdether\ether.h 252: 
m:\nipc\ether\wdether\ether.h 253: } connection;
m:\nipc\ether\wdether\ether.h 254: 
m:\nipc\ether\wdether\ether.h 255: 
m:\nipc\ether\wdether\ether.h 256: 
m:\nipc\ether\wdether\ether.h 257: 
m:\nipc\ether\wdether\ether.h 258: 
m:\nipc\ether\wdether\ether.h 259: #pragma warn -rvl
m:\nipc\ether\wdether\ether.h 260: #pragma argsused
m:\nipc\ether\wdether\ether.h 261: 
m:\nipc\ether\wdether\ether.h 262: 
m:\nipc\ether\wdether\ether.h 263: connection __far * (__far openether)(char __far *source_address,char __far *dest_addr, short tsouce,short tdest,char opts);;
m:\nipc\ether\wdether\ether.h 264: #pragma argsused
m:\nipc\ether\wdether\ether.h 265: 
m:\nipc\ether\wdether\ether.h 266: unsigned short (__far sendether)(connection __far * ether,char __far *buffer,unsigned short length);;
m:\nipc\ether\wdether\ether.h 267: #pragma argsused
m:\nipc\ether\wdether\ether.h 268: 
m:\nipc\ether\wdether\ether.h 269: unsigned short (__far readether)(connection __far * ether,char __far *buffer,unsigned short maxlength,char options);;
m:\nipc\ether\wdether\ether.h 270: #pragma argsused
m:\nipc\ether\wdether\ether.h 271: short (__far close_connection)(connection __far * ether);;  
m:\nipc\ether\wdether\ether.h 272: #pragma argsused
m:\nipc\ether\wdether\ether.h 273: void (__far Ether_term)(connection __far * ether,short read);;
m:\nipc\ether\wdether\ether.h 274: #pragma argsused
m:\nipc\ether\wdether\ether.h 275: void (__far etherflush)(connection __far * ether);;
m:\nipc\ether\wdether\ether.h 276: #pragma warn .rvl
m:\nipc\ether\wdether\ether.h 277: 
m:\nipc\ether\wdether\ether.h 278: 
m:\nipc\ether\wdether\ether.h 279: 
\common.ipc\ether.h 2: 
\common.ipc\ether.h 3: 
multhand.c 6: 
multhand.c 7: 
multhand.c 8: 
multhand.c 9: 
multhand.c 10: 
multhand.c 11: 
multhand.c 12: 
multhand.c 13: 
multhand.c 14: char types[256];
multhand.c 15: 
multhand.c 16: typedef struct NodeHolder
multhand.c 17: {
multhand.c 18: Node __far *Node;
multhand.c 19: struct NodeHolder __far *next;
multhand.c 20: }NodeHolder;
multhand.c 21: 
multhand.c 22: 
multhand.c 23: 
multhand.c 24: 
multhand.c 25: 
multhand.c 26: 
multhand.c 27: 
multhand.c 28: 
multhand.c 29: typedef struct channel
multhand.c 30: {
multhand.c 31: char status;
multhand.c 32: char state;        
multhand.c 33: char type;         
multhand.c 34: char Transport;    
multhand.c 35: char Channel;      
multhand.c 36: Node __far *cur_node; 
multhand.c 37: void __far *output;   
multhand.c 38: NodeHolder __far *Next_node;  
multhand.c 39: struct channel __far *nextTrans,__far *priorTrans,
multhand.c 40: __far *priorChan,__far *nextChan; 
multhand.c 41: long trickle_time;
multhand.c 42: short idx,start;
multhand.c 43: char __far *Data;
multhand.c 44: short CLP,LPP;
multhand.c 45: }channel;
multhand.c 46: 
multhand.c 47: 
multhand.c 48: 
multhand.c 49: channel __far *first_chan,__far *currentC;
multhand.c 50: 
multhand.c 51: module __far *channel_processor;
multhand.c 52: 
multhand.c 53: void configure_handler()
multhand.c 54: {
multhand.c 55: 
multhand.c 56: 
multhand.c 57: 
multhand.c 58: }
multhand.c 59: 
multhand.c 60: void __far *open(char __far *state,char type)
multhand.c 61: {
multhand.c 62: }
multhand.c 63: 
multhand.c 64: char Process_node(char state,char type,Node __far *Node)
multhand.c 65: {
multhand.c 66: char data_read;
multhand.c 67: if (!currentC->output)
multhand.c 68: {
multhand.c 69: currentC->output=open(&state,type);
multhand.c 70: if (!currentC->output)
multhand.c 71: return(state);
multhand.c 72: }
multhand.c 73: switch(Node->Node.Rex&0xf)
multhand.c 74: {
multhand.c 75: case 0:
multhand.c 76: switch(state)
multhand.c 77: {
multhand.c 78: case 0:
multhand.c 79: if (Node->Tracking.Side==0)
multhand.c 80: {
multhand.c 81: slough_work(Node);
multhand.c 82: }
multhand.c 83: 
multhand.c 84: if (Node->Node.XOptions&0x0010)
multhand.c 85: {
multhand.c 86: 
multhand.c 87: flushline(currentC->output);
multhand.c 88: }
multhand.c 89: currentC->Data=Node->Data;
multhand.c 90: currentC->idx=0;
multhand.c 91: currentC->start=0;
multhand.c 92: currentC->trickle_time=0;
multhand.c 93: if (Node->Node.Options&0x1000)
multhand.c 94: {
multhand.c 95: if (type==0)
multhand.c 96: ((card_entry __far *)currentC->output)->Flow_control&=
multhand.c 97: ~(0x20);
multhand.c 98: if (!(Node->Node.Options&0x4000))
multhand.c 99: {
multhand.c 100: 
multhand.c 101: 
multhand.c 102: case 1:
multhand.c 103: if (readascii(currentC->output,&data_read))
multhand.c 104: {
multhand.c 105: if (ECHO(Node))
multhand.c 106: write(&data_read,1,currentC->output);
multhand.c 107: if (!data_read)
multhand.c 108: return(1);
multhand.c 109: }
multhand.c 110: else
multhand.c 111: return(1);
multhand.c 112: 
multhand.c 113: currentC->Data[currentC->idx++]=data_read;
multhand.c 114: gettime(&currentC->trickle_time);
multhand.c 115: }
multhand.c 116: 
multhand.c 117: if (Node->Node.Options&0x2000)
multhand.c 118: { 
multhand.c 119: if (!(Node->Node.Options&0x0200))
multhand.c 120: {
multhand.c 121: case 2:
multhand.c 122: if (readascii(currentC->output,currentC->Data+currentC->idx))
multhand.c 123: {
multhand.c 124: if (ECHO(Node))
multhand.c 125: write(currentC->Data+currentC->idx,1,currentC->output);
multhand.c 126: currentC->idx++;
multhand.c 127: if (currentC->idx<2)
multhand.c 128: return(2);
multhand.c 129: }
multhand.c 130: else
multhand.c 131: return(2);
multhand.c 132: 
multhand.c 133: if (*((short __far *)currentC->Data)==0x2424)
multhand.c 134: {
multhand.c 135: Node->Return.Status=0x0020;
multhand.c 136: complete_Node(Node);
multhand.c 137: return(-1);
multhand.c 138: }
multhand.c 139: }
multhand.c 140: if (!currentC->idx)
multhand.c 141: {
multhand.c 142: case 3:
multhand.c 143: if (readascii(currentC->output,currentC->Data+currentC->idx))
multhand.c 144: {
multhand.c 145: if (ECHO(Node))
multhand.c 146: write(currentC->Data+currentC->idx,1,currentC->output);
multhand.c 147: currentC->idx++;
multhand.c 148: }
multhand.c 149: else
multhand.c 150: return(3);
multhand.c 151: }
multhand.c 152: if (currentC->Data[0]==3||currentC->Data[0]==7)
multhand.c 153: {
multhand.c 154: short block_size;
multhand.c 155: short tsize;
multhand.c 156: case 4:
multhand.c 157: if (readascii(currentC->output,currentC->Data+currentC->idx++)&&ECHO(Node))
multhand.c 158: {
multhand.c 159: write(currentC->Data+currentC->idx,1,currentC->output);
multhand.c 160: if (currentC->idx<4)
multhand.c 161: return(4);
multhand.c 162: }
multhand.c 163: else
multhand.c 164: return(4);
multhand.c 165: case 5:
multhand.c 166: block_size=intswap(*(short __far*)(currentC->Data+2));
multhand.c 167: currentC->idx+=(tsize=readbin(currentC->Data+currentC->idx,
multhand.c 168: block_size-currentC->idx,
multhand.c 169: currentC->output));
multhand.c 170: if (tsize&&ECHO(Node))
multhand.c 171: {
multhand.c 172: write(currentC->Data+currentC->idx-tsize,
multhand.c 173: tsize,
multhand.c 174: currentC->output);
multhand.c 175: }
multhand.c 176: if (currentC->idx<block_size)
multhand.c 177: return(5);
multhand.c 178: }
multhand.c 179: else
multhand.c 180: {
multhand.c 181: abort_node(Node,0x7726);  
multhand.c 182: }
multhand.c 183: }
multhand.c 184: else
multhand.c 185: {  
multhand.c 186: short tsize;
multhand.c 187: case 6:
multhand.c 188: currentC->idx+= (tsize=readbin(currentC->Data+currentC->idx,
multhand.c 189: Node->Node.Byte_count-currentC->idx,
multhand.c 190: currentC->output));
multhand.c 191: if (tsize&&ECHO(Node))
multhand.c 192: {
multhand.c 193: write(currentC->Data+currentC->idx-tsize,
multhand.c 194: tsize,
multhand.c 195: currentC->output);
multhand.c 196: }
multhand.c 197: 
multhand.c 198: if (currentC->idx<Node->Node.Byte_count)
multhand.c 199: return(6);
multhand.c 200: }
multhand.c 201: }
multhand.c 202: else
multhand.c 203: {
multhand.c 204: char tchar,gotone=0;
multhand.c 205: if (type==0)
multhand.c 206: ((card_entry __far *)currentC->output)->Flow_control|=(0x20);
multhand.c 207: if (!(Node->Node.Options&0x4000))
multhand.c 208: {
multhand.c 209: write("\r\n",2,currentC->output);
multhand.c 210: currentC->CLP++;
multhand.c 211: }
multhand.c 212: else
multhand.c 213: {
multhand.c 214: case 7:
multhand.c 215: if (readascii(currentC->output,&tchar)&&ECHO(Node))
multhand.c 216: {
multhand.c 217: write(&tchar,1,currentC->output);
multhand.c 218: if (!tchar)
multhand.c 219: return(7);
multhand.c 220: }
multhand.c 221: else
multhand.c 222: return(7);
multhand.c 223: gotone=(!0);
multhand.c 224: }
multhand.c 225: if (Node->Node.Options&0x2000)
multhand.c 226: { 
multhand.c 227: case 8:
multhand.c 228: if (currentC->idx)
multhand.c 229: {
multhand.c 230: if (currentC->Data[0]==3||currentC->Data[0]==7)
multhand.c 231: return(4);
multhand.c 232: }
multhand.c 233: if (!gotone)
multhand.c 234: {
multhand.c 235: if (readascii(currentC->output,&tchar)&&ECHO(Node))
multhand.c 236: {
multhand.c 237: write(&tchar,1,currentC->output);
multhand.c 238: }
multhand.c 239: else
multhand.c 240: return(8);
multhand.c 241: }
multhand.c 242: else
multhand.c 243: gotone=0;
multhand.c 244: switch(tchar)
multhand.c 245: {
multhand.c 246: case '\r':
multhand.c 247: case 0   :
multhand.c 248: if (!(Node->Node.Options&0x0200)&&currentC->idx)
multhand.c 249: {
multhand.c 250: currentC->idx--;
multhand.c 251: while (currentC->Data[currentC->idx]==' ')
multhand.c 252: currentC->idx--;
multhand.c 253: currentC->idx++;
multhand.c 254: }
multhand.c 255: if ((currentC->idx&1)&&
multhand.c 256: (currentC->idx+1<Node->Node.Byte_count))
multhand.c 257: {
multhand.c 258: currentC->Data[currentC->idx++]=' ';
multhand.c 259: Node->Return.XStatus=-1;
multhand.c 260: }
multhand.c 261: if (!(Node->Node.Options&0x0200)&&
multhand.c 262: (currentC->idx+2<Node->Node.Byte_count))
multhand.c 263: {
multhand.c 264: currentC->Data[currentC->idx++]=0;
multhand.c 265: currentC->Data[currentC->idx++]=0;
multhand.c 266: Node->Return.XStatus-=2;
multhand.c 267: }
multhand.c 268: Node->Return.XStatus&=0x1ff;
multhand.c 269: return(9);
multhand.c 270: case '\n':break;
multhand.c 271: case '\b':if (currentC->idx)
multhand.c 272: currentC->idx--;
multhand.c 273: break;
multhand.c 274: case 127:currentC->idx=0;
multhand.c 275: write("\r\n",2,currentC->output);
multhand.c 276: currentC->CLP++;
multhand.c 277: break;
multhand.c 278: default:currentC->Data[currentC->idx++]=tchar;
multhand.c 279: }
multhand.c 280: if (currentC->idx<Node->Node.Byte_count)
multhand.c 281: return(8);
multhand.c 282: case 9:
multhand.c 283: if ((currentC->idx>2)&&!(Node->Node.Options&0x0200))
multhand.c 284: if (*(short __far *)currentC->Data==0x2424)
multhand.c 285: Node->Return.Status|=0x0020;
multhand.c 286: }
multhand.c 287: else
multhand.c 288: { 
multhand.c 289: case 10:
multhand.c 290: if (!gotone)
multhand.c 291: if (readascii(currentC->output,&tchar)&&ECHO(Node))
multhand.c 292: write(&tchar,1,currentC->output);
multhand.c 293: else
multhand.c 294: return(10);
multhand.c 295: else
multhand.c 296: gotone=0;
multhand.c 297: currentC->Data[currentC->idx++]=tchar;
multhand.c 298: if (!(Node->Node.Options&0x0080))
multhand.c 299: {
multhand.c 300: if (tchar==(Node->Node.Options&0x007f))
multhand.c 301: {
multhand.c 302: Node->Return.XStatus=(-1)&0x1ff;
multhand.c 303: break;
multhand.c 304: }
multhand.c 305: }
multhand.c 306: if (currentC->idx<Node->Node.Byte_count)
multhand.c 307: return(10);
multhand.c 308: }
multhand.c 309: Node->Return.Byte_count=currentC->idx;
multhand.c 310: }
multhand.c 311: case 11:
multhand.c 312: while (Node->Tracking.Status&(0x08|0x40))
multhand.c 313: return(11);
multhand.c 314: 
multhand.c 315: Node->Tracking.Side=1;
multhand.c 316: Node->Node.FPI++;
multhand.c 317: Node->Return.Opcode=1;
multhand.c 318: break;
multhand.c 319: }
multhand.c 320: case 1:
multhand.c 321: {
multhand.c 322: unsigned short count_sent=1;
multhand.c 323: {
multhand.c 324: if (Node->Node.Options&0x1000)
multhand.c 325: {
multhand.c 326: if (type==0)
multhand.c 327: ((card_entry __far *)currentC->output)->Flow_control&=~(0x40);
multhand.c 328: if (Node->Node.Options&0x2000)
multhand.c 329: {
multhand.c 330: if (currentC->Data[0]==3||currentC->Data[0]==7)
multhand.c 331: {
multhand.c 332: unsigned short output_count=*((short *)(currentC->Data+2));
multhand.c 333: if (output_count>Node->Node.Byte_count)
multhand.c 334: output_count=Node->Node.Byte_count;
multhand.c 335: write(currentC->Data,output_count,currentC->output);
multhand.c 336: Node->Return.Byte_count=output_count;
multhand.c 337: }
multhand.c 338: else
multhand.c 339: {
multhand.c 340: Node->Return.Status=0x8000|0x0080;
multhand.c 341: Node->Return.Byte_count=0;
multhand.c 342: }
multhand.c 343: }
multhand.c 344: else
multhand.c 345: {
multhand.c 346: write(currentC->Data,
multhand.c 347: (short)Node->Node.Byte_count,
multhand.c 348: currentC->output);
multhand.c 349: Node->Return.Byte_count=Node->Node.Byte_count;
multhand.c 350: }
multhand.c 351: }
multhand.c 352: else
multhand.c 353: {
multhand.c 354: unsigned short idx,bias=0;
multhand.c 355: if (type==0)
multhand.c 356: ((card_entry __far *)currentC->output)->Flow_control|=(0x40);
multhand.c 357: if (Node->Node.Options&0x2000)
multhand.c 358: {
multhand.c 359: switch(currentC->Data[0])
multhand.c 360: {
multhand.c 361: case '0':
multhand.c 362: write("\r\n\n",3,currentC->output);
multhand.c 363: currentC->CLP+=2;
multhand.c 364: break;
multhand.c 365: case '1':
multhand.c 366: case '-':
multhand.c 367: write_formfeed(Node,currentC->output);
multhand.c 368: break;
multhand.c 369: case '+':
multhand.c 370: write("\r",1,currentC->output);
multhand.c 371: break;
multhand.c 372: default:
multhand.c 373: write("\r\n",2,currentC->output);
multhand.c 374: currentC->CLP++;
multhand.c 375: break;
multhand.c 376: }
multhand.c 377: currentC->Data++;
multhand.c 378: bias++;
multhand.c 379: Node->Node.Byte_count--;
multhand.c 380: if (currentC->Data[1]==2)
multhand.c 381: {
multhand.c 382: currentC->Data++;
multhand.c 383: bias++;
multhand.c 384: Node->Node.Byte_count--;
multhand.c 385: }
multhand.c 386: for (idx=0;
multhand.c 387: (idx<Node->Node.Byte_count)&&currentC->Data[idx];
multhand.c 388: idx++);
multhand.c 389: }
multhand.c 390: else
multhand.c 391: {
multhand.c 392: unsigned short terminator;
multhand.c 393: 
multhand.c 394: if (Node->Node.Options&0x0080)
multhand.c 395: terminator=256;
multhand.c 396: else
multhand.c 397: terminator=Node->Node.Options&0x007f;
multhand.c 398: 
multhand.c 399: for (idx=0;
multhand.c 400: (idx<Node->Node.Byte_count)&&
multhand.c 401: currentC->Data[idx]!=terminator;
multhand.c 402: idx++);
multhand.c 403: }
multhand.c 404: write(currentC->Data,idx,currentC->output);
multhand.c 405: Node->Return.Byte_count=idx+bias;
multhand.c 406: }
multhand.c 407: destory();
multhand.c 408: }
multhand.c 409: while (Node->Tracking.Status&(0x08|0x40))
multhand.c 410: Relinquish(0L);
multhand.c 411: Node->Return.Opcode=1;
multhand.c 412: Node->Return.Byte_count=count_sent;
multhand.c 413: Node->Node.FPI++;
multhand.c 414: }
multhand.c 415: break;
multhand.c 416: case 2:
multhand.c 417: write_formfeed(Node,currentC->output);
multhand.c 418: while (Node->Tracking.Status&(0x08|0x40))
multhand.c 419: Relinquish(0L);
multhand.c 420: Node->Return.Opcode=1;
multhand.c 421: Node->Return.Byte_count=0;
multhand.c 422: Node->Return.Status=0x0020|0x0010;
multhand.c 423: Node->Node.FPI=0;
multhand.c 424: break;
multhand.c 425: case 4:
multhand.c 426: while (Node->Tracking.Status&(0x08|0x40))
multhand.c 427: Relinquish(0L);
multhand.c 428: Node->Return.Opcode=1;
multhand.c 429: Node->Return.Byte_count=0;
multhand.c 430: Node->Return.Status=0x0020|0x0010;
multhand.c 431: 
multhand.c 432: break;
multhand.c 433: case 3:
multhand.c 434: while (Node->Tracking.Status&(0x08|0x40))
multhand.c 435: Relinquish(0L);
multhand.c 436: Node->Return.Opcode=1;
multhand.c 437: Node->Return.Byte_count=0;
multhand.c 438: Node->Return.Status=0x0020|0x0010;
multhand.c 439: break;
multhand.c 440: case 6:
multhand.c 441: write_formfeed(Node,currentC->output);
multhand.c 442: while (Node->Tracking.Status&(0x08|0x40))
multhand.c 443: Relinquish(0L);
multhand.c 444: Node->Return.Opcode=1;
multhand.c 445: Node->Return.Byte_count=0;
multhand.c 446: Node->Return.Status=0x0020;
multhand.c 447: Node->Node.FPI++;
multhand.c 448: break;
multhand.c 449: case 5:
multhand.c 450: write("\r\n",2,currentC->output);
multhand.c 451: currentC->CLP++;
multhand.c 452: while (Node->Tracking.Status&(0x08|0x40))
multhand.c 453: Relinquish(0L);
multhand.c 454: Node->Return.Opcode=1;
multhand.c 455: Node->Return.Byte_count=0;
multhand.c 456: Node->Node.FPI++;
multhand.c 457: break;
multhand.c 458: case 7:
multhand.c 459: write("\r\n\n$$",5,currentC->output);
multhand.c 460: currentC->CLP=0;
multhand.c 461: while (Node->Tracking.Status&(0x08|0x40))
multhand.c 462: Relinquish(0L);
multhand.c 463: Node->Return.Opcode=1;
multhand.c 464: Node->Return.Byte_count=0;
multhand.c 465: Node->Node.FPI++;
multhand.c 466: break;
multhand.c 467: case 8:
multhand.c 468: if (Node->Node.XOptions&0x0008)
multhand.c 469: {
multhand.c 470: closechan(currentC->output);
multhand.c 471: currentC->output=0L;
multhand.c 472: }
multhand.c 473: Node->Return.Byte_count=0;
multhand.c 474: Node->Node.FPI++;
multhand.c 475: Node->Return.Opcode=1;
multhand.c 476: break;
multhand.c 477: }
multhand.c 478: if (!currentC->CLP&&!(Node->Return.Status&0x8000))
multhand.c 479: Node->Return.Status|=0x0020;
multhand.c 480: 
multhand.c 481: if (!(Node->Tracking.Status&0x04))
multhand.c 482: {
multhand.c 483: Node->Tracking.Status|=0x04;
multhand.c 484: while (Node->Tracking.Status&0x40)
multhand.c 485: {
multhand.c 486: Relinquish(0L);
multhand.c 487: }
multhand.c 488: if (!(Node->Tracking.Status&0x08))  
multhand.c 489: {
multhand.c 490: Node->Tracking.Status|=0x08;
multhand.c 491: Export(Node->Tracking.Source,Node);
multhand.c 492: }
multhand.c 493: }
multhand.c 494: return(-1);
multhand.c 495: 
multhand.c 496: }
multhand.c 497: 
multhand.c 498: void dequeue(channel __far *Channel)
multhand.c 499: {
multhand.c 500: 
multhand.c 501: 
multhand.c 502: NodeHolder __far *next;
multhand.c 503: if (Channel->Next_node)
multhand.c 504: {
multhand.c 505: Channel->cur_node=Channel->Next_node->Node;
multhand.c 506: next=Channel->Next_node->next;
multhand.c 507: Free(Channel->Next_node);
multhand.c 508: Channel->Next_node=next;
multhand.c 509: Channel->state=0;
multhand.c 510: }
multhand.c 511: }
multhand.c 512: 
multhand.c 513: void oprocess_node(Node __far *what,short me,channel __far *config)
multhand.c 514: {
multhand.c 515: 
multhand.c 516: short temp;
multhand.c 517: char __far *opptr;
multhand.c 518: 
multhand.c 519: 
multhand.c 520: 
multhand.c 521: 
multhand.c 522: 
multhand.c 523: 
multhand.c 524: 
multhand.c 525: 
multhand.c 526: 
multhand.c 527: 
multhand.c 528: 
multhand.c 529: if ((config->status&0b0010)==0b0010)
multhand.c 530: {
multhand.c 531: if (!output[me>>1])
multhand.c 532: {
multhand.c 533: (long)output[me>>1]=1L;  
multhand.c 534: output[me>>1]
multhand.c 535: =openether(my_node->Node.Info.Ether.SourceIP,
multhand.c 536: my_node->Node.Info.Ether.DestIP,
multhand.c 537: intswap(my_node->Node.Info.Ether.SourceTCP),
multhand.c 538: intswap(my_node->Node.Info.Ether.DestTCP),1);
multhand.c 539: 
multhand.c 540: }
multhand.c 541: 
multhand.c 542: 
multhand.c 543: 
multhand.c 544: 
multhand.c 545: 
multhand.c 546: 
multhand.c 547: 
multhand.c 548: 
multhand.c 549: }
multhand.c 550: if ((config->status&0b0100)==0b0100)
multhand.c 551: {
multhand.c 552: breaks.Transport=Node->Node.Transport;
multhand.c 553: breaks.SubChannel=Node->Node.Channel;
multhand.c 554: breaks.TCL_idx=Node->Node.Info.Comm.TCL_index;
multhand.c 555: breaks.TCL_avail=Node->Node.Info.Comm.TCL_data;
multhand.c 556: if (!currentC->outputs.open||
multhand.c 557: (currentC->outputs.control&0xf0)!=(Node->Node.Info.Comm.Control&0xf0)||
multhand.c 558: currentC->outputs.FrStPa!=Node->Node.Info.Comm.Frame_Stop_Par||
multhand.c 559: currentC->outputs.Mode!=Node->Node.Info.Comm.Mode||
multhand.c 560: currentC->outputs.baud!=Node->Node.Info.Comm.Baud)
multhand.c 561: {
multhand.c 562: currentC->outputs.LPP=Node->Node.Info.Comm.Top_of_form;
multhand.c 563: currentC->CLP=0;
multhand.c 564: currentC->outputs.FrStPa=Node->Node.Info.Comm.Frame_Stop_Par;
multhand.c 565: currentC->outputs.Mode=Node->Node.Info.Comm.Mode;
multhand.c 566: currentC->outputs.baud=Node->Node.Info.Comm.Baud;
multhand.c 567: if (bauds[Node->Node.Info.Comm.Baud]>0)
multhand.c 568: {
multhand.c 569: currentC->outputs.config.baud=bauds[Node->Node.Info.Comm.Baud];
multhand.c 570: }
multhand.c 571: else
multhand.c 572: {
multhand.c 573: abort_node(Node,0xcbd);
multhand.c 574: }
multhand.c 575: currentC->outputs.config.rx_size=Node->Node.Info.Comm.Frame_Stop_Par&0xc0;
multhand.c 576: currentC->outputs.config.tx_size=(Node->Node.Info.Comm.Frame_Stop_Par&0xc0)>>1;
multhand.c 577: currentC->outputs.config.stop_par=((Node->Node.Info.Comm.Frame_Stop_Par&0x30)>>2)|
multhand.c 578: ((Node->Node.Info.Comm.Frame_Stop_Par&0x06)>>1);
multhand.c 579: currentC->outputs.config.flow=0x20|0x40|
multhand.c 580: (Node->Node.Info.Comm.Control&0xf0)>>4;;
multhand.c 581: currentC->outputs.open=openchan(currentC->output,&currentC->outputs.config);
multhand.c 582: }
multhand.c 583: currentC->outputs.control=Node->Node.Info.Comm.Control;
multhand.c 584: if (!currentC->outputs.open)
multhand.c 585: {
multhand.c 586: abort_node(Node,0x596e);  
multhand.c 587: }
multhand.c 588: }
multhand.c 589: }
multhand.c 590: 
multhand.c 591: void Process_nodes()
multhand.c 592: {
multhand.c 593: channel __far *currentT,__far *next;
multhand.c 594: channel_processor=(*my_tcbptr);
multhand.c 595: 
multhand.c 596: while (first_chan)
multhand.c 597: {
multhand.c 598: currentT=first_chan;
multhand.c 599: while (currentT)
multhand.c 600: {
multhand.c 601: currentC=currentT;
multhand.c 602: while (currentC)
multhand.c 603: {
multhand.c 604: if (currentC->state!=-1)
multhand.c 605: currentC->state=Process_node(currentC->state,
multhand.c 606: currentC->type,
multhand.c 607: currentC->cur_node);
multhand.c 608: if (currentC->state==-1)
multhand.c 609: dequeue(currentC);
multhand.c 610: currentC=currentC->nextChan;
multhand.c 611: }
multhand.c 612: currentT=currentT->nextTrans;
multhand.c 613: }
multhand.c 614: Relinquish(0L);
multhand.c 615: }
multhand.c 616: perish();
multhand.c 617: }
multhand.c 618: 
multhand.c 619: 
multhand.c 620: 
multhand.c 621: 
multhand.c 622: 
multhand.c 623: 
multhand.c 624: 
multhand.c 625: 
multhand.c 626: void activate(char Transport,char Channel,Node __far *Node)
multhand.c 627: {
multhand.c 628: channel __far *curchan=first_chan,__far *tempchan,__far *priorchan;
multhand.c 629: char lost=0;
multhand.c 630: while (curchan)
multhand.c 631: {
multhand.c 632: if (Transport<curchan->Transport)
multhand.c 633: {
multhand.c 634: priorchan=curchan;
multhand.c 635: curchan=curchan->nextTrans;
multhand.c 636: lost=1;
multhand.c 637: }
multhand.c 638: else
multhand.c 639: if (Transport==curchan->Transport)
multhand.c 640: {
multhand.c 641: if (Channel<curchan->Channel)
multhand.c 642: {
multhand.c 643: priorchan=curchan;
multhand.c 644: curchan=curchan->nextChan;
multhand.c 645: lost=4;
multhand.c 646: }
multhand.c 647: else
multhand.c 648: if (Channel==curchan->Channel)
multhand.c 649: {
multhand.c 650: 
multhand.c 651: 
multhand.c 652: lost=2;
multhand.c 653: break;
multhand.c 654: }
multhand.c 655: else
multhand.c 656: {
multhand.c 657: 
multhand.c 658: 
multhand.c 659: lost=3;
multhand.c 660: break;
multhand.c 661: }
multhand.c 662: }
multhand.c 663: else
multhand.c 664: {
multhand.c 665: 
multhand.c 666: 
multhand.c 667: lost=5;
multhand.c 668: break;
multhand.c 669: }
multhand.c 670: }
multhand.c 671: while (!(tempchan=Allocate(sizeof(channel))))
multhand.c 672: {
multhand.c 673: Relinquish(0L);
multhand.c 674: }
multhand.c 675: switch(lost)
multhand.c 676: {
multhand.c 677: case 0: 
multhand.c 678: first_chan=tempchan;
multhand.c 679: first_chan->priorTrans=first_chan->nextTrans=0L;
multhand.c 680: first_chan->priorChan=first_chan->priorChan=0L;
multhand.c 681: break;
multhand.c 682: case 1: 
multhand.c 683: tempchan->nextChan=tempchan->priorChan=0L;
multhand.c 684: tempchan->nextTrans=curchan;
multhand.c 685: tempchan->priorTrans=curchan->priorTrans;
multhand.c 686: if (curchan->priorTrans)
multhand.c 687: curchan->priorTrans->nextTrans=tempchan;
multhand.c 688: else
multhand.c 689: first_chan=tempchan;
multhand.c 690: curchan->priorTrans=tempchan;
multhand.c 691: break;
multhand.c 692: case 2: 
multhand.c 693: Free(tempchan);
multhand.c 694: tempchan=0L;
multhand.c 695: if (curchan->state==-1)
multhand.c 696: {
multhand.c 697: curchan->state=0;
multhand.c 698: curchan->cur_node=Node;
multhand.c 699: }
multhand.c 700: else
multhand.c 701: {
multhand.c 702: NodeHolder __far *nodehold;
multhand.c 703: nodehold=Allocate(sizeof(NodeHolder));
multhand.c 704: nodehold->Node=Node;
multhand.c 705: nodehold->next=0L;
multhand.c 706: if (curchan->Next_node)
multhand.c 707: {
multhand.c 708: NodeHolder __far *curhold;
multhand.c 709: curhold=curchan->Next_node;
multhand.c 710: while (curhold->next)
multhand.c 711: curhold=curhold->next;
multhand.c 712: curhold->next=nodehold;
multhand.c 713: }
multhand.c 714: else
multhand.c 715: curchan->Next_node=nodehold;
multhand.c 716: }
multhand.c 717: break;
multhand.c 718: case 3:  
multhand.c 719: priorchan->nextChan=tempchan;
multhand.c 720: tempchan->priorChan=priorchan;
multhand.c 721: tempchan->nextChan=0L;
multhand.c 722: tempchan->priorTrans=tempchan->nextTrans=0L;
multhand.c 723: break;
multhand.c 724: case 4:  
multhand.c 725: tempchan->priorTrans=curchan->priorTrans;
multhand.c 726: if (curchan->priorTrans)
multhand.c 727: curchan->priorTrans->nextTrans=tempchan;
multhand.c 728: else
multhand.c 729: if (!curchan->priorChan)
multhand.c 730: first_chan=tempchan;
multhand.c 731: tempchan->nextTrans=curchan->nextTrans;
multhand.c 732: if (curchan->nextTrans)
multhand.c 733: curchan->nextTrans->nextTrans=tempchan;
multhand.c 734: curchan->nextTrans=0L;
multhand.c 735: tempchan->nextChan=curchan;
multhand.c 736: tempchan->priorChan=curchan->priorChan;
multhand.c 737: break;
multhand.c 738: case 5:  
multhand.c 739: priorchan->nextTrans=tempchan;
multhand.c 740: tempchan->priorTrans=priorchan;
multhand.c 741: tempchan->nextTrans=0L;
multhand.c 742: tempchan->nextChan=tempchan->priorChan=0L;
multhand.c 743: break;
multhand.c 744: }
multhand.c 745: if (tempchan)
multhand.c 746: {
multhand.c 747: tempchan->state=0;
multhand.c 748: tempchan->type=types[Transport];
multhand.c 749: tempchan->Transport=Transport;
multhand.c 750: tempchan->Channel=Channel;
multhand.c 751: tempchan->cur_node=Node;
multhand.c 752: tempchan->Next_node=0L;
multhand.c 753: }
multhand.c 754: if (!channel_processor)
multhand.c 755: {
multhand.c 756: if (fork(1))
multhand.c 757: Porcess_nodes();
multhand.c 758: }
multhand.c 759: }
multhand.c 760: 
multhand.c 761: void main()
multhand.c 762: {
multhand.c 763: Node __far *received;
multhand.c 764: configure_handler();
multhand.c 765: while (1)
multhand.c 766: {
multhand.c 767: received=Import(((*my_tcbptr)->device_name));
multhand.c 768: activate(received->Node.Transport,received->Node.Channel,received);
multhand.c 769: }
multhand.c 770: }
multhand.c 771: 
multhand.c 772: 