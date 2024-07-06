# How does it work

## Architecture

c.f. [doc](https://ibkrcampus.com/ibkr-api-page/twsapi-doc/#architecture)

- `EWrapper`: the interface used by TWS to deliver information to the API client application
    `nextValidID`: member used to indicate that the connection is completed and other messages can be sent from the API client to TWS.
- `EClientSocket`: class used to send messages to TWS
    - `eConnect`: member used to make the connection
- `EReaderSignal`: object used in APIs to signal a message is ready for processing in the queue.
- `EReader`:  class which reads and parses raw messages from TWS.

The API programs have 3 threads: a reader thread, a queue thread, and then a wrapper thread.

Test fails on ST_CONTRACTOPERATION_ACK where msg is always empty

Do not put breakpoints in the tws_api source code (segmentation fails)