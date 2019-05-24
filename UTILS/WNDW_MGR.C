
#include "mod.h"
#include "video.h"

#define FALSE 0
#define TRUE (!FALSE)

/*-------------- window management declarations --------------------*/

static windowptr my_window,temp_window;

static short window_count=4;

/*------------------------------------------------------------------*/


void main()
{

	my_window=opendisplay(60,10,21,window_count,NO_CURSOR|BORDER|NEWLINE
											 ,WHITE|ON_BLUE				 /* data	 */
											 ,LT_CYAN|ON_BLUE			 /* border */
											 ,0x00								 /* curser */
											 ,"wndw_mgr");

	while(TRUE)
		{ temp_window=my_window;
			while(temp_window->previous_window)temp_window=temp_window->previous_window;

			/* we now have bottom window in temp_window */

			window_count=1;
			while(temp_window->next_window)
				{ temp_window=temp_window->next_window;
					window_count++;
				}

			/* we now have top window in temp_window
						and number of windows in window_count */

			if((window_count != (my_window->height+1) ) &&
				 (!my_window->next_window	 )					)
				{
					closedisplay(my_window);
					my_window=opendisplay(60,10,21,window_count
											 ,NO_CURSOR|BORDER|NEWLINE
											 ,WHITE|ON_BLUE
											 ,LT_CYAN|ON_BLUE
											 ,0x00
											 ,"wndw_mgr");
				}

			window_count=0;
			do{ position(my_window,0,window_count);
					displayln(my_window,"*3d *s",window_count,temp_window->title);
					clr_display(my_window,2); /* Erase to EOL */

					temp_window=temp_window->previous_window;
					window_count++;

				}while (temp_window && (window_count <= my_window->height) );


			Relinquish(-1000L);

		}
}
