# VSYS_Mailserver

Usage:

## Mail Client

Needs IP ( 127.0.0.1 or localhost ) and Port Number ( 6543 ) as parameters.

### SEND-Request
1. Type 'SEND' or 'send' to start the SEND-Request.
2. Then add your information to the console prompt.
3. If everything is correct, the message gets sent to the server. 
4. Wrong or invalid input cancels the SEND-Request.

If you want to send another one, start over.

## Mail Server

Needs Port Number (6543) as parameter.

### SEND-Request
1. If the server receives 'SEND' or 'send', he will save the message in a csv, and the message
metadate ( sender, recipient, timestamp ) in data.csv
2. If everything went well, the server simply responds with 'OK'.


 
