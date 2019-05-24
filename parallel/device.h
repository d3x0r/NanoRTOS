


typedef struct stdprn_tag {
	void (far *Close)( struct stdprn_tag * );
	void (far *Reset)( struct stdprn_tag * );
	int  (far *Read)( struct stdprn_tag *, char *pointer, int size );
	int  (far *Write)( struct stdprn_tag *, char *pointer, int size );
	void (far *Enter)( struct stdprn_tag * );
	void (far *Leave)( struct stdprn_tag * );
	void (far *EnableStatus)( struct stdprn_tag * );
} STDPRN, *PSTDPRN;

