GAS REAPER
===========

GAS reaper is a tool to block all GAS communication, It will monitor a specific channel to make this channel GAS silent. When received a GAS initial request, it will soon transmit a fake GAS intial response to mislead the requesting station that the target AP cannot access to advertisement server. As the protocol suggests, it may soon kill the GAS dialog with a *FAIL* notification.

Notice: When use this tool, you should keep your interface in promisc mode. Not every driver support wireless promisc, you may need some modification. When encountering this, you can contact me.


License: [GPL2](http://www.gnu.org/licenses/gpl-2.0.html)

Author: [Li Shijian](http://github.com/furtherLee)