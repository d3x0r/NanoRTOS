/* The following macro defines are used for bit maniulations.

  arrayvalue,arrayindex,
  bitconstant,remaindermask  are all used only as interanls to the other
                             defines for ease of definition.

  testbit(bitlist,bit)      is passed a list of bits (bitlist) and a bit
                            number from the absolute left of the list.
                            it returns a 1 if the bit is on and a 0 if the
                            bit is off.

  setbit(bitlist,bit)       turns on the bit specified within the bitlist.
                            Its result is the value of the element of the
                            list in which the bit was contained, after
                            turning on the bit.

  resetbit(bitlist,bit)     turns off the bit specified within the bitlist.
                            Its result is the value of the element of the
                            list in which the bit was contained, after
                            turning off the bit.

  changebit(bitlist,bit)    Changes the bit to the opposite state from where
                            it was.  Ie.  a 1 will become a 0 and vice versa.
                            Its result is the value of the element after
                            the bit had been flipped.

  equbit(bitlist,bit,value)  sets the bit specified within the bit array
                             to the value specified.  Its result is the
                             value of the element after the modification
                             has been made.

  testsetbit(bitlist,bit)    This define checks the condition of the bit
                             prior to modification, and then sets the
                             bit on.  Its result is the state of the bit
                             prior to modification.  Ie. 1 if bit was on
                             and 0 if bit was off.

  testresetbit(bitlist,bit)  This define checks the condition of the bit
                             prior to modification, and then resets the
                             bit to off.  Its result is the state of the
                             bit prior to modification.  Ie. 1 if bit was
                             on and 0 if bit was off.

  testchangebit(bitlist,bit) This define checks the condition of the bit
                             prior to modification, and then flips the
                             bit to its opposite state.  Its result is the
                             state of the bit prior to modification.  Ie.
                             1 if bit was on and 0 if bit was off. */


#define arrayvalue(array,bit) (*(array+arrayindex(array,bit)))

#define arrayindex(array,bit) (bit/(sizeof(array[0])*8))
#define bitconstant(array)   ( (unsigned)(                              \
                                          1L<< (                        \
                                                (sizeof(array[0])*8) -1 \
                                              )                         \
                                         ))                             \

#define remaindermask(array)  ((sizeof(array[0])*8)-1)


#define testbit(bitlist,bit) ((arrayvalue(bitlist,bit)&                   \
                              bitconstant(bitlist)>>                      \
                                (bit&remaindermask(bitlist))              \
                             )?1:0)


#define setbit(bitlist,bit)  (                                        \
                                (arrayvalue(bitlist,bit)|=            \
                                   (bitconstant(bitlist)>>            \
                                     (bit&remaindermask(bitlist)))    \
                                )                                     \
                             )

#define resetbit(bitlist,bit) (                                        \
                                 (arrayvalue(bitlist,bit)&=            \
                                   ~(bitconstant(bitlist)>>            \
                                      (bit&remaindermask(bitlist)))    \
                                 )                                     \
                              )

#define changebit(bitlist,bit) (                                       \
                                 (arrayvalue(bitlist,bit)^=            \
                                   ~(bitconstant(bitlist)>>            \
                                      (bit&remaindermask(bitlist)))    \
                                 )                                     \
                               )

#define equbit(bitlist,bit,value) ((value)?setbit(bitlist,bit):        \
                                          resetbit(bitlist,bit))

#define testsetbit(bitlist,bit) ((testbit(bitlist,bit))?(setbit(bitlist,bit)?1:1): \
                                                       setbit(bitlist,bit)&0)

#define testresetbit(bitlist,bit) ((testbit(bitlist,bit))?(resetbit(bitlist,bit)?1:1): \
                                                       resetbit(bitlist,bit)&0)

#define testchangebit(bitlist,bit) ((testbit(bitlist,bit))?(changebit(bitlist,bit)?1:1): \
                                                       changebit(bitlist,bit)&0)

