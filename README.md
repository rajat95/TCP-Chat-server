# TCP-Chat-server
THis is a group chat server.
Server takes port number as CLA.

When client sends a message (not starting with GROUP), it is sent to every client conected to server.

When client sends a message GROUP$ <no>, server adds the client to group named <no>.

When client sends a message GROUPMSG$<no>$<msg>, server sends the message<msg> to all the clients in the group named <no>.
