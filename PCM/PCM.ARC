  This is the new proposed archetecture for PC Mynah C.00.  This version
will be implemented under Panther OS.  Configuration files will either
be text based or a compiled version of text.  External Protocols will be
supported.  Everything that is not nessecary will be swapped to disk on
a swap.
  Terminal Emulation files should be perferably user programable so that
they can be field updated.  Therefore they need to be based on a
structure that can be compiled and inputed as norm.  Each window could
have a different translator.
  Behind the scenes look will be a wiring between an input device, a
translator and an output window.  Multiple sessions could be run at the
same time then.  The keyboard will of course focus on the attentive
window.  Certain keys will bring up the main menu and certain other
global functions.  Menus could be left up and merely turn the attention
to another window to access it.  Each session will have to be either
created when it comes up, or they have to be configured outside to be
set-up.  I beleive I could probably save what I have active upon an exit
and restore everything to that state.

  Peices and parts that I have availiable now.
      Comm
      Ether

        (translators)

      Video
      Keyboard



new desision.  Objects as translation filter boxes.  Wire then in so that
you call the translator with availiable incoming characters.  so, a switch
board manager is pulling from the connections it made to the outside world
on the behalf of a requestor, and is piping it actively to the screen output
that the requestor specified as the destination.  Therefore there are three
types of programs in this set.  One someone to decide to make a connection,
and operator if you will.  One will become a pump from passive to active,
one thread per pump will run here.  One will be passive in itself, and
be merely a routine that will process the character(s).

   Process_data(dimensions,buffer,...)
     dimensions-  specifies how the buffer is laid out.
              0 - just character
              1 - a line of characters
              2 - an array of characters  (usable?)
              3 - a cube of characters    (usable?)

   Source(*Device_structure)
   Output(*Device_structure)
     Device_structure
     {
       type- character describing what kind of device this is
       union
       {
         device specific sturctures.
       }parameters;
     }

   then can we multiply split the information?  Yes we can.  We can define
     another dest from a single source.  but we cannot specitfy one com channel
     to go to multiple translators.

   A translator looks at the character to process, and either accepts it or
   passes it to the next translator in the list.  All translation familys
   should be ended with a glass-tty device so that the output is at least
   output.  The keyboard backfeed must also be specified.



      Source Pool of Info
      ToSource
      ToDest


   Translators are then responcible for captures that go on... this is common
   code, so I'm not sure that that is where the capture should be.  One can
   either capture raw or cooked, inbound and/or outbound data.

      Change Attrib
      output(window,dimensions,buffer,...)



































































  Hello Love!
     Well it's 8-2 now, and either somewhere in between here and there your
  letter, which you said you would write (saturday?), is lost, or you have not
  written it... shrug, I'm writing...
     I wish I had been able to talk with you longer, but unfortunatly I didn't
  get a chance to... such is life... you and I should get together as long
  lost friends if nothing else, I would love to talk to you about all the
  things that I see, and what you think of them...
     Anyhow, Now you have met Charlene and Lightfoot, although he was a little
  more calm than she was... I tried extremely hard to get her over her hate so
  that she might be a little more calm, but oh well it ran deeper than I
  thought.  I really do apologize if I have brought you upsetness because of
  my visit.  I have so much to say and so little that I can recall, so bear
  with me.  First I have to address some things...
     One, from what I heard, you didn't care for me as anything other than a
  dude, as something to just pass a little time with... I object, and I hope
  that you may now realize how much I'm not just a "dude".  Remember, we had
  a lot of talk about "manly men" and "bitchy women"?  I remember quite well
  even after all the drinking and smoking I have done since.  You have power,
  I do concur with that.  When you started going out with Don, you said,
  "maybe I'll give someone something this time, rather than taking..."  Do
  you remember?  Okay, I may be sounding egotistical, but, whatever happens
  around me is usually becuase of my will... You taught me something a while
  ago... we were talking about Pisces, that being my Zodiac, and you said that
  they were very wishy-washy in their lives and emotions, but strong in
  whatever they have picked up at the time... To me that was a flaw, and I have
  fixed that... If you hadn't noticed, seeing as to how I drove 1060 miles
  just to be able to see you for 20 minutes.  (more about the trip later)
  Between the two of us, if we could just come up with some sort of compromise,
  I don't see there is anything that couldn't be done... I can see that at
  sometime in the past, you may not have cared, and you may not even now, but
  I would wish that there was something that I could share that would let you
  care for me... I would rather have you on my team than just a stray player
  out there in the world...
     Okay, here's a good starting point... The trip... remember you called,
  collect, to my work, and we talked a little.  I said that I was putting in
  for vacation for the 17th of July, and had in fact just that day turned
  in the papers and had it approved?  so that was how long ago I started
  planning this... as time progressed, it seemed as if the trip would never
  get here, then it was a week before and there wasn't enough time to do
  anything becuase it felt like it was rushing up quickly now.  Anyhow,
  we left 1:30 or so Friday, and arrived 10:30 Saturday morning, it was
  1060 miles as I said before that I drove straight to see you... I even
  forgot the time change, so I actually showed up at the jail an hour early...
  which if you think about it is better than showing up an hour late.
  We spent the night in Galveston, and left sunday about 11:30 to drive
  to Virginia 1560 miles more...We were going up there to visit with Charlene's
  best friend for a while before coming home the next saturday another 900
  miles away...  So that was our trip, most of the travel was pretty
  interesting, but then as I've said before, I've always LOVED to travel.
     I do have to tell you it was definatly extrememly interesting to run
  into Don's mother... it was like, what do I say when she says, "who are you?"
  "well uhh, I'm Jim, Jennifer's lover..." was what I thought, but actually
  it never came up, she just asked where we were from, and things like that,
  so when we said Florida, she said, well tell her that I came up here with
  the girls to see her, and tell her that Don wants her to write (I enjoyed
  your comment at that, snicker).
     Well, anyhow, it was a relief that Charlene didn't really react un-
  expectedly...She was very upset, and demanded that we go back to the hotel,
  she was waiting outside, though she says if she knew she had the keys that
  she would have left...anyhow, it was the first time that I actually remained
  calm while she yelled and screamed irrationally at me... and I didn't have
  to become irrational also.  Well it pretty much looks like if I continued
  to love you, as I do, then she would probably leave, and see there is no
  way to fix that save one which I will say long after this... Anyhow, I
  wouldn't destroy love in order that another might find comfort, that just
  isn't kosher, and she couldn't ask me to destroy it either.  However, if
  this were the perfect world, and everything was exactly the best that it
  could be, there is another solution rather than having either one of you
  leave.  And, before I mention it, I want you to know that it's not for
  any of the reasons that a "manyly man" would want it... My wish is for the
  two of you to be friends... Then I could love both of you, and not have
  either of you mind...(Love being expressed in whatever way that it should
  be, not nessesarily sex), But as it stands Charlene holds a lot of hatred
  towards you merely becuase she beleives that you didn't give a damn about
  anyone on the other side of the fence, which is true that you didn't, as
  you've said but I don't really believe that to be true... and my question
  to you is- is what you said to Charlene about not having cared, true now,
  or were youjust expressing what WAS true, because that is what she asked
  for?


