#include <mod.h>
#include <npca.h>
#include <video.h>

windowptr output;
Node far *my_node,far *first,far *cur;
short i,p,page=0;
char ch;

#define false 0
#define true (!false)

main()
{
	output=opendisplay(5,5,60,11,BORDER|NEWLINE|NO_CURSOR,0x1f,0x1f,0x2f,"Nodes");
	my_node=Create_node(0);
	while (true)
	{
		Relinquish(-500L);
		if (keypressed(output))
		{
			ch=readch(output);
			switch(ch)
			{
				case 'n':
				case 'N':
					page++;
					break;
				case 'p':
				case 'P':
					page--;
					break;
			}

		}

		position(output,0,0);
		first=my_node;
		while (first->Tracking.prior)
			first=first->Tracking.prior;
		cur=first;
		for (p=0;p<page;p++)
			for (i=0;i<10;i++)
			{
				cur=cur->Tracking.next;
				if (!cur->Tracking.next)
				{
					page=p;
					for (i=i;i>=0;i++)
						cur=cur->Tracking.prior;
					break;
				}
			}
		for (i=0;i<10;i++)
		{
			Relinquish(0L);
			if (!cur)
			{
				displayln(output,"																													\n");
				continue;
			}
			if (cur!=my_node)
				displayln(output,"*c *h *n *h *h *h *n *n *H *n *h *h *h \n",
					cur->Tracking.Side?'E':'O',				 /*c*/
					cur->Tracking.Status,							 /*h*/
					cur->Return.Opcode,								 /*n*/
					cur->Return.Status,								 /*h*/
					(short)cur->Return.Byte_count,		 /*h*/
					cur->Return.XStatus,							 /*h*/
					cur->Node.Transport,							 /*n*/
					cur->Node.Channel,								 /*n*/
					(long)cur->Node.Node_Address,			 /*H*/
					cur->Node.Rex,										 /*n*/
					cur->Node.Options,								 /*h*/
					(short)cur->Node.Byte_count,			 /*h*/
					cur->Node.XOptions);							 /*h*/
			else
				displayln(output,"*c *h *n *h *h *h *n *n *H *n *h *h *h&\n",
					cur->Tracking.Side?'E':'O',
					cur->Tracking.Status,
					cur->Return.Opcode,
					cur->Return.Status,
					(short)cur->Return.Byte_count,
					cur->Return.XStatus,
					cur->Node.Transport,
					cur->Node.Channel,
					(long)cur->Node.Node_Address,
					cur->Node.Rex,
					cur->Node.Options,
					(short)cur->Node.Byte_count,
					cur->Node.XOptions);
			cur=cur->Tracking.next;
		}
	}
}
